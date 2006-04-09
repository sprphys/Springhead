#include <Springhead.h>
#include <Foundation/Object.h>
#include "Collision.h"
#include <set>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#include "CDQuickHull3DImp.h"

namespace Spr{;
	
//----------------------------------------------------------------------------
//	CDSphere
OBJECTIMP(CDSphere, CDConvex);	
IF_IMP(CDSphere, CDConvex);	
	
CDSphere::CDSphere() {
}

CDSphere::CDSphere(const CDSphereDesc& desc) {
	center = desc.center;
	radius = desc.radius;
}

// サポートポイントを求める
Vec3f CDSphere::Support(const Vec3f& p) const {	
	Vec3f p_ = p;
	p_ -= center;					// centerからpへのベクトルを求める
	float s = p_.norm();			// centerからpへのベクトルのlength
	if (s > 1e-8f) {		
		float r = radius / s;	
		return r*p_ + center;		// 衝突検出に必要な球の最近傍点
	}else{
		return center;
	}	
}

// 切り口を求める. 接触解析を行う.
bool CDSphere::FindCutRing(CDCutRing& r, const Posed& toW) {
	assert(0);
	return false;
}

// 球体の中心座標を取得
Vec3f CDSphere::GetCenter() {
	return center;
}	

// 球体の半径を取得
float CDSphere::GetRadius() {
	return radius;
}

}	//	namespace Spr
