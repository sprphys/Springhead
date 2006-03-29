#ifndef CDCONVEXMESH_H
#define CDCONVEXMESH_H

#include <SprCollision.h>
#include <Foundation/Object.h>
#include "CDConvex.h"

namespace Spr{;

class CDVertexIDs:public std::vector<int>{
public:
	int FindPos(int id) const;
};

/**	凸多面体の面を表すクラス．CDConvexMesh が所有．
	CDContactAnalysis で使われる．
*/
class CDFace: public CDFaceIf{
public:
	int vtxs[3];	///< 面の頂点ID

	///
	virtual int NIndex(){ return 3; }
	///
	virtual int* GetIndices(){ return vtxs; }
};

class CDFaces:public std::vector<CDFace>{
};

///	凸多面体
class CDConvexMesh : public InheritNamedObject<CDConvexMeshIf, CDConvex>{
public:
	OBJECTDEF(CDConvexMesh);

	///	探索開始頂点番号
	mutable int curPos;
	///	頂点の座標(ローカル座標系)
	std::vector<Vec3f> base;

	///	頂点の隣の頂点の位置が入った配列
	std::vector< std::vector<int> > neighbor;
	///	面(3角形 0..nPlanes-1 が独立な面，それ以降はMargeFaceで削除される同一平面上の面)
	CDFaces faces;
	///	面(3角形のうち，MergeFace()で残った数)
	int nPlanes;

public:
	CDConvexMesh();
	CDConvexMesh(const CDConvexMeshDesc& desc);

	///	ShapeType
	virtual int ShapeType(){ return CDShapeDesc::CONVEXMESH; }

	///	頂点から面や接続情報を生成する．
	void CalcFace();
	///	同一平面上で接続されている3角形をマージする
	void MergeFace();
	
	///	サポートポイントを求める．
	virtual Vec3f Support(const Vec3f& p) const;
	
	///	切り口を求める．接触解析に使う．
	virtual void FindCutRing(CDCutRing& r, const Posed& toW);

	CDFaceIf* GetFace(size_t i);
	size_t NFace();
	Vec3f* GetVertices();
	size_t NVertex();
};

}	//	namespace Spr
#endif
