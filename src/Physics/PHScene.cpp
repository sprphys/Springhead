#include "Physics.h"
#pragma hdrstop
#include <sstream>
#include <FLOAT.H>



namespace Spr{;

//----------------------------------------------------------------------------
//	PHScene
OBJECTIMP(PHScene, Scene);

PHScene::PHScene(PHSdkIf* s){
	Init();
	sdk = s;
}
PHScene::PHScene(){
	Init();
}
void PHScene::Init(){
	engines.scene = this;
	timeStep = 0.005;
	Scene::Clear();
	solids = DBG_NEW PHSolidContainer;
	engines.Add(solids);
	PHGravityEngine* ge = DBG_NEW PHGravityEngine;
	engines.Add(ge);
	PHPenaltyEngine* pe = DBG_NEW PHPenaltyEngine;
	engines.Add(pe);
}


PHSolidIf* PHScene::CreateSolid(const PHSolidDesc& desc){
	PHSolid* s = DBG_NEW PHSolid(desc);
	s->SetScene(this);
	solids->AddChildObject(s, this);	

	PHPenaltyEngine* pe;
	engines.Find(pe);
	assert(pe);
	pe->Add(s);
	pe->Init();

	PHGravityEngine* ge;
	engines.Find(ge);
	assert(ge);
	ge->AddChildObject(s);

	return s;
}
PHSolidIf* PHScene::CreateSolid(){
	PHSolidDesc def;
	return CreateSolid(def);
}

void PHScene::SetGravity(const Vec3d& g){
	//GravityEngineが無ければ作成
	PHGravityEngine* ge;
	engines.Find(ge);

}

int PHScene::GetNSolids(){
	return solids->solids.size();
}
PHSolidIf** PHScene::GetSolids(){
	return (PHSolidIf**)&*solids->solids.begin();
}

void PHScene::Clear(){
	Scene::Clear();
	count = 0;
}

void PHScene::SetTimeStep(double dt){
	timeStep = dt;
}

void PHScene::Step(){
	ClearForce();
	GenerateForce();
	Integrate();
}
void PHScene::ClearForce(){
	engines.ClearForce();
}
#define FP_ERROR_MASK	(_EM_INEXACT|_EM_UNDERFLOW)
void PHScene::GenerateForce(){
//	_controlfp(FP_ERROR_MASK, _MCW_EM);	//	コメントをはずすと例外が起きる．要調査
	engines.GenerateForce();
//	_controlfp(_MCW_EM, _MCW_EM);
}
void PHScene::Integrate(){
//	_controlfp(FP_ERROR_MASK, _MCW_EM);
	engines.Integrate();
//	_controlfp(_MCW_EM, _MCW_EM);
//	time += timeStep;
	count++;
}

void PHScene::SetGravity(Vec3f accel){
	PHGravityEngine* ge;
	engines.Find(ge);
	assert(ge);
	ge->accel = accel;
}
Vec3f PHScene::GetGravity(){
	PHGravityEngine* ge;
	engines.Find(ge);
	assert(ge);
	return ge->accel;
}


}
