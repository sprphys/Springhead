#include <Physics/PHHapticEngineImpulse.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticLoopImpulse

// 1/7�͂�������
void PHHapticLoopImpulse::Step(){
	UpdateInterface();
	HapticRendering();
}

void PHHapticLoopImpulse::HapticRendering(){
	PHHapticRenderInfo info;
	info.pointers = GetHapticPointers();
	info.hsolids = GetHapticSolids();
	info.sps = GetSolidPairsForHaptic();
	info.hdt = GetHapticTimeStep();
	info.pdt = GetPhysicsTimeStep();
	info.loopCount = loopCount;
	info.bInterpolatePose = true;
	GetHapticRender()->HapticRendering(info);
}


//----------------------------------------------------------------------------
// PHHapticEngineImpulse
PHHapticEngineImpulse::PHHapticEngineImpulse(){ 
	hapticLoop = &hapticLoopImpulse;
	hapticLoop->engineImp = this;
}

void PHHapticEngineImpulse::Step1(){};
void PHHapticEngineImpulse::Step2(){
	for(int i = 0; i < NHapticSolids(); i++){
		PHSolid* solid = GetHapticSolid(i)->sceneSolid;
		if(i == 1)
			CSVOUT << solid->GetVelocity().y << "," << solid->GetFramePosition().y  << std::endl;
	}
	engine->StartDetection();
}

void PHHapticEngineImpulse::SyncHaptic2Physic(){
	// physics <------ haptic
	// PHSolidForHaptic�̓���
	// PHSolidPairForHaptic(�͊o�|�C���^�ƋߖT�̕���)�̊e����̓���
	for(int i = 0; i < hapticLoop->NHapticPointers(); i++){
		PHHapticPointer* hpointer = hapticLoop->GetHapticPointer(i);
		int hpointerID = hpointer->GetPointerID();
		int nNeighbors = hpointer->neighborSolidIDs.size();
		// �ߖT���̂ł���y�A��������
		for(int j = 0; j < nNeighbors; j++){
			int solidID = hpointer->neighborSolidIDs[j];
			PHSolidPairForHaptic* hpair = hapticLoop->GetSolidPairForHaptic(solidID, hpointerID);
			PHSolidPairForHaptic* ppair = GetSolidPairForHaptic(solidID, hpointerID);
			PHSolidPairForHapticSt* hst = (PHSolidPairForHapticSt*)hpair;
			PHSolidPairForHapticSt* pst = (PHSolidPairForHapticSt*)ppair;
			*pst = *hst;	// haptic���ŕێ����Ă����ׂ����𓯊�
		}
	}
	// �����_�����O�����͂��V�[���ɔ��f
	for(int i = 0; i < (int)hapticLoop->NHapticSolids(); i++){
		PHSolidForHaptic* hsolid = hapticLoop->GetHapticSolid(i);
		if(hsolid->bPointer) continue;
		PHSolid* sceneSolid = hsolid->sceneSolid;
		sceneSolid->AddForce(hsolid->force * GetHapticTimeStep() / GetPhysicsTimeStep());
		sceneSolid->AddTorque(hsolid->torque * GetHapticTimeStep() / GetPhysicsTimeStep());
		hsolid->force.clear();
		hsolid->torque.clear();
	}
}

void PHHapticEngineImpulse::SyncPhysic2Haptic(){
	// haptic <------ physics
	// PHSolidForHaptic�̓���
	for(int i = 0; i < NHapticSolids(); i++){
		PHSolidForHaptic* psolid = GetHapticSolid(i);
		PHSolidForHaptic* hsolid = hapticLoop->GetHapticSolid(i);
		*psolid->GetLocalSolid() = *psolid->sceneSolid;	//	impulse�̏ꍇ�͏펞scene�ŊǗ�����Ă���solid�Ɠ���				
		*hsolid = PHSolidForHaptic(*psolid);			// LocalDynamics�̏ꍇ��dosim�ɂ���ē�������������K�v������
	}
	// solidpair, shapepair�̓���
	// �ߖT���̂̂ݓ���������
	for(int i = 0; i < NHapticPointers(); i++){
		PHHapticPointer* ppointer = GetHapticPointer(i);
		const int ppointerID = ppointer->GetPointerID();
		const int nNeighbors = ppointer->neighborSolidIDs.size();
		for(int j = 0; j < nNeighbors; j++){
			const int solidID = ppointer->neighborSolidIDs[j];
			PHSolidPairForHaptic* hpair = hapticLoop->GetSolidPairForHaptic(solidID, ppointerID);
			PHSolidPairForHaptic* ppair = GetSolidPairForHaptic(solidID, ppointerID);
			*hpair = PHSolidPairForHaptic(*ppair);
		}
	}
}

}