﻿#ifndef PH_HAPTICPOINTER_H
#define PH_HAPTICPOINTER_H

//#include <Springhead.h>
#include <Physics/PHSolid.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticPointer
//	Sync時にHaptic→Physicsにコピーすべき状態
struct PHHapticPointerHapticSt{
	Vec3d last_dr;
	Vec3d last_dtheta;
	Posed proxyPose;			// 反力計算用のProxyの姿勢	IRsのLCPで追い出した後の位置
	SpatialVector proxyVelocity;// Proxyの速度、ポインタ速度を射影したもの(ProxyにDyanmicsを考える DYNAMIC_CONSTRAINT 時に使用)
	Posed lastProxyPose;		// 1(haptic)ステップ前のproxyPoseの位置、デバイスの向き
	SpatialVector lastProxyVelocity;// lastProxyの速度(ProxyにDyanmicsを考える DYNAMIC_CONSTRAINT 時に使用)
	SpatialVector hapticForce;	// HapticRenderingで求めた、提示すべき力
	bool bLastContactState;		// 前回接触していればtrue
	PHHapticPointerHapticSt();
};
struct PHHapticPointerPhysicsSt {
	float frictionSpring;
	float reflexSpring;
	float reflexDamper;
	float rotationReflexSpring;
	float rotationReflexDamper;
	float localRange;
	double posScale;
	Posed defaultPose;
	double rotaionalWeight;
	bool bForce;
	bool bFriction;
	bool bTimeVaryFriction;
	bool bVibration;
	bool bMultiPoints;
	PHHapticPointerPhysicsSt();
};

class PHHapticPointer : public PHHapticPointerHapticSt, public PHHapticPointerPhysicsSt, public PHHapticPointerDesc, public PHSolid{
	friend class PHSolidPairForHaptic;
	friend class PHHapticRender;
	friend class FWHapticPointer;
public:
	SPR_OBJECTDEF(PHHapticPointer);
	ACCESS_DESC(PHHapticPointer);

protected:
	int pointerID;
	int pointerSolidID;
public:
	std::vector<int> neighborSolidIDs;
	PHSolidState hiSolidSt;						//	Haptic Interfaceの姿勢・速度
	PHHapticPointer();

	//API
	void	SetHapticRenderMode(HapticRenderMode m){ renderMode = m; }
	void	EnableForce(bool b){ bForce = b; }
	void	EnableFriction(bool b){ bFriction = b; }
	void	EnableTimeVaryFriction(bool b) { bTimeVaryFriction = b; }
	bool	IsTimeVaryFriction() { return bTimeVaryFriction;  }
	void	EnableMultiPoints(bool b){ bMultiPoints = b; }
	bool	IsMultiPoints(){ return bMultiPoints; }
	void	EnableVibration(bool b){ bVibration = b; }
	void	SetFrictionSpring(float s) { frictionSpring = s; }
	float	GetFrictionSpring() { return frictionSpring; }
	void	SetReflexSpring(float s) { reflexSpring = s; }
	float	GetReflexSpring() { return reflexSpring; }
	void	SetReflexDamper(float d) { reflexDamper = d; }
	float	GetReflexDamper() { return reflexDamper; }
	void	SetRotationReflexSpring(float s) { rotationReflexSpring = s; }
	float	GetRotationReflexSpring() { return rotationReflexSpring; }
	void	SetRotationReflexDamper(float d) { rotationReflexDamper = d; }
	float	GetRotationReflexDamper() { return rotationReflexDamper; }
	void	SetLocalRange(float r){ localRange = r; } 
	float	GetLocalRange(){ return localRange; }
	void	SetPosScale(double scale){ posScale = scale; }
	double	GetPosScale(){ return posScale; }
	void	SetRotationalWeight(double w){ rotaionalWeight = w; }
	double	GetRotationalWeight(){ return rotaionalWeight ; }
	void	SetDefaultPose(Posed p){ 
		SetPose(p);
		defaultPose = p; 
	}
	Posed	GetDefaultPose(){ return defaultPose; }

	int     NNeighborSolids() { return (int)neighborSolidIDs.size(); }
	PHSolidIf*   GetNeighborSolid(int i) {
		int j = neighborSolidIDs[i];
		return DCAST(PHSceneIf,GetScene())->GetSolids()[j];
	}
	float   GetContactForce(int i);

	// Implementation
	void	SetPointerID(int id){ pointerID = id; }
	int		GetPointerID(){ return pointerID; }
	void	SetSolidID(int id){ pointerSolidID = id; }
	int		GetSolidID(){ return pointerSolidID; }
	///	Set device vel/pose into hiSolid.
	void	UpdateHumanInterface(const Posed& pose, const SpatialVector& vel);
	///	Copy from hiSolid to the inherited solid.
	void	UpdatePointer() {
		*(PHSolidState*)this = hiSolidSt;
	}
	void	AddHapticForce(const SpatialVector& f);
	SpatialVector	GetHapticForce();

	virtual void UpdateVelocity(double dt){}
	virtual void UpdatePosition(double dt){}
};
class PHHapticPointers : public std::vector< UTRef< PHHapticPointer > >{};

}
#endif