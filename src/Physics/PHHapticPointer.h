#ifndef PH_HAPTICPOINTER_H
#define PH_HAPTICPOINTER_H

#include <Springhead.h>
#include <Physics/PHSolid.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticPointer
struct PHHapticPointerSt : public PHHapticPointerDesc{
	Vec3d last_dr;
	Vec3d last_dtheta;
	Posed proxyPose;	// ���C�v�Z�p�̃v���L�V
	Posed lastProxyPose;
	Posed targetProxy;	// ���͌v�Z�p�̃v���L�V
};
class PHHapticPointer : public PHHapticPointerSt, public PHSolid{
	SPR_OBJECTDEF(PHHapticPointer);
	ACCESS_DESC(PHHapticPointer);
protected:
	int pointerID;
	int pointerSolidID;
	float reflexSpring, reflexDamper;
	float reflexSpringOri, reflexDamperOri;
	float localRange;
	double posScale;
	Posed defaultPose;
	double rotaionalWeight;
	SpatialVector hapticForce;

public:
	bool bDebugControl;		// physicsLoop����͊o�|�C���^�𑀍삷�邽�߂̃t���O
	bool bDirectControl;	// hapticLoop����͊o�|�C���^�𑀍삷�邽�߂̃t���O
	bool bForce;
	bool bFriction;
	bool bVibration;
	bool bMultiPoints;
	bool bVirtualCoupling;
	HapticRenderMode hapticRenderMode;
	std::vector<int> neighborSolidIDs;
	PHSolid hiSolid;
	PHSolidIf* vcSolid;
	HIBaseIf* humanInterface;
	PHHapticPointer();
	PHHapticPointer(const PHHapticPointer& p);

	//API
	void	SetHumanInterface(HIBaseIf* hi){ humanInterface = hi; }
	void	SetHapticRenderMode(HapticRenderMode m){ hapticRenderMode = m; }
	void	EnableForce(bool b){ bForce = b; }
	void	EnableFriction(bool b){ bFriction = b; }
	void	EnableMultiPoints(bool b){ bMultiPoints = b; }
	void	EnableVibration(bool b){ bVibration = b; }
	void	EnableDebugControl(bool b){ bDebugControl = b; }
	void	EnableDirectControl(bool b){ bDirectControl = b; }
	void	SetReflexSpring(float s){ reflexSpring = s; }
	float	GetReflexSpring(){ return reflexSpring; }
	void	SetReflexDamper(float d){ reflexDamper = d; }
	float	GetReflexDamper(){ return reflexDamper; }
	void	SetReflexSpringOri(float s){ reflexSpringOri = s; }
	float	GetReflexSpringOri(){ return reflexSpringOri; }
	void	SetReflexDamperOri(float d){ reflexDamperOri = d; }
	float	GetReflexDamperOri(){ return reflexDamperOri; }
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

	// Implementation
	void	SetPointerID(int id){ pointerID = id; }
	int		GetPointerID(){ return pointerID; }
	void	SetSolidID(int id){ pointerSolidID = id; }
	int		GetSolidID(){ return pointerSolidID; }
	void	UpdateInterface(float dt);
	void	UpdateDirect();
	void	AddHapticForce(SpatialVector f);
	void	DisplayHapticForce();
};
class PHHapticPointers : public std::vector< UTRef< PHHapticPointer > >{};

}
#endif