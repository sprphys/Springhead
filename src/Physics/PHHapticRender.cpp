﻿#include <Physics/PHHapticRender.h>
#include <Physics/PHHapticEngine.h>

namespace Spr{;
PHHapticRenderInfo::PHHapticRenderInfo() {
	loopCount = 0;
	pdt = 0.02f;
	hdt = 0.001f;
	bInterpolatePose = true;
}

PHHapticRender::PHHapticRender(){
}

void PHHapticRender::HapticRendering(PHHapticRenderInfo info){
	*(PHHapticRenderInfo*)this = info;
	for(int i = 0; i < (int)pointers->size(); i++){
		PHHapticPointer* pointer = pointers->at(i);
		pointer->hapticForce.clear();
		switch (pointer->renderMode){
			case PHHapticPointerDesc::PENALTY:
				PenaltyBasedRendering(pointer);
				break;
			case PHHapticPointerDesc::CONSTRAINT:
			case PHHapticPointerDesc::DYNAMICS_CONSTRAINT:
				ConstraintBasedRendering(pointer);
				break;
		}
 		VibrationRendering(pointer);
	}
}

PHIrs PHHapticRender::CompIntermediateRepresentation(PHHapticPointer* pointer){
	PHIrs irs;
	int nNeighbors = (int)pointer->neighborSolidIDs.size();
	for(int i = 0; i < nNeighbors; i++){
		int solidID = pointer->neighborSolidIDs[i];
		PHSolidPairForHaptic* sp = sps->item(solidID, pointer->GetPointerID());
		PHIrs tempIrs = sp->CompIntermediateRepresentation(this, hsolids->at(solidID)->GetLocalSolid(), pointer);
		if(tempIrs.size() == 0) continue;
		irs.insert(irs.end(), tempIrs.begin(), tempIrs.end());
	}
	return irs;
}

// 力覚レンダリング
// （とりあえず欲しい）
// 凸形状が使えるPenaltyBasedRendering(multiple point intermediate representation)
// 凸形状が使えるconstratint based rendering
// 凸形状が使えるvirtual coupling
// （余力があれば）
// 球型ポインタのみが使えるProxy3DoF(池田くんが作った奴）
// proxyに質量を与え、stick-slip frictionが提示できるproxy simulation


// 各接触頂点の侵入量にバネ係数をかけて合計したものを出力
// バネ係数が高いと不安定になるので低めに（1000N/m)程度に抑える必要あり
void PHHapticRender::PenaltyBasedRendering(PHHapticPointer* pointer){
	// 摩擦は追加できないので、強制的にfalse
	const bool bFric = pointer->bFriction;
	if(pointer->bFriction){
		pointer->EnableFriction(false);
	}
	PHIrs irs = CompIntermediateRepresentation(pointer);
	SpatialVector outForce = SpatialVector();
	int NIrs = (int)irs.size();
	if(NIrs > 0){
		for(int i = 0; i < NIrs; i++){
			PHIr* ir = irs[i];
			Vec3d ortho = ir->depth * ir->normal;
			Vec3d dv = ir->pointerPointVel - ir->contactPointVel;
			Vec3d dvortho = dv.norm() * ir->normal;

			//float K = ir->springK / pointer->GetPosScale();
			//float D = ir->damperD / pointer->GetPosScale();
			float K  = pointer->GetReflexSpring() / pointer->GetPosScale();
			float D = pointer->GetReflexDamper() / pointer->GetPosScale();

			Vec3d addforce = K * ortho + D * dvortho;
			outForce.v() += addforce;
			
			Vec3d pointForce = -1 * addforce;
			hsolids->at(irs[i]->solidID)->AddForce(pointForce, irs[i]->contactPointW);
			PHSolid* localSolid = &hsolids->at(irs[i]->solidID)->localSolid;
			PHSolidPairForHaptic* sp = sps->item(irs[i]->solidID, pointer->GetPointerID());
			sp->force += pointForce;	// あるポインタが剛体に加える力
			sp->torque += (irs[i]->contactPointW - localSolid->GetCenterPosition()) ^ pointForce;
		}
	}
	pointer->AddHapticForce(outForce);
	pointer->EnableFriction(bFric);
}

void PHHapticRender::ConstraintBasedRendering(PHHapticPointer* pointer){
	// プロキシの状態の保存と更新
	pointer->lastProxyPose = Posed(pointer->proxyPose.Pos(), pointer->GetOrientation());

	// 中間表現を求める。摩擦状態を更新
	PHIrs irs = CompIntermediateRepresentation(pointer);
		
	SpatialVector outForce = SpatialVector();
	int Nirs = (int)irs.size();
	if(Nirs > 0){
		// プロキシ姿勢計算のための連立不等式
		VMatrixRow< double > c;
		c.resize(Nirs, Nirs);
		c.clear(0.0);
		VVector< double > d;
		d.resize(Nirs);
		d.clear(0.0);
		double massInv = pointer->GetMassInv();
		Matrix3d rotationMatInv =  (pointer->GetInertia() * pointer->GetRotationalWeight()).inv();
		for(int i = 0; i < Nirs; i++){
			for(int j = 0; j < Nirs; j++){
				Vec3d ri = irs[i]->r;
				Vec3d ni = irs[i]->normal;
				Vec3d rj = irs[j]->r;
				Vec3d nj = irs[j]->normal;

				// 並進拘束
				double trans = ni * nj * massInv;
				// 回転拘束
				double rotate = (ri % ni) *	rotationMatInv * (rj % nj);
				c[i][j] = trans + rotate;
			}
			d[i] = irs[i]->depth;
		}
		VVector< double > f;
		f.resize(Nirs);
		f.clear(0.0);

		// 連立不等式を解く
		// 球（1点接触）で摩擦ありの場合だと侵入解除に回転が
		// 含まれる。解は正しいが、プロキシの更新がうまくいかなくなるので、
		// 回転の重み行列をなるべく大きくする必要がある。
		// 回転の重み行列を大きくするとプロキシの回転移動がなくなるため、回転摩擦がでなくなる
		GaussSeidel(c, f, -d);

		// ポインタ移動量を求める
		Vec3d dr = Vec3d();
		Vec3d dtheta = Vec3d();
		Vec3d allDepth = Vec3d();
		for(int i = 0; i < Nirs; i++){
			f[i] = std::max(f[i], 0.0);
			// 並進量
			Vec3d tmpdr = f[i] * irs[i]->normal * massInv;
			dr += tmpdr;
			// 回転量
			Vec3d tmpdtheta = f[i] * rotationMatInv * (irs[i]->r % irs[i]->normal);
			dtheta += tmpdtheta;
				
			allDepth += -1 * irs[i]->normal * irs[i]->depth;
		}


		// プロキシ位置姿勢更新（目標位置姿勢解除状態）
		pointer->proxyPose.Ori() = ( Quaterniond::Rot(dtheta) * pointer->GetOrientation() ).unit();
		pointer->proxyPose.Pos() = pointer->GetFramePosition() + dr;
		
		/// 力覚インタフェースに出力する力の計算
		Vec3d last_dr = pointer->last_dr;
		Vec3d last_dtheta = pointer->last_dtheta;

		float K  = pointer->GetReflexSpring() / pointer->GetPosScale();
		float D = pointer->GetReflexDamper() / pointer->GetPosScale();
		float KOri = pointer->GetReflexSpringOri() * pointer->GetRotationalWeight();
		float DOri = pointer->GetReflexDamperOri() * pointer->GetRotationalWeight();

		outForce.v() = K * dr  + D * (dr - last_dr)/hdt;
		outForce.w() = KOri * dtheta + DOri * ((dtheta - last_dtheta)/hdt);
		pointer->last_dr = dr;
		pointer->last_dtheta = dtheta; 

		// 剛体に加える力を計算
		// レンダリングした力から各接触点に働く力を逆算
		// うまくいってない可能性がある
		Vec3d ratio;
		double epsilon = 1e-10;
		for(int i = 0; i < 3; i++){
			ratio[i] = outForce.v()[i] / allDepth[i];
			if(abs(allDepth[i]) < epsilon) ratio[i] = 0.0;
		}
		//DSTR << "all" << outForce << std::endl;
		//DSTR << "ratio" << ratio << std::endl;
		//DSTR << "NIrs" << Nirs << std::endl;
		for(int i = 0; i < Nirs; i++){
			Vec3d pointForce = Vec3d();	// 各接触点に働く力
			Vec3d dir = irs[i]->normal * irs[i]->depth;
			for(int j = 0; j < 3; j++){
				pointForce[j] = ratio[j] * dir[j];// *  hri.hdt / hri.pdt;
			}
			//DSTR << "pos" << irs[i]->contactPointW << std::endl;
			//DSTR << "depth" << irs[i]->depth << std::endl;
			//DSTR << "pointForce" << pointForce << std::endl;
			hsolids->at(irs[i]->solidID)->AddForce(pointForce, irs[i]->contactPointW);	// 各ポインタが剛体に加えた全ての力
			PHSolid* localSolid = &hsolids->at(irs[i]->solidID)->localSolid;
			PHSolidPairForHaptic* sp = sps->item(irs[i]->solidID, pointer->GetPointerID());
			sp->force += pointForce;	// あるポインタが剛体に加える力
			sp->torque += (irs[i]->contactPointW - localSolid->GetCenterPosition()) ^ pointForce;
			//DSTR << sp->force << std::endl;
			//DSTR << irs[i]->contactPointW << " " << localSolid->GetCenterPosition() << std::endl;
			//DSTR << irs[i]->contactPointW - localSolid->GetCenterPosition() << std::endl;
		}
	}
	else {
		pointer->proxyPose = pointer->GetPose();
	}

	pointer->AddHapticForce(outForce);
	//DSTR << pointer->GetFramePosition() << std::endl;
	//DSTR << "render" << outForce << std::endl;
	//CSVOUT << outForce[0] << "," << outForce[1] << "," << outForce[2] << "," << outForce[3] << "," << outForce[4] << "," << outForce[5] << "," <<std::endl;
}


void PHHapticRender::VibrationRendering(PHHapticPointer* pointer){
	if(!pointer->bVibration) return;
	int Nneigbors = (int)pointer->neighborSolidIDs.size();
	for(int j = 0; j < (int)Nneigbors; j++){
		int solidID = pointer->neighborSolidIDs[j];
		PHSolidPairForHaptic* sp = sps->item(solidID, pointer->GetPointerID());
		PHSolid* solid = hsolids->at(solidID)->GetLocalSolid(); 
		if(sp->frictionState == sp->FREE) continue;
		if(sp->contactCount == 0){
			sp->vibrationVel = pointer->GetVelocity() - solid->GetVelocity();
		}

		Vec3d vibV = sp->vibrationVel;
		double vibA = solid->GetShape(0)->GetVibA();
		double vibB = solid->GetShape(0)->GetVibB();
		double vibW = solid->GetShape(0)->GetVibW();
		double vibT = sp->contactCount * hdt;

		SpatialVector vibForce;
		// 法線方向に射影する必要がある？
		vibForce.v() = vibA * vibV * exp(-vibB * vibT) * sin(2 * M_PI * vibW * vibT) / pointer->GetPosScale();		//振動計算
		if (sp->frictionState == sp->DYNAMIC) {
			Vec3d vibV = sp->totalFrictionForce;
			double vibT = sp->fricCount * hdt;
			vibForce.v() += 1000 * vibA * vibV * exp(-vibB * vibT) * sin(2 * M_PI * vibW * vibT) / pointer->GetPosScale();		//振動計算
		}

		pointer->AddHapticForce(vibForce);
		//CSVOUT << vibForce.v().x << "," << vibForce.v().y << "," << vibForce.v().z << std::endl;
	}
}

}