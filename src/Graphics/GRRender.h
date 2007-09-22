/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file GRRender.h
 *	@brief グラフィックスレンダラーの基本クラス　　
 */
#ifndef GRRENDER_H
#define GRRENDER_H

#include <SprGraphics.h>
#include <Foundation/Scene.h>
#include "GRFrame.h"
#include "IfStubGraphics.h"

namespace Spr{;

class GRCamera:public GRVisual, public GRCameraIfInit, public GRCameraDesc{
public:
	OBJECTDEF(GRCamera, GRVisual);
	ACCESS_DESC(GRCamera);
	UTRef<GRFrameIf> frame;
	GRCamera(const GRCameraDesc& desc=GRCameraDesc()):GRCameraDesc(desc), frame(NULL){}
	virtual size_t NChildObject() const ;
	virtual ObjectIf* GetChildObject(size_t pos);
	virtual GRFrameIf* GetFrame(){ return frame; }
	virtual void SetFrame(GRFrameIf* fr){ frame = fr; }
	virtual void SetDesc(const GRCameraDesc& desc);
	virtual bool AddChildObject(ObjectIf* o);
	virtual void Render(GRRenderIf* render);	
};

class GRLight :public GRVisual, public GRLightIfInit, public GRLightDesc{
public:
	OBJECTDEF(GRLight, GRVisual);
	ACCESS_DESC(GRLight);
	GRLight(const GRLightDesc& desc = GRLightDesc()):GRLightDesc(desc){}
	virtual void Render(GRRenderIf* render);
	virtual void Rendered(GRRenderIf* render);
};

/**	@brief	グラフィックスの材質 */
class GRMaterial :public GRVisual, public GRMaterialIfInit, public GRMaterialDesc{
public:
	OBJECTDEF(GRMaterial, GRVisual);
	ACCESS_DESC(GRMaterial);
	GRMaterial(const GRMaterialDesc& desc=GRMaterialDesc()):GRMaterialDesc(desc){}
	GRMaterial(Vec4f a, Vec4f d, Vec4f s, Vec4f e, float p){
		ambient = a;
		diffuse = d;
		specular = s;
		emissive = e;
		power = p;
	}
	GRMaterial(Vec4f c, float p){
		ambient = diffuse = specular = emissive = c;
		power = p;
	}
	GRMaterial(Vec4f c){
		ambient = diffuse = specular = emissive = c;		
	}
	/**	W()要素は、アルファ値(0.0〜1.0で透明度を表す). 1.0が不透明を表す.
		materialのW()要素を判定して、不透明物体か、透明物体かを判定する. 
		透明なオブジェクトを描くとき、遠くのものから順番に描画しないと、意図に反した結果となる. */
	bool IsOpaque() const {		
		return ambient.W() >= 1.0 && diffuse.W() >= 1.0 && specular.W() >= 1.0 && emissive.W() >= 1.0;
	}
	void Render(GRRenderIf* render);
	//virtual bool AddChildObject(ObjectIf* o);
};

/**	@class	GRRenderBase
    @brief	グラフィックスレンダラー/デバイスの基本クラス　 */
class GRRenderBase: public Object, public GRRenderBaseIfInit{
public:
	OBJECTDEF_ABST(GRRenderBase, Object);
	///	ビューポートの設定
	virtual void SetViewport(Vec2f pos, Vec2f sz){}
	///	バッファクリア
	virtual void ClearBuffer(){}
	///	レンダリングの開始前に呼ぶ関数
	virtual void BeginScene(){}
	///	レンダリングの終了後に呼ぶ関数
	virtual void EndScene(){}
	///	カレントの視点行列をafvで置き換える
	virtual void SetViewMatrix(const Affinef& afv){}
	///	カレントの投影行列をafpで置き換える
	virtual void SetProjectionMatrix(const Affinef& afp){}
	///	カレントの投影行列を取得する
	virtual void GetProjectionMatrix(const Affinef& afp){}
	///	カレントのモデル行列をafwで置き換える
	virtual void SetModelMatrix(const Affinef& afw){}
	///	カレントのモデル行列に対してafwを掛ける
	virtual void MultModelMatrix(const Affinef& afw){}
	///	カレントのモデル行列をモデル行列スタックへ保存する
	virtual void PushModelMatrix(){}
	///	モデル行列スタックから取り出し、カレントのモデル行列とする
	virtual void PopModelMatrix(){}
	/// ブレンド変換行列の全要素を削除する
	virtual void ClearBlendMatrix(){}
	/// ブレンド変換行列を設定する
	virtual bool SetBlendMatrix(const Affinef& afb, unsigned int id=0){return 0;}	
	///	頂点フォーマットの指定
	virtual void SetVertexFormat(const GRVertexElement* e){}
	///	頂点シェーダーの指定	API化候補．引数など要検討 2006.6.7 hase
	virtual void SetVertexShader(void* shader){}
	///	頂点を指定してプリミティブを描画
	virtual void DrawDirect(GRRenderBaseIf::TPrimitiveType ty, void* vtx, size_t count, size_t stride=0){}
	///	頂点とインデックスを指定してプリミティブを描画
	virtual void DrawIndexed(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, void* vtx, size_t count, size_t stride=0){}
	///	球体を描画
	virtual void DrawSphere(float radius, int slices, int stacks){}
 	///	頂点の成分ごとの配列を指定して，プリミティブを描画
	virtual void DrawArrays(GRRenderBaseIf::TPrimitiveType ty, GRVertexArray* arrays, size_t count){}
 	///	インデックスと頂点の成分ごとの配列を指定して，プリミティブを描画
	virtual void DrawArrays(GRRenderBaseIf::TPrimitiveType ty, size_t* idx, GRVertexArray* arrays, size_t count){}
	
	///	DiplayList の作成(リスト作成開始)
	virtual int StartList(){return 0;}
	///	リスト作成終了
	virtual void EndList(){}
	///	DisplayListの表示
	virtual void DrawList(int i){}
	///	DisplayListの解放
	virtual void ReleaseList(int i){}
	///	2次元テキストの描画　　 Windows環境(VC)でのみfontをサポートし、他の環境ではfontを指定しても利用されない。
	virtual void DrawFont(Vec2f pos, const std::string str, const GRFont& font=0){}
	///	3次元テキストの描画　　 Windows環境(VC)でのみfontをサポートし、他の環境ではfontを指定しても利用されない。	
	virtual void DrawFont(Vec3f pos, const std::string str, const GRFont& font=0){}
	///	描画の材質の設定
	virtual void SetMaterial(const GRMaterialDesc& mat){}
	virtual void SetMaterial(const GRMaterialIf* mat){}
	///	描画する点・線の太さの設定
	virtual void SetLineWidth(float w){}
	///	光源スタックをPush
	virtual void PushLight(const GRLightDesc& light){}
	virtual void PushLight(const GRLightIf* light){}
	///	光源スタックをPop
	virtual void PopLight(){}
	///	デプスバッファへの書き込みを許可/禁止する
	virtual void SetDepthWrite(bool b){}
	///	デプステストを有効/無効にする
	virtual void SetDepthTest(bool b){}
	///	デプスバッファ法に用いる判定条件を指定する
	virtual void SetDepthFunc(GRRenderBaseIf::TDepthFunc f){}
	/// アルファブレンディングを有効/無効にする
	virtual void SetAlphaTest(bool b){}
	///	アルファブレンディングのモード設定(SRCの混合係数, DEST混合係数)
	virtual void SetAlphaMode(GRRenderBaseIf::TBlendFunc src, GRRenderBaseIf::TBlendFunc dest){}
	/// テクスチャのロード（戻り値：テクスチャID）
	virtual unsigned int LoadTexture(const std::string filename){return 0;}
	/// シェーダの初期化
	virtual void InitShader(){}
	/// シェーダフォーマットの設定
	virtual void SetShaderFormat(GRShaderFormat::ShaderType type){}	
	/// シェーダオブジェクトの作成
	virtual bool CreateShader(std::string vShaderFile, std::string fShaderFile, GRHandler& shader){return 0;}
	/// シェーダオブジェクトの作成、GRDeviceGL::shaderへの登録（あらかじめShaderFile名を登録しておく必要がある）	
	virtual GRHandler CreateShader(){return 0;}
	/// シェーダのソースプログラムをメモリに読み込み、シェーダオブジェクトと関連付ける
	virtual bool ReadShaderSource(GRHandler shader, std::string file){return 0;}	
	/// ロケーション情報の取得（SetShaderFormat()でシェーダフォーマットを設定しておく必要あり）
	virtual void GetShaderLocation(GRHandler shader, void* location){}
};

/**	@class	GRRender
    @brief	グラフィックスレンダラーの基本クラス（デバイスの切り分け）　 */
class GRRender: public GRRenderBase, public GRRenderIfInit{
	OBJECTDEF(GRRender, GRRenderBase);
protected:
	UTRef<GRDeviceIf> device;		///<	デバイス
	GRCameraDesc camera;			///<	カメラ
	Vec2f viewportPos;				///<	ビューポートの左上
	Vec2f viewportSize;				///<	ビューポートのサイズ
public:
#define REDIRECTIMP_GRRENDERBASE(ptr)																		\
	virtual void SetViewport(Vec2f p, Vec2f s){ ptr SetViewport(p, s); }									\
	virtual void ClearBuffer(){ ptr ClearBuffer(); }														\
	virtual void BeginScene(){ ptr BeginScene(); }															\
	virtual void EndScene(){ ptr EndScene(); }																\
	virtual void SetViewMatrix(const Affinef& afv){ ptr SetViewMatrix(afv); }								\
	virtual void SetProjectionMatrix(const Affinef& afp){ ptr SetProjectionMatrix(afp); }					\
	virtual void GetProjectionMatrix(const Affinef& afp){ ptr GetProjectionMatrix(afp); }					\
	virtual void SetModelMatrix(const Affinef& afw){ ptr SetModelMatrix(afw); }								\
	virtual void MultModelMatrix(const Affinef& afw){ ptr MultModelMatrix(afw); }							\
	virtual void PushModelMatrix(){ ptr PushModelMatrix(); }												\
	virtual void PopModelMatrix(){ ptr PopModelMatrix(); }													\
	virtual bool SetBlendMatrix(const Affinef& afb){ return ptr SetBlendMatrix(afb); }						\
	virtual bool SetBlendMatrix(const Affinef& afb, unsigned int id){ return ptr SetBlendMatrix(afb, id); }	\
	virtual void ClearBlendMatrix(){ ptr ClearBlendMatrix(); }												\
	virtual void SetVertexFormat(const GRVertexElement* f){ ptr SetVertexFormat(f); }						\
	virtual void DrawDirect(GRRenderBaseIf::TPrimitiveType ty, void* vtx, size_t ct, size_t st=0)			\
		{ ptr DrawDirect(ty, vtx, ct, st); }																\
	virtual void DrawIndexed(GRRenderBaseIf::TPrimitiveType ty,												\
		size_t* idx, void* vtx, size_t ct, size_t st=0)														\
		{ ptr DrawIndexed(ty, idx, vtx, ct, st); }															\
	virtual void DrawSphere(float radius, int stacks, int slice)											\
		{ ptr DrawSphere(radius, stacks, slice); }															\
	virtual int StartList()																					\
		{ return ptr StartList(); }																			\
	virtual void EndList()																					\
		{ ptr EndList(); }																					\
	virtual void DrawList(int i){ ptr DrawList(i); }														\
	virtual void ReleaseList(int i){ ptr ReleaseList(i); }													\
	virtual void DrawFont(Vec2f pos, const std::string str){ ptr DrawFont(pos, str); }						\
    virtual void DrawFont(Vec3f pos, const std::string str){ ptr DrawFont(pos, str); }						\
	virtual void DrawFont(Vec2f pos, const std::string str, const GRFont& font)								\
		{ ptr DrawFont(pos, str, font); }																	\
	virtual void DrawFont(Vec3f pos, const std::string str, const GRFont& font)								\
		{ ptr DrawFont(pos, str, font); }																	\
	virtual void SetMaterial(const GRMaterialDesc& mat){ ptr SetMaterial(mat); }							\
	virtual void SetMaterial(const GRMaterialIf* mat)														\
		{ if(mat) ptr SetMaterial((const GRMaterialDesc&)*DCAST(GRMaterial, mat)); }						\
	virtual void SetLineWidth(float w){ ptr SetLineWidth(w); }												\
	virtual void PushLight(const GRLightDesc& light){ptr PushLight(light);}									\
	virtual void PushLight(const GRLightIf* light)															\
		{ if(light) ptr PushLight((const GRLightDesc&)*DCAST(GRLight, light)); }							\
	virtual void PopLight(){ptr PopLight(); }																\
	virtual void SetDepthWrite(bool b){ ptr SetDepthWrite(b); }												\
	virtual void SetDepthTest(bool b){ptr SetDepthTest(b); }												\
	virtual void SetDepthFunc(GRRenderBaseIf::TDepthFunc f){ ptr SetDepthFunc(f); }							\
	virtual void SetAlphaTest(bool b){ptr SetAlphaTest(b); }												\
	virtual void SetAlphaMode(GRRenderBaseIf::TBlendFunc src, GRRenderBaseIf::TBlendFunc dest)				\
		{ ptr SetAlphaMode(src, dest); }																	\
	virtual unsigned int LoadTexture(const std::string filename){ return ptr LoadTexture(filename); }		\
	virtual void InitShader(){ ptr InitShader(); }															\
	virtual void SetShaderFormat(GRShaderFormat::ShaderType type){ ptr SetShaderFormat(type); }				\
	virtual bool CreateShader(std::string vShaderFile, std::string fShaderFile, GRHandler& shader)			\
		{ return ptr CreateShader(vShaderFile, fShaderFile, shader); }										\
	virtual GRHandler CreateShader(){ return ptr CreateShader(); }											\
	virtual bool ReadShaderSource(GRHandler shader, std::string file)										\
		{ return ptr ReadShaderSource(shader, file); }														\
	virtual void GetShaderLocation(GRHandler shader, void* location)										\
		{ ptr GetShaderLocation(shader, location); }														\
	
	REDIRECTIMP_GRRENDERBASE(device->)
	
	///	デバイスの設定
	virtual void SetDevice(GRDeviceIf* dev){ device = dev; }
	///	デバッグ表示
	virtual void Print(std::ostream& os) const;
	///	カメラの設定
	void SetCamera(const GRCameraDesc& c);
	const GRCameraDesc& GetCamera(){ return camera; }
	///	スクリーンサイズとプロジェクション行列の設定
	virtual void Reshape(Vec2f pos, Vec2f sz);
};

/**	@class	GRDevice
    @brief	グラフィックス描画の実装　 */
class GRDevice: public GRRenderBase, public GRDeviceIfInit{
public:
	OBJECTDEF_ABST(GRDevice, GRRenderBase);
	virtual void Init(){}
	virtual void SetMaterial(const GRMaterialDesc& mat){}
	virtual void SetMaterial(const GRMaterialIf* mat){
		if(mat) SetMaterial(*DCAST(GRMaterial, mat)); }
	virtual void PushLight(const GRLightDesc& light){}
    virtual void PushLight(const GRLightIf* light){
        if(light) PushLight(*DCAST(GRLight, light)); }
};
}
#endif
