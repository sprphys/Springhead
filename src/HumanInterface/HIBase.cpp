/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "HumanInterface.h"
#include <HumanInterface/SprHIBase.h>
#include <HumanInterface/SprHIDevice.h>
#ifdef _WIN32
 #pragma hdrstop
#endif

namespace Spr {;
//-----------------------------------------------------------------
//	HIBase
//

int HIBase::deviceUpdateStep;
HIBase::RealDevices& HIBase::GetRealDevices(){
	static RealDevices realDevices;
	return realDevices;
}
HISdkIf* HIBase::GetSdk(){
	return GetNameManager()->Cast();
}

void HIBase::AddDeviceDependency(HIRealDeviceIf* rd){
	GetRealDevices().insert(rd->Cast());
}
void HIBase::ClearDeviceDependency(){
	GetRealDevices().clear();
}
void HIBase::Update(float dt){
	updateStep ++;
	if (updateStep > deviceUpdateStep){
		for(std::set<HIRealDevice*>::iterator it = GetRealDevices().begin(); it != GetRealDevices().end(); ++it){
			(*it)->Update();
		}
		deviceUpdateStep = updateStep;
	}else{
		updateStep = deviceUpdateStep;
	}
}

//-----------------------------------------------------------------
//	HIPosition
//
Vec3f HIPosition::GetPosition(){
	return Vec3f();
}

//-----------------------------------------------------------------
//	HIOrientation
//
Quaternionf HIOrientation::GetOrientation(){
	return Quaternionf();
}

//-----------------------------------------------------------------
//	HIPose
//
Quaternionf HIPose::GetOrientation(){
	return Quaternionf();
}
Posef HIPose::GetPose(){
	Posef rv;
	rv.Ori() = GetOrientation();
	rv.Pos() = GetPosition();
	return rv;
}


}	//	namespace Spr
