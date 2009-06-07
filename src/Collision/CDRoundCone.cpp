/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
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
//	CDRoundCone
CDRoundCone::CDRoundCone() {
}

CDRoundCone::CDRoundCone(const CDRoundConeDesc& desc){
	radius = desc.radius;
	length = desc.length;
	material = desc.material;
}

bool CDRoundCone::IsInside(const Vec3f& p){
	return (p - Vec3f(0.0f, 0.0f, 0.5f * -length)).square() < radius[0] * radius[0] ||
		   (p - Vec3f(0.0f, 0.0f, 0.5f * +length)).square() < radius[1] * radius[1] ||
		   (p.x*p.x + p.y*p.y < radius * radius && -0.5f*length < p.z && p.z < 0.5f*length);
}
float CDRoundCone::CalcVolume(){
	return  2.0f/3.0f * (float)M_PI * radius[0] * radius[0] * radius[0] +
			2.0f/3.0f * (float)M_PI * radius[1] * radius[1] * radius[1] +
			(float)M_PI * (radius[0]*radius[0] + radius[0]*radius[1] + radius[1]*radius[1]) * length / 3.0f;
}
	
// サポートポイントを求める
Vec3f CDRoundCone::Support(const Vec3f& p) const {
	float normal_Z = (radius[0] - radius[1]) / length;
	float n = p.norm();
	Vec3f dir;
	if(n < 1.0e-10f){
		dir = Vec3f();
	}else{
		dir = p / n;
	}

	if (-1 < normal_Z && normal_Z < 1) {
		if (normal_Z < dir.Z()) {
			// pの方がZ軸前方 → radius[1]を使用
			return dir*radius[1] + Vec3f(0,0, length/2.0);
		} else {
			// pの方がZ軸後方 → radius[0]を使用
			return dir*radius[0] + Vec3f(0,0,-length/2.0);
		}
	} else {
		// どちらかの球に包含されている
		if (radius[0] < radius[1]) {
			return dir*radius[1] + Vec3f(0,0, length/2.0);
		} else {
			return dir*radius[0] + Vec3f(0,0,-length/2.0);
		}
	}
}

// 切り口を求める. 接触解析を行う.
bool CDRoundCone::FindCutRing(CDCutRing& ring, const Posed& toW) {
	//	切り口(ring.local)系での カプセルの向き
	Vec3f dir = ring.localInv.Ori() * toW.Ori() * Vec3f(0,0,1);
	Vec3f center = ring.localInv * toW.Pos();
	float sign = center.X() > 0.0f ? 1.0f : -1.0f;
	//	sinA : (r1-r0)/length になる。
	//	sinB : Cutring面と円筒面の線とのなす角が B
	//	sinA+B = dir.X() になる。
	float sinA = (radius[1]-radius[0]) / length;
	float sinB = dir.X()*sign * sqrt(1-sinA*sinA)  -  sqrt(1-dir.X()*dir.X()) * sinA;

	float r = radius[0];
	if (sinB < 0){
		dir = -dir;
		sinA *= -1;
		sinB *= -1;
		r = radius[1];
	}
	center = center - (length/2) * dir;
	if (sinB < 0.3f) { // 側面が接触面にほぼ平行な場合
		float shrink = sqrt(1-dir.X()*dir.X());	//	傾いているために距離が縮む割合
		float start = -0.0f*length*shrink;
		float end = 1.0f*length*shrink;

		if (sinB > 1e-4){	//	完全に平行でない場合
			float depth = r/shrink - sign*center.X();
			float cosB = sqrt(1-sinB*sinB);
			float is = depth / sinB * cosB;	//	接触面と中心線を半径ずらした線との交点
			if (is < end) end = is;
			if (end+1e-4 < start){//0.001 < start){
				DSTR << "CDRoundCone::FindCutRing() may have a problem" << std::endl;
			}
			if (end <= start) return false;
		}
		//	ringに線分を追加
		float lenInv = 1/sqrt(dir.Y()*dir.Y() + dir.Z()*dir.Z());
		ring.lines.push_back(CDCutLine(Vec2f(-dir.Y(), -dir.Z())*lenInv, -start));
		ring.lines.push_back(CDCutLine(Vec2f(dir.Y(), dir.Z())*lenInv, end));
		ring.lines.push_back(CDCutLine(Vec2f(dir.Z(), -dir.Y())*lenInv, 0));
		ring.lines.push_back(CDCutLine(Vec2f(-dir.Z(), dir.Y())*lenInv, 0));
		return true;
	}
	return false;
}

Vec3d CDRoundCone::Normal(Vec3d p){
	Vec2d	r = radius;
	double	l = length;

	// RoundConeの側面の角度（側面がZ軸に垂直なとき0°、平行(つまりカプセル型)のとき90°）
	double theta = acos((r[1]-r[0])/l);

	if ( (p[2] > (r[0]*cos(theta) + l/2.0)) || (p[2] < (r[1]*cos(theta) - l/2.0)) ) {
		// 接触点がどちらかの球体にある場合：
		return p.unit();
	} else {
		// 接触点が球体と球体の間にある場合：
		Vec3d pNormal = p;
		pNormal[2] = 0; pNormal = pNormal.unit() * sin(theta);
		pNormal[2] = cos(theta);
		return pNormal;
	}
}

double CDRoundCone::CurvatureRadius(Vec3d p){
	/// 下記の計算は現時点でバグもち。
	/// RoundConeの両端の半径が同じであるときにINDになる
	/// いずれ修正すること！ ('09/02/17, mitake)

	Vec2d	r = radius;
	double	l = length;

	// RoundConeの側面の角度（側面がZ軸に垂直なとき0°、平行(つまりカプセル型)のとき90°）
	double theta = acos((r[1]-r[0])/l);
	// 接触点のZ座標
	double Zc = p[2];

	/*
	接触点がどちらかの球体にある場合：
	*/
	if (Zc > (r[0]*cos(theta) + l/2.0)) {
		return r[0];
	} else if (Zc < (r[1]*cos(theta) - l/2.0)) {
		return r[1];
	}

	/*
	接触点が球体と球体の間にある場合：
	接触点を通り、接触点の位置で側面と直交する断面におけるRoundConeの切断を考える。
	その断面は楕円となり、その楕円の最小曲率半径（長軸の端における曲率半径）が求めるものとなる。
	*/

	/// -- RoundConeを延長した円錐の頂点のZ座標
	double Z0 = l/2*(r[1]+r[0])/(r[1]-r[0]);

	/// -- Z-R座標系で見たときの側面の傾き
	double M1 = tan(Rad(90) - theta);
	/// -- Z-R座標系で見たときの断面の傾き
	double M2 = tan(theta);

	/// -- 切断面の向こう側の点
	double Za = (M1*Z0-M2*Zc)/(M1-M2);
	Vec2d pA = Vec2d(Za, -M1*Za + M1*Z0);
	/// -- 切断面のこっち側の点
	double Zb = (M1*Z0+M2*Zc)/(M1+M2);
	Vec2d pB = Vec2d(Zb,  M1*Zb - M1*Z0);
	/// -- RoundConeを延長した円錐の頂点
	Vec2d p0 = Vec2d(Z0, 0);

	/// -- 切断楕円の大きさを考えるための三角形の三辺
	double Ta = (pB - p0).norm(), Tb = (pA - p0).norm(), Tc = (pA - pB).norm();
	/// -- 切断楕円の長軸半径
	double eL = Tc/2;
	/// -- 切断楕円の中心から焦点までの距離（長軸半径からTa,Tb,Tcからなる三角形の内接円半径を引いたものになる）
	double f = eL - (Ta*Tc)/(Ta+Tb+Tc);
	/// -- 切断楕円の短軸半径
	double eS = sqrt(eL*eL - f*f);
	/// -- 切断楕円の最小曲率半径
	double Rmin = eS*eS / eL;

	return Rmin;
}

Vec2f CDRoundCone::GetRadius() {
	return radius;
}
float CDRoundCone::GetLength() {
	return length;
}
void CDRoundCone::SetRadius(Vec2f r) {
	radius=r;
}
void CDRoundCone::SetLength(float l) {
	length=l;
}
void CDRoundCone::SetWidth(Vec2f r) {
	Vec2f radiusMargin=r-radius;
	
	length-=(radiusMargin.x+radiusMargin.y);
	radius=r;
}

}	//	namespace Spr
