#ifndef SPR_GRRENDER_H
#define SPR_GRRENDER_H

#include <float.h>
#include <Physics/SprPHSolid.h>

namespace Spr{;

/**	\addtogroup	gpGraphics	グラフィックス	*/
//@{

/**
 *	@file SprGRRender.h
 *	@brief グラフィックスレンダラーの基本クラス、基本インタフェース
 */

/**	@brief	テキスト描画のフォント */
class SPR_DLL GRFont{
public:
	int height;				///<	フォントの高さ
	int width;				///<	平均文字幅
	int weight;				///<	フォントの太さ（0 - 900）
	std::string face;		///<	タイプフェイス
	unsigned long color;	///<	フォントの色
	bool bItalic;			///<	イタリック体
	GRFont(int h=20, const char* f=NULL){
		height=h; width=0;
		face=f ? f : "";
		weight = 400; color=0xffffffff; bItalic=false;
	}
	void SetColor(unsigned long c){ color = c; }
	bool operator < (GRFont& f){
		if (face < f.face) return true;
		if (height < f.height) return true;
		if (color < f.color) return true;
		return false;
	}
	GRFont& operator=(const GRFont& rhs) {
		if (this==&rhs)	return *this;
		height	= rhs.height;
		width	= rhs.width;
		weight	= rhs.weight;
		face	= rhs.face;
		color	= rhs.color;
		bItalic	= rhs.bItalic;
		return *this;
	}
};

/**	@brief	光源		*/
struct GRLight{
    Vec4f ambient;		///<	環境光
    Vec4f diffuse;		///<	拡散光
    Vec4f specular;		///<	鏡面光
    Vec4f position;		///<	光源位置（w=1.0で点光源、w=0.0で平行光源）
    float range;		///<	光が届く範囲（deviceがDirectXの場合のみ利用可能）
	/**
	 *	@name	減衰パラメータ
	 *　　 これらを設定することで光の減衰を表現できる。 \n
	 *　　 減衰のパラメータを設定すると、光源から離れるほど高原による効果は小さくなる。 \n
	 *　　 デフォルトでは、減衰なしに設定されている。 \n
	 *　　 平行光源の場合、減衰はさせない。 \n
	 *　　　　 減衰係数 = 1/( att0 + att1 * d + att2 * d^2)　　　d:距離
	 *	@{
	 */
	float attenuation0;	///<	一定減衰率
    float attenuation1;	///<	線形減衰率
    float attenuation2;	///<	2次減衰率
	/** @} */	// end of 減衰パラメータ

	Vec3f spotDirection;///<	スポットライトの向き
    float spotFalloff;	///<	スポットライトの円錐内での減衰率(大きいほど急峻) 0..∞
	float spotInner;	///<	スポットライトの中心部分(内部コーン)（deviceがDirectXの場合のみ利用可能） 0..spotCutoff
	float spotCutoff;	///<	スポットライトの広がり角度(度)(外部コーン) 0..π(pi)
	GRLight(){
		range = FLT_MAX;
		attenuation0 = 1.0f;
		attenuation1 = 0.0f;
		attenuation2 = 0.0f;
		spotDirection = Vec3f(0.0, -1.0, 0.0);
		spotFalloff  = 0.0f;
		spotInner    = 0.0f;
		spotCutoff   = 180.0f;
	}
};

/**	@brief	グラフィックスの材質 */
struct GRMaterial{
	Vec4f ambient;					///<	環境光に対する反射率
	Vec4f diffuse;					///<	拡散光に対する反射率
	Vec4f specular;					///<	鏡面光に対する反射率
	Vec4f emissive;					///<	放射輝度
	float power;					///<	鏡面反射の強度、鏡面係数
	std::string texture;			///<	テクスチャファイル名
	GRMaterial(){ power = 0.0f; }
	GRMaterial(Vec4f a, Vec4f d, Vec4f s, Vec4f e, float p):
		ambient(a), diffuse(d), specular(s), emissive(e), power(p){}
	GRMaterial(Vec4f c, float p):
		ambient(c), diffuse(c), specular(c), emissive(c), power(p){}
	GRMaterial(Vec4f c):
		ambient(c), diffuse(c), specular(c), emissive(c), power(0.0f){}
	/**	W()要素は、アルファ値(0.0〜1.0で透明度を表す). 1.0が不透明を表す.
		materialのW()要素を判定して、不透明物体か、透明物体かを判定する. 
		透明なオブジェクトを描くとき、遠くのものから順番に描画しないと、意図に反した結果となる. */
	bool IsOpaque(){		
		return ambient.W() >= 1.0 && diffuse.W() >= 1.0 && specular.W() >= 1.0 && emissive.W() >= 1.0;
	}
};

/**	@brief	カメラの情報 */
struct GRCamera{
	Vec2f size;				///<	スクリーンのサイズ
	Vec2f center;			///<	カメラからのスクリーンのずれ
	float front, back;		///<	視点からクリップ面までの相対距離（正の値で指定）
	GRCamera(Vec2f initSize=Vec2f(480.0, 360.0), 
				Vec2f initCenter=Vec2f(0.0, 0.0),
				float initFront=1.0,
				float initBack=5000.0) 
			: size(initSize), center(initCenter), front(initFront), back(initBack) {}
};

struct GRDeviceIf;

/**	@brief	グラフィックスレンダラーの基本クラス（ユーザインタフェース） */
struct GRRenderBaseIf: public ObjectIf{
	IF_DEF(GRRenderBase);
	///	プリミティブの種類
	enum TPrimitiveType {
		POINTS,
		LINES,
		LINE_STRIP,
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN
	};
	/**
	 *	@name	デプスバッファ法に用いる判定条件
	 *	@{
	 */
	enum TDepthFunc{
		DF_NEVER,		///<	新しいZ値に関係なく更新しない
		DF_LESS,		///<	新しいZ値が小さければ更新する（default)
		DF_EQUAL,		///<	新しいZ値が等しければ描かれる
		DF_LEQUAL,		///<	新しいZ値が大きくなければ描かれる
		DF_GREATER,		///<	新しいZ値が大きければ描かれる
		DF_NOTEQUAL,	///<	新しいZ値と等しくなければ描かれる
		DF_GEQUAL,		///<	新しいZ値が小さくなければ描かれる
		DF_ALWAYS		///<	新しいZ値に関係なく更新する
	};
	/** @} */	
	/**
	 *	@name	アルファブレンディングの混合係数
	 *　　 SRCがこれから描画される合成させたい色、DESTがすでに描かれたカラーバッファの色    \n
	 *　　　　 合成結果 =SRC * SRCのブレンディング係数 + DEST * DESTのブレンディング係数 			\n
	 *　　 ただし、BF_BOTHINVSRCALPHA、BF_BLENDFACTOR、BF_INVBLENDFACTOR に関しては、			\n
	 *　　 deviceがDirectXのときのみ利用可能。指定してもBF_ZEROとして扱う。
	 *	@{
	 */
	enum TBlendFunc{
		BF_ZERO,				///<	(0,0,0,0)	
		BF_ONE,					///<	(1,1,1,1)
		BF_SRCCOLOR,			///<	(Rs, Gs, Bs, As)
		BF_INVSRCCOLOR,			///<	(1-Rs, 1-Gs, 1-Bs, 1-As) 
		BF_SRCALPHA,			///<	(As, As, As, As) 
		BF_INVSRCALPHA,			///<	(1-As, 1-As, 1-As, 1-As)
		BF_DESTALPHA,			///<	(Ad, Ad, Ad, Ad)  
		BF_INVDESTALPHA,		///<	(1-Ad, 1-Ad, 1-Ad, 1-Ad) 
		BF_DESTCOLOR,			///<	(Rd, Gd, Bd, Ad)
		BF_INVDESTCOLOR,		///<	(1-Rd, 1-Gd, 1-Bd, 1-Ad) 
		BF_SRCALPHASAT,			///<	(f, f, f, 1)、f = min(As, 1-Ad) 
		BF_BOTHINVSRCALPHA,		///<	SRCブレンディング係数は(1-As, 1-As, 1-As, 1-As)。DSTブレンディング係数は(As, As, As, As)。
		BF_BLENDFACTOR,			///<	フレームバッファ合成時に使われる定数色で合成
		BF_INVBLENDFACTOR		///<	フレームバッファ合成時に使われる定数色を反転した色で合成
	};
	/** @} */	
	///	バッファクリア
	virtual void ClearBuffer()=0;
	///	レンダリングの開始前に呼ぶ関数
	virtual void BeginScene()=0;
	///	レンダリングの終了後に呼ぶ関数
	virtual void EndScene()=0;
	///	モデル行列をかける
	virtual void MultModelMatrix(const Affinef& afw)=0;
	///	モデル行列の行列スタックをPush
	virtual void PushModelMatrix()=0;
	///	モデル行列の行列スタックをPop
	virtual void PopModelMatrix()=0;
	///	モデル行列を設定
	virtual void SetModelMatrix(const Affinef& afw)=0;
	///	視点行列を設定
	virtual void SetViewMatrix(const Affinef& afv)=0;
	///	投影行列を設定
	virtual void SetProjectionMatrix(const Affinef& afp)=0;
	///	頂点座標を指定してプリミティブを描画
	virtual void DrawDirect(TPrimitiveType ty, Vec3f* begin, Vec3f* end)=0;
	///	頂点座標とインデックスを指定してプリミティブを描画
	virtual void DrawIndexed(TPrimitiveType ty, size_t* begin, size_t* end, Vec3f* vtx)=0;
	///	2次元テキストの描画　　 Windows環境(VC)でのみfontをサポートし、他の環境ではfontを指定しても利用されない。
	virtual void DrawFont(Vec2f pos, const std::string str, const GRFont& font=0)=0;
	///	3次元テキストの描画　　 Windows環境(VC)でのみfontをサポートし、他の環境ではfontを指定しても利用されない。	
	virtual void DrawFont(Vec3f pos, const std::string str, const GRFont& font=0)=0;
	///	描画の材質の設定
	virtual void SetMaterial(const GRMaterial& mat)=0;
	///	描画する点・線の太さの設定
	virtual void SetLineWidth(float w)=0;
	///	光源スタックをPush
	virtual void PushLight(const GRLight& light)=0;
	///	光源スタックをPop
	virtual void PopLight()=0;
	///	デプスバッファへの書き込みを許可/禁止する
	virtual void SetDepthWrite(bool b)=0;
	///	デプステストを有効/無効にする
	virtual void SetDepthTest(bool b)=0;
	///	デプスバッファ法に用いる判定条件を指定する
	virtual void SetDepthFunc(TDepthFunc f)=0;
	/// アルファブレンディングを有効/無効にする
	virtual void SetAlphaTest(bool b)=0;
	///	アルファブレンディングのモード設定(SRCの混合係数, DEST混合係数)
	virtual void SetAlphaMode(TBlendFunc src, TBlendFunc dest)=0;
};

/**	@brief	グラフィックスレンダラーの基本クラス（デバイスの設定、カメラの設定） */
struct GRRenderIf: public GRRenderBaseIf{
	IF_DEF(GRRender);
	virtual void SetDevice(GRDeviceIf* dev)=0;
	virtual void SetCamera(GRCamera& cam)=0;
};

/**	@brief	グラフィックスレンダラーのデバイスクラス．OpenGLやDirectXのラッパ */
struct GRDeviceIf: public GRRenderBaseIf{
	IF_DEF(GRDevice);
	///	初期化
	virtual void Init()=0;
	///	デバッグ用の状態レポート
	virtual void Print(std::ostream& os) const=0;
};

/**	@brief	OpenGLのレンダラー基本クラス */
struct GRDeviceGLIf: public GRDeviceIf{
	IF_DEF(GRDeviceGL);
	/// ウィンドウIDを設定する
	virtual void SetWindow(int w)=0;
};

/**	@brief	DirectXのレンダラー基本クラス */
struct GRDeviceD3DIf: public GRDeviceIf{
	IF_DEF(GRDeviceD3D);
};

/**	@brief	デバッグ情報レンダラーの基本クラス */
struct GRDebugRenderIf:public GRRenderIf{
	IF_DEF(GRDebugRender);
	///	Viewportと射影行列を設定
	virtual void Reshape(Vec2f screen)=0;
	/// 剛体をレンダリングする
	virtual void DrawSolid(PHSolidIf* so)=0;
	/// 面をレンダリングする
	virtual void DrawFace(CDFaceIf* face, Vec3f * base)=0;
};

//@}


}
#endif
