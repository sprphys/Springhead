﻿/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Collision/CDRoundCone.h>

namespace Spr{;

//----------------------------------------------------------------------------
//	CDRoundCone
CDRoundCone::CDRoundCone() {
}

CDRoundCone::CDRoundCone(const CDRoundConeDesc& desc){
	radius = desc.radius;
	length = desc.length;
	material = desc.material;
	CalcMetrics();
}

bool CDRoundCone::IsInside(const Vec3f& p){
	float halfl = 0.5f * length;
	float px2 = p.x*p.x;
	float py2 = p.y*p.y;
	float pz[2];
	float pz2[2];
	pz[0] = (p.z + halfl);
	pz[1] = (p.z - halfl);
	pz2[0] = pz[0]*pz[0];
	pz2[1] = pz[1]*pz[1];

	if(px2 + py2 + pz2[0] < radius[0]*radius[0])
		return true;
	if(px2 + py2 + pz2[1] < radius[1]*radius[1])
		return true;
	if(-halfl < p.z && p.z < halfl){
		float r = (radius[1] - radius[0])/length * p.z + (radius[1] + radius[0])/2.0f;
		if(px2 + py2 < r*r)
			return true;
	}
	return false;
}

float CDRoundCone::CalcVolume(){
	float sinTheta = (radius[1] - radius[0]) / length;
	float cosTheta = sqrt(1 - pow((radius[1] - radius[0]) / length, 2.0f));
	float zMinus = -(length / 2.0f) - radius[0];
	float zZero = -(length / 2.0f) - radius[0] * sinTheta;
	float zOne = length / 2.0f - radius[1] * sinTheta;
	float zPlus = length / 2.0f + radius[1];

	float volumeR0 = M_PI * radius[0]*radius[0] * (zZero - zMinus) -
		(M_PI / 3.0f)*(pow((zZero + length / 2.0f), 3.0f) - pow(zMinus + length/2.0f, 3.0f));
	float volumeR1 = M_PI * radius[1] * radius[1] * (zPlus - zOne) +
		(M_PI / 3.0f)*(pow(length / 2.0f - zPlus, 3.0f) - pow(length / 2.0f - zOne, 3.0f));
	//float volumeCone = M_PI * pow((radius[0] + radius[1]) * cosTheta,2.0f)/4.0f*(length - radius[0]*sinTheta + radius[1] * sinTheta);
	float volumeCone = (1.0f / 3.0f)*length * M_PI *(pow(radius[0],2.0f) + radius[0]*radius[1] + pow(radius[1],2.0f));
	//DSTR << " length " << length << " r0 " << radius[0] << " r1 " <<radius[1] << 
	//	"sinTheta " << sinTheta << " cosTheta " << cosTheta <<
	//	" zMinus " << zMinus << " zZero " << zZero << " zOne " << zOne <<
	//	" zPlus " << zPlus << std::endl << " volumeR0 " << volumeR0 << " volumeR1 " << volumeR1 << " volumeCone " << volumeCone <<std::endl;
	
	return volumeR0 + volumeR1 + volumeCone;
	//return  CDShape::CalcHemisphereVolume(radius[0]) +
	//		CDShape::CalcHemisphereVolume(radius[1]) +
	//		(1.0f/3.0f) * (float)M_PI * (radius[0]*radius[0] + radius[0]*radius[1] + radius[1]*radius[1]) * length;
}

Vec3f CDRoundCone::CalcCenterOfMass(){
	float sinTheta = (radius[1] - radius[0]) / length;
	float cosTheta = sqrt(1 - pow((radius[1] - radius[0]) / length, 2.0f));
	float tanTheta = sinTheta / cosTheta;
	float zMinus = -(length / 2.0f) - radius[0];
	float zZero = -(length / 2.0f) - radius[0] * sinTheta;
	float zOne = length / 2.0f - radius[1] * sinTheta;
	float zPlus = length / 2.0f + radius[1];
	float density = GetDensity();

	// 球の中にすべて含まれる場合
	if((radius[0] >= radius[1]) && radius[0] >= length + radius[1]){
		return Vec3f(0, 0, -length / 2);
	}else if ((radius[1] >= radius[0]) && radius[1] >= length + radius[0]) {
		return Vec3f(0, 0, length / 2);
	}else {
		float r0 = density * M_PI *(pow(radius[0], 2.0f) * (pow(zZero, 2.0f) - pow(zMinus, 2.0f)) / 2.0f -
			(zZero*pow(length / 2.0f + zZero, 3.0f) -
				zMinus * pow(length / 2.0f + zMinus, 3.0f)) / 3.0f +
				(pow(length / 2 + zZero, 4.0f) - pow(length / 2 + zMinus, 4.0f)) / 12.0f);
		float r1 = density * M_PI *(pow(radius[1], 2.0f) * (pow(zPlus, 2.0f) - pow(zOne, 2.0f)) / 2.0f +
			(zPlus*pow(length / 2.0f - zPlus, 3.0f) -
				zOne * pow(length / 2.0f - zOne, 3.0f)) / 3.0f +
				(pow(length / 2 - zPlus, 4.0f) - pow(length / 2 - zOne, 4.0f)) / 12.0f);

		float tempVar = radius[0] * cosTheta + length * tanTheta / 2.0f + radius[0] * sinTheta*tanTheta;
		float cone = 0;
		// tanThetaが0の時0割になってしまうのでその場合は0にする
		if (abs(tanTheta) >= 1.0e-05) {
			cone = density * M_PI *((zOne*pow(zOne*tanTheta + tempVar, 3.0f) -
				zZero * pow(zZero*tanTheta + tempVar, 3.0f)) / (3.0f * tanTheta) -
				((pow(zOne*tanTheta + tempVar, 4.0f) -
					pow(zZero*tanTheta + tempVar, 4.0f)) / (12.0f * pow(tanTheta, 2.0f))));
		}
		//float cone = density * M_PI * ((pow(zOne, 2.0f) - pow(zZero, 2.0f))*
		//	(radius[0] * cosTheta + length * tanTheta / 2.0f + radius[0] * sinTheta*tanTheta) / 2.0f +
		//	(pow(zOne, 3.0f) - pow(zZero, 3.0f))*tanTheta / 3.0f);

		//DSTR << " length " << length << " r0 " << radius[0] << " r1 " << radius[1] <<
		//	"sinTheta " << sinTheta << " cosTheta " << cosTheta << " tanTheta " << tanTheta <<
		//	" zMinus " << zMinus << " zZero " << zZero << " zOne " << zOne <<
		//	" zPlus " << zPlus << std::endl << " r0 " << r0 << " r1 " << r1 <<
		//	" cone " << cone << " mu " << density << " volume " << CalcVolume() <<
		//	" M " << CalcVolume()*density << " tasu " << r0 + r1 + cone <<
		//	" ans " << (r0 + r1 + cone) / (CalcVolume()*density) << std::endl;
		//DSTR << pow(radius[0], 2.0f) * (pow(zZero, 2.0f) - pow(zMinus, 2.0f)) / 2.0f << " " <<
		//	pow(radius[1], 2.0f) * (pow(zPlus, 2.0f) - pow(zOne, 2.0f)) / 2.0f << std::endl;
		//DSTR << (zZero*pow(length / 2.0f + zZero, 3.0f) -
		//	zMinus * pow(length / 2.0f + zMinus, 3.0f)) / 3.0f << " " <<
		//	(zPlus*pow(length / 2.0f - zPlus, 3.0f) -
		//		zOne * pow(length / 2.0f - zOne, 3.0f)) / 3.0f << std::endl;
		return Vec3f(0, 0, (r0 + r1 + cone) / (CalcVolume()*density));
	}
}

Matrix3f CDRoundCone::CalcMomentOfInertia(){
	float sinTheta = (radius[1] - radius[0]) / length;
	float cosTheta = sqrt(1 - pow((radius[1] - radius[0]) / length, 2.0f));
	float tanTheta = sinTheta / cosTheta;
	float zMinus = -(length / 2.0f) - radius[0];
	float zZero = -(length / 2.0f) - radius[0] * sinTheta;
	float zOne = length / 2.0f - radius[1] * sinTheta;
	float zPlus = length / 2.0f + radius[1];

	
	// 球の中にすべて含まれる場合
	if((radius[0] >= radius[1]) && radius[0] >= length + radius[1]){
		return (2.0f/5.0f)*(4.0f/3.0f)*M_PI*pow(radius[0],3.0f)*pow(radius[0],2.0f)* Matrix3f::Unit();
	}else if ((radius[1] >= radius[0]) && radius[1] >= length + radius[0]) {
		return (2.0f/5.0f)*(4.0f/3.0f)*M_PI*pow(radius[1],3.0f)*pow(radius[1],2.0f)* Matrix3f::Unit();
	}
	else {
		float r0 = (M_PI / 2.0f)*(pow(radius[0], 4.0f)*(zZero - zMinus) -
			(2.0f / 3.0f)*pow(radius[0], 2.0f)*(pow(zZero + length / 2, 3.0f) - (pow(zMinus + length / 2, 3.0f))) +
			(1.0f / 5.0f)*(pow(zZero + length / 2.0f, 5.0f) - pow(zMinus + length / 2.0f, 5.0f)));
		float r1 = (M_PI / 2.0f)*(pow(radius[0], 4.0f)*(zPlus - zOne) +
			(2.0f / 3.0f)*pow(radius[0], 2.0f)*(pow(length / 2 - zPlus, 3.0f) - (pow(length / 2 - zOne, 3.0f))) -
			(1.0f / 5.0f)*(pow(length / 2.0f - zPlus, 5.0f) - pow(length / 2.0f - zOne, 5.0f)));
		float tempVar = radius[0] * cosTheta + length * tanTheta / 2.0f + radius[0] * sinTheta*tanTheta;
		float cone = 0;
		if (abs(tanTheta) >= 1.0e-05) {
			cone = (M_PI / (10.0f*tanTheta))*(pow(zOne*tanTheta + tempVar, 5.0f) -
				pow(zZero*tanTheta + tempVar, 5.0f));
		}
		else
		{
			cone = CDShape::CalcCylinderInertia(radius[0], length).zz;
		}

		//DSTR << "RoundCone "<<cone << " r0 "<<r0 << " r1 " << r1  <<" Iz Inertia "<<r0 + r1 + cone << std::endl;
		float Iz = r0 + r1 + cone;
		float g = CalcCenterOfMass().z;
		float r0Ix = M_PI * (1.0f / 60.0f)* (12 * pow(zMinus, 5) + 15 * pow(zMinus, 4)* (length - 2 * g) + 5 * pow(zMinus, 3)*
			(4 * Square(g) - 8 * g* length + Square(length) - 4 * Square(radius[0])) + 15 * Square(zMinus)* g *(2 * g* length - Square(length) +
				4 * pow(radius[0], 2)) + 15 * zMinus *pow(g, 2) *(pow(length, 2) - 4 * pow(radius[0], 2)) -
			zZero * (12 * pow(zZero, 4) + 15 * pow(zZero, 3) *(length - 2 * g) + 5 * pow(zZero, 2) *(4 * pow(g, 2) -
				8 * g* length + pow(length, 2) - 4 * pow(radius[0], 2)) + 15 * zZero* g *(2 * g *length - pow(length, 2) +
					4 * pow(radius[0], 2)) + 15 * pow(g, 2)* (pow(length, 2) - 4 * pow(radius[0], 2))));

		float r1Ix = M_PI * (1.0f / 60.0f)* (12 * pow(zOne, 5) - 15 * pow(zOne, 4) *(2 * g + length) + 5 * pow(zOne, 3) *
			(4 * pow(g, 2) + 8 * g* length + pow(length, 2) - 4 * pow(radius[1], 2)) - 15 * pow(zOne, 2) *g* (2 * g *length + pow(length, 2) -
				4 * pow(radius[1], 2)) + 15 * zOne *pow(g, 2) *(pow(length, 2) - 4 * pow(radius[1], 2)) +
			zPlus * ((-12) *pow(zPlus, 4) + 15 * pow(zPlus, 3) *(2 * g + length) - 5 * pow(zPlus, 2) *(4 * pow(g, 2) +
				8 * g* length + pow(length, 2) - 4 * pow(radius[1], 2)) + 15 * zPlus* g* (2 * g* length + pow(length, 2) -
					4 * pow(radius[1], 2)) - 15 * pow(g, 2) *(pow(length, 2) - 4 * pow(radius[1], 2))));
		float coneIx = 0;
		if (abs(tanTheta) >= 1.0e-05) {
			coneIx = M_PI * (1.0f / 30.0f)* (-6.0f* pow(zZero, 5)* Square(tanTheta) + 15 * pow(zZero, 4)* tanTheta* (g* tanTheta - tempVar) - 10.0f *pow(zZero, 3.0f)* (Square(tempVar) - 4.0f* tempVar* g* tanTheta + Square(g)*Square(tanTheta)) + 30.0f* Square(zZero)* tempVar *g *(tempVar - g * tanTheta) -
				30 * zZero* Square(tempVar)* Square(g) + zOne * (6.0f * pow(zOne, 4)*Square(tanTheta) + 15 * pow(zOne, 3) *tanTheta* (tempVar - g * tanTheta) + 10 * Square(zOne)* (Square(tempVar) - 4 * tempVar* g *tanTheta + Square(g)*Square(tanTheta)) - 30 * zOne* tempVar* g* (tempVar - g * tanTheta) + 30 * Square(tempVar) *Square(g)));
		}
		else
		{
			coneIx = CDShape::CalcCylinderInertia(radius[0], length).xx;
		}
		//DSTR << "RoundCone Ix r0"<<r0Ix<<" r1 " <<r1Ix << " cone " << coneIx << std::endl;
		float Ix = (1.0f / 2.0f)*Iz + r0Ix + r1Ix + coneIx;
		Matrix3d I;
		I.zz = Iz;
		I.xx = Ix;
		I.yy = Ix;
		return I;
	}
}
	
// サポートポイントを求める
int CDRoundCone::Support(Vec3f&w, const Vec3f& v) const{
	float normal_Z = (radius[0] - radius[1]) / length;
	float n = v.norm();
	Vec3f dir;
	if(n < 1.0e-10f){
		dir = Vec3f();
	}else{
		dir = v / n;
	}

	if (-1 < normal_Z && normal_Z < 1) {
		if (normal_Z < dir.Z()) {
			// vの向きがZ軸前方 → radius[1]を使用
			w = dir*radius[1] + Vec3f(0,0, length/2.0);
			return 1;
		} else {
			// vの向きがZ軸後方 → radius[0]を使用
			w = dir*radius[0] + Vec3f(0,0,-length/2.0);
			return 0;
		}
	} else {
		assert(0);
		// どちらかの球に包含されている
		if (radius[0] < radius[1]) {
			w = dir*radius[1] + Vec3f(0,0, length/2.0);
		} else {
			w =  dir*radius[0] + Vec3f(0,0,-length/2.0);
		}
		return -1;
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
	if (-1 > (-sinA) || (-sinA) > 1) { return false; } // 球体になっている
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
			if (end+length/20 < start){//0.001 < start){
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
	radius    = r;
	bboxReady = false;
	CalcMetrics();
}
void CDRoundCone::SetLength(float l) {
	length    = l;
	bboxReady = false;
	CalcMetrics();
}
void CDRoundCone::SetWidth(Vec2f r) {
	Vec2f radiusMargin=r-radius;
	
	length-=(radiusMargin.x+radiusMargin.y);
	radius=r;
	bboxReady = false;
	CalcMetrics();
}

int CDRoundCone::LineIntersect(const Vec3f& origin, const Vec3f& dir, Vec3f* result, float* offset){
	const float eps = 1.0e-10f;
	Vec3f p;
	int num = 0;
	Vec3f sCenter[2];	//球の中心位置
	sCenter[0] = Vec3f(0.0f,0.0f,length);
	sCenter[1] = Vec3f(0.0f,0.0f,-length);

	//球部分の判定
	for(int i=0; i<2; i++){
		const Vec3f n = sCenter[i] - origin;		 //面の法線 = カメラと球の原点を結ぶベクトル
		float tmp = n * dir;						 //面の法線とポインタのベクトルとの内積
		if(abs(tmp) < eps)							 //内積が小さい場合は判定しない
			continue;
		float s = ((sCenter[i] - origin) * n) / tmp; //カメラと面の距離 
		if(s < 0.0)
			continue;
		p = origin + dir * s;						 //直線と面の交点p = カメラ座標系の原点+カメラ座標系から面へのベクトル*距離 (Shape座標系)
		Vec3f po = p-sCenter[i];					 //球の中心を原点とした時の交点の位置
		// 円の内部にあるか
		if(po.norm()<GetRadius()[i]){
			result[num] = p;
			offset[num] = s;
			num++;
		}
	}
	//円柱部分の判定
	const Vec3f n =  origin;						 //カメラ方向への垂直な断面の法線 = カメラとshapeの原点を結ぶベクトル
	float tmp = n * dir;							 //面の法線とポインタのベクトルとの内積

	for(int i=0; i<1; i++){
		if(abs(tmp) < eps)							 //内積が小さい場合は判定しない
			continue;
		float s = ((- origin) * n) / tmp;			 //カメラと面の距離 
		if(s < 0.0)
			continue;
		p = origin + dir * s;						 //直線と面の交点p = カメラ座標系の原点+カメラ座標系から面へのベクトル*距離 (Shape座標系)
		
		Vec2f pr = Vec2f(p.x,p.y);					 //xy平面のp
		Vec2f r = GetRadius();
		if(r[0]<r[1]) r[0] = r[1]; 
		// 円柱の内部にあるか
		if(pr.norm()<r[0] && abs(p.z)<=GetLength()*0.5){
			result[num] = p;
			offset[num] = s;
			num++;
		}
		/*注意
		現在は円柱で判定を行っているが、本来は円錐の内部にあるか判定する必要がある。
		修正が必要
		*/
	}
	return num;
}

}	//	namespace Spr
