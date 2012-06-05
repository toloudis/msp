/****************************************************************************\
**  g3dRenderPass.hpp
**
**      g3dRenderPass.hpp is a render pass.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_RENDERPASS_HPP
#error g3dRenderPass.hpp multiply included
#endif
#define G3D_RENDERPASS_HPP

#ifndef G3D_RENDERSTATECACHE_HPP
#include "GraphicsDX11/g3d/g3dRenderStateCache.hpp"
#endif

#ifndef MA_VECTOR4D_HPP
#include "Core/ma/maVector4d.hpp"
#endif

class g2dRenderTarget;
class g3dSceneNode;
class matMaterial;

struct g3dRenderStats
{
	int m_nTriangles;

	g3dRenderStats() 
		:	m_nTriangles(0)
	{
	}

	void Reset()
	{
		m_nTriangles = 0;
	}

	void operator += ( const g3dRenderStats& i_Other )
	{ 
		m_nTriangles += i_Other.m_nTriangles;
	}

	g3dRenderStats operator + ( const g3dRenderStats& i_Other ) const
	{ 
		g3dRenderStats retVal;
		
		retVal.m_nTriangles = m_nTriangles + i_Other.m_nTriangles;

		return retVal;
	}
};

class g3dSceneNodeRenderer
{
public:
	virtual int Render(float i_time, const g3dSceneNode* i_pNode, g3dRenderStateCache i_cacheState) = 0;

	void SetRenderTarget(g2dRenderTarget* i_pRenderTarget) {m_pRenderTarget = i_pRenderTarget;}

	g3dRenderStats m_stats;

protected:
	g2dRenderTarget* m_pRenderTarget;
};

class g3dRenderPass
{
public:
	g3dRenderPass();
	virtual ~g3dRenderPass(void);

	virtual void PerFrameInit(float i_time) {}
	virtual int Render(float i_time) = 0;
	virtual void PerFrameCleanup() {}

	void SetRenderTarget(g2dRenderTarget* i_pRenderTarget) {m_pRenderTarget = i_pRenderTarget;}
	g2dRenderTarget* GetRenderTarget(){ return m_pRenderTarget; }

	g3dRenderStats m_stats;

	static void InitStates();
	static void CleanupStates();

protected:
	g2dRenderTarget* m_pRenderTarget;
};

#include <vector>
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/g2d/g2dFontHandle.hpp"
#endif
class g3dRenderText : public g3dRenderPass
{
public:
	g3dRenderText(std::vector<itString>& i_Messages, g2dFontHandle i_Font);
	virtual ~g3dRenderText(void);

	virtual int Render(float i_time);
protected:
	std::vector<itString> m_Messages;
	g2dFontHandle m_Font;
};

class matTextureDX11;
class matRenderTargetTexture;
#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

class g3dRenderFullScreenQuad : public g3dRenderPass
{
public:
	g3dRenderFullScreenQuad(matTextureDX11* i_tex, g2dRenderTarget* i_dest, bool clearDest = true);
	virtual ~g3dRenderFullScreenQuad(void);

	// 0 is near plane, 1 is far plane
	void SetDepth(float i_depth) {m_depth = i_depth;}

	virtual int Render(float i_time);

protected:
	virtual void InitShader();
	virtual void DrawQuad(float i_time);

	bool m_clearDest;

	matTextureDX11* m_source;

	float m_depth;
};

class matShaderEffect;
class g3dRenderDOF : public g3dRenderFullScreenQuad
{
public:
	g3dRenderDOF(matTextureDX11* i_Tex, matTextureDX11* i_Blurry,
		g2dRenderTarget* i_Target);
	virtual ~g3dRenderDOF(void);

protected:
	virtual void InitShader();
	virtual void DrawQuad(float i_time);

	matShaderEffect* m_Effect;
	matTextureDX11* m_Blurry;
};

class g3dRenderAlpha : public g3dRenderFullScreenQuad
{
public:
	g3dRenderAlpha(matTextureDX11* i_tex, g2dRenderTarget* i_target);
	virtual ~g3dRenderAlpha(void);

protected:
	virtual void InitShader();
	virtual void DrawQuad(float i_time);
	matShaderEffect* m_effect;
};

class g3dRenderBlur : public g3dRenderFullScreenQuad
{
public:
	g3dRenderBlur(matTextureDX11* i_src, matRenderTargetTexture* i_dest,
		matRenderTargetTexture* i_intermediate0,
		matRenderTargetTexture* i_intermediate1);
	virtual ~g3dRenderBlur(void);

protected:
	virtual void InitShader();
	virtual void DrawQuad(float i_time);

	matShaderEffect* m_effect;
	int m_paramSrcTex;
	int m_paramDownsampTex;
	int m_paramBlurTex;

	// separable blur: 1 horiz blur pass and 1 vert blur pass
	// requires an intermediate render target before completion
	matRenderTargetTexture* m_intermediateSurface0;
	matRenderTargetTexture* m_intermediateSurface1;
	matRenderTargetTexture* m_destinationSurface;
};

class camCamera;
class g3dLayer;
class g3dRenderLayer : public g3dRenderPass
{
public:
	g3dRenderLayer();
	g3dRenderLayer(
		g3dLayer*			i_pLayer,
		const camCamera*	i_pCamera,
		g2dRenderTarget*	i_pRenderTarget);
	virtual ~g3dRenderLayer(void) {}

	virtual int Render(float i_time);

	void Set(
		g3dLayer*			i_pLayer,
		const camCamera*	i_pCamera,
		g2dRenderTarget*	i_pRenderTarget);
protected:
	g3dLayer*			m_pLayer;
	const camCamera*			m_pCamera;

	virtual void DataChanged() {}
};

// this is an overlay post effect, expecting the dest target to have a scene in it
class g3dRenderGlow : public g3dRenderFullScreenQuad
{
public:
	g3dRenderGlow(matTextureDX11* i_src, g2dRenderTarget* i_dest, const g3dSceneNode* i_node, float i_depth = 0.0f);
	virtual ~g3dRenderGlow(void);

protected:
	virtual void InitShader();
	virtual void DrawQuad(float i_time);

	matShaderEffect* m_effect;
	const g3dSceneNode* m_pNode;
	float m_depth;
};

// this is an overlay post effect, expecting the dest target to have a scene in it
class g3dRenderOutline : public g3dRenderFullScreenQuad
{
public:
	g3dRenderOutline(matTextureDX11* i_src, g2dRenderTarget* i_dest,  const matMaterial* pMtl = NULL, bool i_UseNormals = false );
	virtual ~g3dRenderOutline(void);

protected:
	void InitShader();
	virtual void DrawQuad(float i_time);

	matShaderEffect* m_effect;
	const matMaterial*	m_mtl;
	bool m_bUseNormals;
};

// this is an overlay post effect, expecting the dest target to have a scene in it
class g3dRenderAA : public g3dRenderFullScreenQuad
{
public:
	g3dRenderAA(matTextureDX11* i_src, g2dRenderTarget* i_dest );
	virtual ~g3dRenderAA(void);

protected:
	void InitShader();
	virtual void DrawQuad(float i_time);

	matShaderEffect* m_effect;
	int m_paramSrcTex;
};
