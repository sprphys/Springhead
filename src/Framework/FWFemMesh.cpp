#include "Framework.h"
#include <Framework/SprFWFemMesh.h>
#include <Graphics/GRFrame.h>
#include <Graphics/GRMesh.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHFemMeshThermo.h>
#include <Foundation/UTLoadHandler.h>
#include "FWFemMesh.h"
#include "FWSprTetgen.h"



#include <Collision/CDQuickHull2DImp.h>


#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

#ifdef _DEBUG
# define PDEBUG(x)	x
#else
# define PDEBUG(x)
#endif

namespace Spr{;

FWFemMesh::FWFemMesh(const FWFemMeshDesc& d):grMesh(NULL){
	SetDesc(&d);
}
void FWFemMesh::Sync(bool ph2gr){	
	//	テスト用
	static double value, delta;
	if (value <= 0) delta = 0.01;
	if (value >= 1) delta = -0.01;
	value += delta;

	//negitest
	//焦げテクスチャの枚数
	unsigned kogetex =5;
	//サーモテクスチャの枚数
	unsigned thtex = 5;
	//水分テクスチャの枚数
	unsigned watex = 2;
	
	double dtex =(double) 1.0 / ( kogetex + thtex + watex);		//	テクスチャ奥行座標の層間隔
	double texstart = dtex /2.0;								//	テクスチャ座標の初期値 = 焦げテクスチャのスタート座標
	double thstart = texstart + kogetex * dtex ;				//	サーモのテクスチャのスタート座標
	double wastart = texstart + kogetex * dtex + thtex * dtex;	//	水分量表示テクスチャのスタート座標

	//	50度刻み:テクスチャの深さ計算(0~50)	( vertices.temp - 50.0 ) * dtex / 50.0
	//	50度刻み:テクスチャの深さ計算(50~100)	( vertices.temp - 100.0 ) * dtex / 50.0
	//	50度刻み:テクスチャの深さ計算(100~150)	( vertices.temp - 150.0 ) * dtex / 50.0
	//	これを満たすように、50,100,150度などを変数にしてもよい。が、他に流用しないし、一目でわかりやすいので、このままでいいかも。
	//	50度刻みごとにdtexを加算せずに、gvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex / 50.0 + thstart;だけでやるのも手

	//	同期処理
	FWObject::Sync(ph2gr);
	if (ph2gr && grMesh->IsTex3D()){
		float* gvtx = grMesh->GetVertexBuffer();
		if (gvtx){
			int tex = grMesh->GetTexOffset();
			int stride = grMesh->GetStride();
			for(unsigned gv=0; gv<vertexIdMap.size(); ++gv){
				int pv = vertexIdMap[gv];
				//	PHから何らかの物理量を取ってくる
						//phから節点の温度を取ってくる
				//PHFemMeshThermoの各節点の温度を取ってくる。
				//温度の値に応じて、↑の係数を用いて、テクスチャ座標を計算する
				//	value = phMeshの派生クラス->thermo[pv];
				//	GRのテクスチャ座標として設定する。	s t r q の rを設定
				//gvtx[stride*gv + tex + 2] = value + gvtx[stride*gv];	//	gvtx[stride*gv]で場所によって違う深度を拾ってくることに
				//gvtx[stride*gv + tex + 2] = 0.1 + value;
				////gvtx[stride*gv + tex + 2] = thstart;
//				gvtx[stride*gv + tex + 2] = thstart;

				//	どのテクスチャにするかの条件分岐を作る
				//	直前のテクスチャ座標を保存しておく。なければ、初期値を設定
				//	テクスチャの表示モードを切り替えるSWをキーボードから切り替え⇒SampleApp.hのAddHotkey、AddAction周りをいじる
				
				//	CADThermoの該当部分のソース
				//if(tvtxs[j].temp <= tvtxs[j].temp5){		//tvtxs[j].wmass > wmass * ratio1
				//	texz	= texbegin;
				//	double texznew =diff * grad + texz;//実質,テクスチャ座標の初期値
				//	////前のテクスチャｚ座標よりも今回の計算値が深かったら、この計算結果を反映させる
				//	if(tvtxs[j].tex1memo <= texznew){			//初めはこの条件がなくてもいいけれど、一度温度が上がって、冷めてからは必要になる
				//		tvtxs[j].SetTexZ(tratio * dl + texz);	//テクスチャのZ座標を決定する。//表示テクスチャはその線形和を表示させるので、Z座標も線形和で表示するので、線形和の計算を使用
				//		tvtxs[j].tex1memo = tratio * dl + texz;	//tex1memoを更新する
				//	}
				//}

				double temp = phMesh->vertices[pv].temp;
				//	一個前のテクスチャ？⇒緑
				if( temp < 0){				//	暫定的にひとつ前のテクスチャにする?何か、色を割り当てる。一目でわかる色に。
					gvtx[stride*gv + tex + 2] = thstart - dtex /10.0;			//thstart直前は真っ黒焦げテクスチャなので、その適当値。サーモが非テクスチャ化されたら、別の色に。
				}
				//	緑⇒黄色
				else if(temp <= 50.0 ){		//temp <0の時、どうしようか？	0.0 < temp	の条件を無くした
					double yellow = temp * dtex / 50.0 + thstart;
					gvtx[stride*gv + tex + 2] = temp * dtex / 50.0 + thstart;
				}
				//	黄色⇒オレンジ
				else if(50.0 < temp && temp <= 100.0){
					gvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex / 50.0 + thstart;
				}
				//	オレンジ⇒赤
				else if(100.0 < temp && temp <= 150.0){
					gvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex / 50.0 + thstart;
				}
				//	赤⇒ピンク
				else if(150.0 < temp && temp <= 200.0){
					double pinkc = (temp - 50.0 ) * dtex / 50.0 + thstart ;
					gvtx[stride*gv + tex + 2] = (temp - 50.0 ) * dtex / 50.0 + thstart ;
				}
				//	ピンク	:色固定
				else if(200.0 < temp){
					gvtx[stride*gv + tex + 2] = dtex * 4.0 + thstart;
				}
			}
		}	
	}else{
		assert(0);	//	not supported.
	}
}


size_t FWFemMesh::NChildObject() const{
	return FWObject::NChildObject() + (grMesh ? 1 : 0);
}
ObjectIf* FWFemMesh::GetChildObject(size_t pos){
	if (pos < FWObject::NChildObject()){
		return FWObject::GetChildObject(pos);
	}
	if (grMesh && pos < NChildObject()) return grMesh->Cast();
	return NULL;
}
bool FWFemMesh::AddChildObject(ObjectIf* o){
	GRMesh* mesh = o->Cast();
	if (mesh){
		grMesh = mesh;
		return true;
	}else{
		return FWObject::AddChildObject(o);
	}
}
void FWFemMesh::Loaded(UTLoadContext*){
	if (!phMesh) CreatePHFromGR();
	if (grFrame){
		grFrame->DelChildObject(grMesh->Cast());
		CreateGRFromPH();
		grFrame->AddChildObject(grMesh->Cast());
	}
}
bool FWFemMesh::CreatePHFromGR(){
	//	呼び出された時点で grMesh にグラフィクスのメッシュが入っている
	//	grMeshを変換して、phMeshをつくる。

	//	以下で、grMeshからtetgenを呼び出して変換して、pmdに値を入れていけば良い。
	PHFemMeshThermoDesc pmd;
	
	//TetGenで四面体メッシュ化
	Vec3d* vtxsOut=NULL;
	int* tetsOut=NULL;
	int nVtxsOut=0, nTetsOut=0;
	std::vector<Vec3d> vtxsIn;
	for(unsigned i=0; i<grMesh->vertices.size(); ++i) vtxsIn.push_back(grMesh->vertices[i]);
	sprTetgen(nVtxsOut, vtxsOut, nTetsOut, tetsOut, grMesh->vertices.size(), &vtxsIn[0], grMesh->faces.size(), &grMesh->faces[0], "pq1.2a0.3");//a0.003
	
	//	PHMesh用のディスクリプタpmdに値を入れていく
	for(int i=0; i < nVtxsOut; i++){
		pmd.vertices.push_back(vtxsOut[i]);
	} 
	pmd.tets.assign(tetsOut, tetsOut + nTetsOut*4);
	//	PHMeshの生成
	phMesh = DBG_NEW PHFemMeshThermo(pmd);
	if (GetPHSolid() && GetPHSolid()->GetScene())
		GetPHSolid()->GetScene()->AddChildObject(phMesh->Cast());
	return true;
}
void FWFemMesh::CreateGRFromPH(){
	//	頂点の対応表を用意
	std::vector<int> vtxMap;
	vtxMap.resize(phMesh->vertices.size(), -1);
	for(unsigned i=0; i<phMesh->surfaceVertices.size(); ++i){
		vtxMap[phMesh->surfaceVertices[i]] = i;
	}
	GRMeshDesc gmd;	//	新しいGRMeshのデスクリプタ
	//	表面の頂点を設定
	for(unsigned i=0; i< phMesh->surfaceVertices.size(); ++i) 
		gmd.vertices.push_back(phMesh->vertices[ phMesh->surfaceVertices[i] ].pos);
	//	表面の三角形を設定
	for(unsigned i=0; i< phMesh->nSurfaceFace; ++i) {
		GRMeshFace f;
		f.nVertices = 3;
		for(int j=0; j<3; ++j){
			f.indices[j] = vtxMap[phMesh->faces[i].vertices[j]];
		}
		gmd.faces.push_back(f);
	}
	//	phMeshの三角形とgrMeshの三角形の対応表をつくる	重なっている面積が最大のものが対応する面
	//	まず、法線が近いものを探し、面1と面2上の頂点の距離が近いものに限り、重なっている面積を求める。
	std::vector<Vec3f> pnormals(gmd.faces.size());
	for(unsigned pf=0; pf<gmd.faces.size(); ++pf){
		assert(gmd.faces[pf].nVertices == 3);
		pnormals[pf] = ((gmd.vertices[gmd.faces[pf].indices[2]] - gmd.vertices[gmd.faces[pf].indices[0]]) %
		(gmd.vertices[gmd.faces[pf].indices[1]] - gmd.vertices[gmd.faces[pf].indices[0]])).unit();
	}
	std::vector<Vec3f> gnormals(grMesh->faces.size());
	struct FaceWall{
		Vec3f wall[4];
	};
	std::vector<FaceWall> gWalls(gnormals.size());
	for(unsigned gf=0; gf<gnormals.size(); ++gf){
		gnormals[gf] = ((grMesh->vertices[grMesh->faces[gf].indices[2]] - grMesh->vertices[grMesh->faces[gf].indices[0]]) %
			(grMesh->vertices[grMesh->faces[gf].indices[1]] - grMesh->vertices[grMesh->faces[gf].indices[0]])).unit();
		int nv = grMesh->faces[gf].nVertices;
		for(int i=0; i<nv; ++i){
			gWalls[gf].wall[i] = ((grMesh->vertices[grMesh->faces[gf].indices[(i+1)%nv]] - grMesh->vertices[grMesh->faces[gf].indices[i]]) % gnormals[gf]).unit();
		}
	}
	std::vector<int> pFaceMap(pnormals.size());
	for(unsigned pf=0; pf<pnormals.size(); ++pf){
		std::vector<int> cand;
		for(unsigned gf=0; gf<gnormals.size(); ++gf){
			if (pnormals[pf] * gnormals[gf] > 0.8){	//	法線が遠いのはだめ
				int i;
				for(i=0; i<3; ++i){
					double d = gnormals[gf] * (gmd.vertices[gmd.faces[pf].indices[i]] - grMesh->vertices[grMesh->faces[gf].indices[0]]);
					if (d*d > 0.01) break;	//	距離が離れすぎているのはだめ
				}	
				if (i==3){
					//	phの三角形の重心が、grの中の面に入っている物をさがす
					Vec3f center;
					for(int v=0;v<3; ++v) center += gmd.vertices[gmd.faces[pf].indices[v]];
					center /= 3;
					int v;
					for(v=0; v<grMesh->faces[gf].nVertices; ++v){
						double d = gWalls[gf].wall[v] * (center - grMesh->vertices[grMesh->faces[gf].indices[v]]);
						if (d<0) break;
					}
					if (v == grMesh->faces[gf].nVertices){
						pFaceMap[pf] = gf;
						//	DEBUG出力
						//	DSTR << "center:" << center <<std::endl;
						//	for(int v=0; v<grMesh->faces[gf].nVertices; ++v)
						//		DSTR << "vtx" << grMesh->vertices[grMesh->faces[gf].indices[v]] << "wall:" << gWalls[gf].wall[v] << std::endl;
						break;
					}
				}
			}
		}
	}
	PDEBUG(	DSTR << "FaceMap PHtoGR:"; )
	PDEBUG(	for(unsigned i=0; i<pFaceMap.size(); ++i) DSTR << pFaceMap[i] << " "; )
	PDEBUG(	DSTR << std::endl; )
	//	対応表に応じてマテリアルリストを設定。
	gmd.materialList.resize(grMesh->materialList.size() ? pFaceMap.size() : 0);
	for(unsigned pf=0; pf<gmd.materialList.size(); ++pf){
		gmd.materialList[pf] = grMesh->materialList[pFaceMap[pf]];
	}
	//	新しく作るGRMeshの頂点からphMeshの頂点への対応表
	vertexIdMap.resize(gmd.vertices.size(), -1);
	//	対応表に応じて、頂点のテクスチャ座標を作成
	//		phの１点がgrの頂点複数に対応する場合がある。
	//		その場合は頂点のコピーを作る必要がある。
	std::vector<bool> vtxUsed(gmd.vertices.size(), false);
	for(unsigned pf=0; pf<pFaceMap.size(); ++pf){		
		for(unsigned i=0; i<3; ++i){
			int pv = gmd.faces[pf].indices[i];
			//	テクスチャ座標を計算
			Vec2f texCoord;
			Vec3f normal;
			GRMeshFace& gFace = grMesh->faces[pFaceMap[pf]];
			GRMeshFace& gFaceNormal = grMesh->faceNormals[pFaceMap[pf]];
			GRMeshFace* gNormal = NULL;
			if (grMesh->normals.size()){
				gNormal = &gFace;
				if (grMesh->faceNormals.size()) gNormal = &grMesh->faceNormals[pFaceMap[pf]];
			}
			if (gFace.nVertices == 3){
				Vec3f weight;
				Matrix3f vtxs;
				for(unsigned j=0; j<3; ++j){
					vtxs.col(j) = grMesh->vertices[gFace.indices[j]];
				}
				int tmp[3];
				vtxs.gauss(weight, gmd.vertices[pv], tmp);
				for(unsigned j=0; j<3; ++j){
					assert(weight[j] > -0.001);
					texCoord += weight[j] * grMesh->texCoords[gFace.indices[j]];
					if(gNormal) normal += weight[j] * grMesh->normals[gNormal->indices[j]];
				}
			}else{	//	4頂点
				//	どの３頂点で近似すると一番良いかを調べ、その３頂点を補間
				Vec3f weight[4];
				Matrix3f vtxs[4];
				double wMin[4];
				double wMinMax = -DBL_MAX;
				int maxId=-1;
				for(int j=0; j<4; ++j){
					for(int k=0; k<3; ++k){
						vtxs[j].col(k) = grMesh->vertices[gFace.indices[k<j ? k : k+1]];
					}
					int tmp[3];
					vtxs[j].gauss(weight[j], gmd.vertices[pv], tmp);
					wMin[j] = DBL_MAX;
					for(int l=0; l<3; ++l) if (wMin[j] > weight[j][l]) wMin[j] = weight[j][l];
					if (wMin[j] > wMinMax){
						wMinMax = wMin[j];
						maxId = j;
					}
				}
				for(int j=0; j<3; ++j){
					texCoord += weight[maxId][j] * grMesh->texCoords[gFace.indices[j<maxId?j:j+1]];
					if(gNormal){
						normal += weight[maxId][j] * grMesh->normals[gFaceNormal.indices[j<maxId?j:j+1]];
					}
				}
			}
			gmd.texCoords.resize(gmd.vertices.size());
			if (grMesh->normals.size()) gmd.normals.resize(gmd.vertices.size());
			//	重複頂点の場合はコピーを作りながら代入
			if (vtxUsed[pv]){
				if (gmd.texCoords[pv] != texCoord || 
					(grMesh->normals.size() && gmd.normals[pv] != normal)){	
					//	頂点のコピーの作成
					gmd.vertices.push_back(gmd.vertices[pv]);
					gmd.texCoords.push_back(texCoord);
					if (gmd.normals.size()) gmd.normals.push_back(normal);
					gmd.faces[pf].indices[i] = gmd.vertices.size()-1;
					vertexIdMap.push_back(phMesh->surfaceVertices[pv]);
				}
			}else{	//	そうでなければ、直接代入
				gmd.texCoords[pv] = texCoord;
				if (gmd.normals.size()) gmd.normals[pv] = normal; 
				vertexIdMap[pv] = phMesh->surfaceVertices[pv];
				vtxUsed[pv] = true;
			}
		}
	}
	//	GRMeshを作成
	GRMesh* rv = grMesh->GetNameManager()->CreateObject(GRMeshIf::GetIfInfoStatic(), &gmd)->Cast();
	//	マテリアルの追加
	for (unsigned i=0; i<grMesh->NChildObject(); ++i){
		rv->AddChildObject(grMesh->GetChildObject(i));
	}
	//	テクスチャモードをコピー
	rv->tex3d = grMesh->tex3d;
	grMesh = rv;
}
}
