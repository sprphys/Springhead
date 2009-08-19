#include <Framework/FWInteractAdaptee.h>
#include <Framework/FWInteractScene.h>
#include <Physics/PHConstraintEngine.h>
#include <iostream>

namespace Spr{;

/** FWHapticLoopBaseの実装 */
///////////////////////////////////////////////////////////////////////////////////

void FWHapticLoopBase::Clear(){
	GetInteractSolids()->clear();
	loopCount = 1;
}

/** FWInteractAdapteeの実装 */
///////////////////////////////////////////////////////////////////////////////////
FWInteractAdaptee::FWInteractAdaptee(){}
/// INScene関係
void FWInteractAdaptee::SetINScene(FWInteractScene* iScene){ interactScene = iScene; }
FWInteractScene* FWInteractAdaptee::GetINScene(){ return interactScene; }			
PHSceneIf* FWInteractAdaptee::GetPHScene(){ return GetINScene()->GetScene()->GetPHScene(); }

/// HapticLoop関係
FWHapticLoopBase* FWInteractAdaptee::GetHapticLoop(){ return NULL; }

/// INPointer関係(INSceneを介して取得)
FWInteractPointer* FWInteractAdaptee::GetINPointer(int i){ return GetINScene()->GetINPointer(i)->Cast(); }
FWInteractPointers* FWInteractAdaptee::GetINPointers(){ return GetINScene()->GetINPointers(); }
int FWInteractAdaptee::NINPointers(){ return GetINScene()->NINPointers(); }

/// INSolid関係(INSceneを介して取得)
FWInteractSolid* FWInteractAdaptee::GetINSolid(int i){ return GetINScene()->GetINSolid(i); }
FWInteractSolids* FWInteractAdaptee::GetINSolids(){ return GetINScene()->GetINSolids(); }
int FWInteractAdaptee::NINSolids(){ return GetINScene()->NINSolids(); }

void FWInteractAdaptee::UpdateSolidList(){
	PHSceneIf* phScene = GetPHScene(); 
	PHSolidIf** phSolids = phScene->GetSolids();
	for(int i = NINSolids(); i < phScene->NSolids(); i++){
		GetINSolids()->resize(i + 1);
		GetINSolids()->back().sceneSolid = phSolids[i]->Cast();
		for(int j = 0; j < NINPointers(); j++){
			GetINPointer(j)->interactInfo.resize(i + 1);
		}
	}
}

void FWInteractAdaptee::NeighborObjectFromPointer(){
	// GJKを使って近傍物体と近傍物体の最近点を取得
	// これをすべてのshapeをもつ剛体についてやる
	// AABBで力覚ポインタ近傍の物体を絞る
	// ここで絞った物体についてGJKを行う．ここで絞ることでGJKをする回数を少なくできる．
	// SolidのBBoxレベルでの交差判定(z軸ソート)．交差のおそれの無い組を除外		
	//1. BBoxレベルの衝突判定
	FWInteractSolid*	inSolid;
	FWInteractPointer*	inPointer;
	FWInteractInfo*		inInfo;

	Vec3f dir(0,0,1);
	Edges edges;
	edges.resize(2 * NINSolids());
	Edges::iterator eit = edges.begin();

	for(int i = 0; i < NINSolids(); i++){
		for(int j = 0; j < NINPointers(); j++){
			GetINPointer(j)->interactInfo[i].flag.bneighbor = false;
		}
		PHSolid* phSolid = GetINSolid(i)->sceneSolid;
		phSolid->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		Vec3d dPos = phSolid->GetDeltaPosition();
		float dLen = (float) (dPos * dir);
		if (dLen < 0){
			eit[0].edge += dLen;
		}else{
			eit[1].edge += dLen;
		}
		eit[0].index = i; eit[0].bMin = true;
		eit[1].index = i; eit[1].bMin = false;
		eit += 2;
	}
	sort(edges.begin(), edges.end());

	//端から見ていって，接触の可能性があるノードの判定をする．
	typedef std::set<int> SolidSet;
	SolidSet cur;							//	現在のSolidのセット
	bool found = false;
	for(int i = 0; i < NINPointers(); i++){
		inPointer = GetINPointer(i);
		PHSolid* soPointer = DCAST(PHSolid, inPointer->GetPointerSolid());
		for(Edges::iterator it = edges.begin(); it != edges.end(); ++it){
			///	初端だったら，リスト内の物体と判定
			if (it->bMin){						
				for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
					int f1 = it->index;
					int f2 = *itf;
					if (f1 > f2) std::swap(f1, f2);
					/// 近傍物体の可能性あり，詳細判定へ
					if(GetINSolid(f1)->sceneSolid == soPointer){
						inPointer->interactInfo[f2].flag.bneighbor = true;
						UpdateInteractSolid(f2, inPointer);
					}else if(GetINSolid(f2)->sceneSolid == soPointer){
						inPointer->interactInfo[f1].flag.bneighbor = true;
						UpdateInteractSolid(f1, inPointer);
					}
				}
				cur.insert(it->index);
			}else{
				///	終端なので削除．
				cur.erase(it->index);	
			}
		}
	}
	
	for(int i = 0; i < NINSolids(); i++){
		int lCount = 0;					///< flag.blocalの数
		int fCount = 0;					///< flag.bfirstlocalの数									
		for(int j = 0; j < NINPointers(); j++){
			inInfo = &GetINPointer(j)->interactInfo[i];
			/// bneighborかつblocalであればlCount++
			if(inInfo->flag.bneighbor){
				if(inInfo->flag.blocal){
					lCount++;
					/// さらにbfirstlocalであればfCount++
					if(inInfo->flag.bfirstlocal){
						fCount++;
						inInfo->flag.bfirstlocal = false;
					}
				}else{
					inInfo->flag.bfirstlocal = false;							
					inInfo->flag.blocal = false;													
				}
			}else{
				/// 近傍物体でないのでfalseにする
				inInfo->flag.bfirstlocal = false;							
				inInfo->flag.blocal = false;									
			}
		}
		/// 初シミュレーションの処理フラグをtrueにする
		inSolid = GetINSolid(i);
		if(fCount > 0){
			inSolid->bfirstSim = true;	
			inSolid->copiedSolid = *inSolid->sceneSolid;	// シーンが持つ剛体の中身を力覚プロセスで使う剛体（実体）としてコピーする
		}
		/// シミュレーションの処理フラグをtrueにする
		if(lCount > 0){
			inSolid->bSim = true;					
		}else{
			/// それ以外は全てfalseにする
			inSolid->bSim = false;					
			inSolid->bfirstSim = false;
		}
	}
			DSTR << "-------------------------------" << std::endl;
}

void FWInteractAdaptee::UpdateInteractSolid(int index, FWInteractPointer* inPointer){
	PHSolid* phSolid = GetINSolid(index)->sceneSolid;
	PHSolid* soPointer = inPointer->GetPointerSolid()->Cast();
	FWInteractInfo* inInfo = &inPointer->interactInfo[index]; 

	if (!phSolid->NShape()==0){													///< 形状を持たない剛体の場合は行わない
		CDConvexIf* a = DCAST(CDConvexIf, phSolid->GetShape(0));				///< 剛体が持つ凸形状
		CDConvexIf* b = DCAST(CDConvexIf, soPointer->GetShape(0));				///< 力覚ポインタの凸形状
		Posed a2w = phSolid->GetPose();											///< 剛体の姿勢
		Posed b2w = soPointer->GetPose();										///< 力覚ポインタの姿勢
		Vec3d dir = -1.0 * inInfo->neighborInfo.face_normal;
		Vec3d cp = phSolid->GetCenterPosition();								///< 剛体の重心
		Vec3d normal;															///< 剛体から力覚ポインタへの法線(ワールド座標)
		Vec3d pa, pb;															///< pa:剛体の近傍点，pb:力覚ポインタの近傍点（ローカル座標）

		/// GJKを使った近傍点探索
		double r = FindNearestPoint(a, b, a2w, b2w, dir, cp, normal, pa, pb);	
		
		/// 近傍点までの長さから近傍物体を絞る
		if(r < inPointer->GetLocalRange()){
			/// 初めて最近傍物体になった場合
			if(inInfo->flag.blocal == false){																
				inInfo->flag.bfirstlocal = true;													
				inInfo->neighborInfo.face_normal = normal;	// 初めて近傍物体になったので，前回の法線に今回できた法線を上書きする．										
				#ifdef _DEBUG
					if (inInfo->neighborInfo.face_normal * normal < 0.8){
						DSTR << "Too big change on normal = " << normal << std::endl;
					}
				#endif
			}
			/// 初めて近傍または継続して近傍だった場合
			inInfo->flag.blocal = true;								// 近傍物体なのでblocalをtrueにする
			inInfo->neighborInfo.closest_point = pa;				// 剛体近傍点のローカル座標
			inInfo->neighborInfo.pointer_point = pb;				// 力覚ポインタ近傍点のローカル座標
			inInfo->neighborInfo.last_face_normal = inInfo->neighborInfo.face_normal;		// 前回の法線(法線の補間に使う)，初めて近傍になった時は今回できた法線
			inInfo->neighborInfo.face_normal = normal;				// 剛体から力覚ポインタへの法線
		}else{
			/// 近傍物体ではないのでfalseにする
			inInfo->flag.bneighbor = false;
			inInfo->flag.bfirstlocal = false;						
			inInfo->flag.blocal = false;																
		}
	}
}

double FWInteractAdaptee::FindNearestPoint(const CDConvexIf* a, const CDConvexIf* b,
											const Posed& a2w, const Posed& b2w, const Vec3d pc, Vec3d& dir, 
											Vec3d& normal, Vec3d& pa, Vec3d& pb){
	/// GJKで近傍点を求め，力覚ポインタ最近傍の物体を決定する
	CDConvex* ca = DCAST(CDConvex, a);
	CDConvex* cb = DCAST(CDConvex, b);
	FindClosestPoints(ca, cb, a2w, b2w, pa, pb);	///< GJKで近傍点の算出
	Vec3d wa = a2w * pa;							///< 剛体近傍点のワールド座標
	Vec3d wb = b2w * pb;							///< 力覚ポインタ近傍点のワールド座標
	Vec3d a2b = wb - wa;							///< 剛体から力覚ポインタへのベクトル
	normal = a2b.unit();
	/// 力覚ポインタと剛体がすでに接触していたらCCDGJKで法線を求める
	if(a2b.norm() < 0.01){									
		pa = pb = Vec3d(0.0, 0.0, 0.0);
		/// dirが潰れてしまっている場合は剛体重心から近傍点へのベクトルとする
		if(dir == Vec3d(0.0, 0.0, 0.0) ){
			dir = wa - pc;
		}
		double dist = 0.0;
		/// CCDGJKの実行
		int cp = ContFindCommonPoint(ca, cb, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
		/// CCDGJKが失敗した場合の処理
		if(cp != 1){
			ContFindCommonPointSaveParam(ca, cb, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
			DSTR << "ContFindCommonPoint do not find contact point" << std::endl;
		}
	}
	return a2b.norm();
}


}