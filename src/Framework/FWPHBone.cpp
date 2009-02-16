/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Foundation/UTLoadHandler.h>
#include <Foundation/UTLoadContext.h>
#include <Foundation/UTPath.h>
#include <FileIO/FISdk.h>
#include <FileIO/FIFileX.h>
#include <Graphics/GRFrame.h>
#include <Graphics/GRMesh.h>
#include <Graphics/GRSphere.h>
#include <Graphics/GRRender.h>
#include <Graphics/GRScene.h>
#include <Physics/PHSolid.h>
#include <Physics/PHJoint.h>
#include <Framework/FWOldSpringheadNodeHandler.h>
#include <Framework/FWObject.h>
#include <Framework/FWScene.h>
#include <Framework/FWSdk.h>
#include <Collision/CDConvexMesh.h>
#include <Framework/FWPHBone.h>

#include <Framework/FWOldSpringheadNode.h>
namespace SprOldSpringhead{
using namespace Spr;

BoneData::BoneData(){
	centerPoint=Vec3d(0.0,0.0,0.0);
	length=0.0;
}
BoneJointData::BoneJointData(){
	K				= 0.01;
	D1				= 0.1;
	D2				= 10;
	yieldStress		= 0.1;
	hardnessRate	= 1e3;
	SocketPos		=Vec3f(0.0,0.0,0.0);
	PlugPos			=Vec3f(0.0,0.0,0.0);
}

FWPHBone::FWPHBone(){
}

void FWPHBone::FWPHBoneCreate(){
	//desc
	desc.mass = 0.05;
	desc.inertia = 0.033 * Matrix3d::Unit();
	CDBoxDesc dBox;

	for(int i=0; i<bone.size(); ++i){
		//soBone�̍쐬
		soBone.push_back(phScene->CreateSolid(desc));
		bone[i].solid=soBone[i];
		{
			if(i==0){
				bone[i].solid->SetDynamical(false);
			}else{
				bone[i].solid->SetDynamical(true);
			}
			bone[i].solid->SetFramePosition(bone[i].centerPoint);
		}
		//shapeBone�̍쐬	
		{
			dBox.boxsize=(Vec3d(0.1,0.1,bone[i].length));
			bone[i].shapeBone=XCAST(fwSdk->GetPHSdk()->CreateShape(dBox));
		}
		bone[i].solid->AddShape(bone[i].shapeBone);
	}
	FWJointCreate();
	FWSkinMeshAdapt();
}
void FWPHBone::FWJointCreate(){
	std::vector<PH3ElementBallJointDesc> d3Ball;
	PH3ElementBallJointDesc _d3Ball;

	if (boneJoint.size()){
		for(int i=0 ;i<boneJoint.size(); ++i){
			d3Ball.push_back(_d3Ball);
			{
				d3Ball[i].poseSocket.Pos()	= boneJoint[i].SocketPos;
				d3Ball[i].posePlug.Pos()	= boneJoint[i].PlugPos;
				d3Ball[i].spring			= boneJoint[i].K;
				d3Ball[i].damper			= boneJoint[i].D1;
				d3Ball[i].secondDamper		= boneJoint[i].D2;
				d3Ball[i].yieldStress		= boneJoint[i].yieldStress;
				d3Ball[i].hardnessRate		= boneJoint[i].hardnessRate;
			}
		}
		for(int i=0;i<boneJoint.size();i++){
			phScene->SetContactMode(bone[i].solid, bone[i+1].solid, PHSceneDesc::MODE_NONE);
			phScene->CreateJoint(bone[i].solid,bone[i+1].solid, d3Ball[i]);
			Joint.push_back( phScene->CreateJoint( soBone[i], soBone[i+1], d3Ball[i]) );
		}
	}
}

void FWPHBone::FWSkinMeshAdapt(){

	for(int i=0; i<bone.size(); ++i){
		fwoBone.push_back(fwSdk->GetScene()->CreateFWObject());
		bone[i].fwObject=fwoBone[i];
		bone[i].grFrame=grfBone[i];
		//PH��GR��A��������
		bone[i].fwObject->SetPHSolid(bone[i].solid);
		bone[i].fwObject->SetGRFrame(bone[i].grFrame->Cast());
	}
}

void FWPHBone::DisplayBonePoint(){
	CDSphereDesc dSphere;
	CDSphereIf* shapeSphere;
	std::vector<PHSolidIf*> soSphere;

	for(int i=0; i<bonePoint.size(); ++i){
		//soSphere�̍쐬
		soSphere.push_back(phScene->CreateSolid(desc));
		{
			soSphere[i]->SetDynamical(false);
			soSphere[i]->SetFramePosition(bonePoint[i]);
		}
		//shapeBone�̍쐬	
		{
			dSphere.radius=0.1;
			shapeSphere=XCAST(fwSdk->GetPHSdk()->CreateShape(dSphere));
		}
		soSphere[i]->AddShape(shapeSphere);
	}
}

void FWPHBone::DisplayPHBoneCenter(){
	CDBoxDesc dBoxD;
	CDBoxIf* shapeBoxD;
	std::vector<PHSolidIf*> soBoxD;
	for(int i=0; i<bone.size(); ++i){
		//soBoxD�̍쐬
		soBoxD.push_back(phScene->CreateSolid(desc));
		{
			soBoxD[i]->SetDynamical(false);
			soBoxD[i]->SetFramePosition(bone[i].centerPoint);
		}
		//shapeBoxD�̍쐬	
		{
			dBoxD.boxsize=(Vec3d(0.1,0.1,0.1));
			shapeBoxD=XCAST(fwSdk->GetPHSdk()->CreateShape(dBoxD));
		}
		soBoxD[i]->AddShape(shapeBoxD);
	}
}
void FWPHBone::SetAffine(std::vector<Affinef> a){
	af.swap(a);
	//�A�t�B���s�񂩂�3�������W���Z�o
	Vec3d BonePoint;
	if (af.size()){
		for(int i=0 ;i<af.size(); ++i){
			BonePoint=af[i]*Vec3d(0.0,0.0,0.0);
			bonePoint.push_back(BonePoint);
			//DSTR << i << std::endl << bonePoint[i] << std::endl;
		}
	}
	//3�������W����PHBone�̍��̏����Z�o
	if (bonePoint.size()){
		for(int i=0 ;i<bonePoint.size()-2; ++i){
			bone.push_back(boneData);
			bone[i].centerPoint = (bonePoint[i+2]+bonePoint[i+1])*0.5;
			Vec3d length = bonePoint[i+2]-bonePoint[i+1];
			bone[i].length = length.norm();
			//DSTR<< i << std::endl << bone[i].centerPoint << bone[i].length << std::endl;
		}
	}
	//PHBone�̍��̏�񂩂�Joint�̃\�P�b�g�E�v���O�̈ʒu���Z�o
	if (bone.size()){
		for(int i=0 ;i<bone.size()-1; ++i){
			boneJoint.push_back(boneJointData);
			boneJoint[i].SocketPos = Vec3f(0.0,0.0, bone[i].length/2);
			boneJoint[i].PlugPos = Vec3f(0.0,0.0,-bone[i+1].length/2);
		}
	}

}
void FWPHBone::Clear(){
	af.clear();
	bonePoint.clear();
}

}