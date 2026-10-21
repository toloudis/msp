/****************************************************************************\
**	shdwPassMaterials.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSMATERIALS_HPP
#include "GraphicsVulkan/shdw/shdwPassMaterials.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#ifndef MA_FUNCTIONS_HPP
#include "Core/ma/maFunctions.hpp"
#endif

#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/g2d/g2dRenderTarget.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

namespace
{
	matMaterial l_MaterialsMat;
	matMaterial l_HairMat;

	g3dBlendStateMgr::BlendState* st_AddBlend = NULL;
	g3dBlendStateMgr::BlendState* st_AddNoBlend = NULL;
}; // namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassMaterials::shdwPassMaterials(g2dRenderTarget* i_pMaterialsTarget,
									 const camCamera* i_pCamera,
									 std::map<std::string, float>* i_ColorMap,
									 float * i_LastNumGenerated,
									 int * i_TotalColors,
									 matRenderTargetTexture* i_pSrcMaterials )
	: m_sceneInfo(NULL), m_pOrigCamera( i_pCamera ), m_pSrcMaterialsBuffer( i_pSrcMaterials ),
	  m_ColorMap(i_ColorMap), m_LastNumGenerated(i_LastNumGenerated), m_TotalColors(i_TotalColors),
	  m_CurrNodeIdx(0)
{
	m_pRenderTarget = i_pMaterialsTarget;

	//copy the camera so we can modify it for overscan;
	m_Camera = camCamera( *i_pCamera );

	// lazy init so that this material can be reused across instantiations.
	if (!l_MaterialsMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/Solid.fx"), matShaderMgr::GetSpecialEffect("Solid.fx"));
		l_MaterialsMat.SetShaderParams(p);
	}
	// lazy init so that this material can be reused across instantiations.
	if (!l_HairMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/HairDefault.fx"), matShaderMgr::GetSpecialEffect("HairDefault.fx"));
		l_HairMat.SetShaderParams(p);
	}
	m_OverscanPixels = 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassMaterials::~shdwPassMaterials()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassMaterials::GenerateHSVStruct(int i_NumNodes)
{
	float lastHueGenerated = 0;
	float lastValGenerated = 0;

	int numIterations = 0;
	for ( int i = 0 ; i < i_NumNodes ; i++ )
	{
		lastHueGenerated = GenerateNextNumAlternating(0,1,lastHueGenerated);

		HSV currHSV;
		if ( i < MAX_HUES )
		{
			currHSV.hue = lastHueGenerated;
			currHSV.sat = 1;
			currHSV.val = 1;
			m_HSVs.push_back( currHSV );
		}

		if ( i%MAX_HUES == 0 && i != 0 )
		{
			numIterations++;
			lastValGenerated = GenerateNextNumAlternating(0,1,lastValGenerated);
		}

		if ( i >= MAX_HUES )
		{
			int idx = i - (numIterations*MAX_HUES);
			currHSV.hue = m_HSVs[idx].hue;
			currHSV.sat = 1;
			currHSV.val = lastValGenerated;
			m_HSVs.push_back( currHSV );
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int shdwPassMaterials::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassMaterials");
	m_stats.Reset();

	// Clear surfaces
	m_pRenderTarget->Clear(maFloatRGBA(0,0,0,0),true);

	// Write to velocity surface
	m_pRenderTarget->MakeCurrent();

	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	g3dBlendStateMgr::SetBlendState(st_AddNoBlend);

	//set altered projection camera
	g3dSceneRenderUtil::SetViewingTransforms(m_Camera);
	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	g3dTransparencySortVk* transparencySort = m_sceneInfo->GetTransparentNodes();
	const TranspNodeVector& transpNodes = transparencySort->GetTransparentNodes();

	int totalNodes = nonSolidNodes.size() + nonShadowNodes.size() + shadowNodes.size() + transpNodes.size();

	#ifdef HAIR_SUPPORTED
	if (p.m_bEnableHair)
	{
		g3dTransparencySortVk* pHairNodes = m_sceneInfo->GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		totalNodes += tHNodes.size();
	}
	#endif

	GenerateHSVStruct(totalNodes);

	int i,n;
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}

	//hair nodes
#ifdef HAIR_SUPPORTED
	if ( p.m_bEnableHair)
	{
		g3dTransparencySortVk* pHairNodes = m_sceneInfo->GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tHNodes.end();

		for (it = tHNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;
			m_stats.m_nTriangles += RenderHairNode( NS );
		}
	}
#endif//HAIR_SUPPORTED
	if( p.m_bEnableTransparent )
	{
		n = transpNodes.size();
		for (i = 0; i < n; i++)
		{
			const TranspNode& transpNode = transpNodes[i];
			sNodePlusState currentNode;
			currentNode.m_pNode = transpNode.m_pSceneNode;

			bool doRender = true;

			g3dFragment * frag = (g3dFragment*)currentNode.m_pNode->GetFragment();
			if ( frag )
			{
				matMaterial * mat = frag->GetMaterial();
				if ( mat )
				{
					doRender = !mat->GetBelongsToLightShaft();
				}
			}

			if ( doRender )
			{
				m_stats.m_nTriangles += RenderNode(currentNode);
			}
		}
	}


	// restore some state.
	g3dBlendStateMgr::SetBlendState(st_AddBlend);
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	//restore original camera transform
	g3dSceneRenderUtil::SetViewingTransforms(*m_pOrigCamera);

	m_CurrNodeIdx = 0;

	return m_stats.m_nTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int shdwPassMaterials::RenderNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;
	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// resolve material/effect
	matMaterial* pMaterial = &l_MaterialsMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	pEffect->SetTechnique("Default");

	const g3dFragment* pFrag = i_Node.m_pNode->GetFragment();

	matMaterial * currMat = (matMaterial*)i_Node.m_pNode->GetFragment()->GetMaterial();

	float myColor[4];
	float r = 0;
	float g = 0;
	float b = 0;

	maFunctions::HSV_to_RGB( m_HSVs[m_CurrNodeIdx].hue,
							 m_HSVs[m_CurrNodeIdx].sat,
							 m_HSVs[m_CurrNodeIdx].val,
							 r,g,b );
	myColor[0] = r;
	myColor[1] = g;
	myColor[2] = b;
	myColor[3] = 1;

	pD3DEffect->GetVariableByName("solidColor")->AsVector()->SetFloatVector(myColor);

	//handle transparent parameters
	float val = 1.0f;
	matTexture* pTex = NULL;
	const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
	if( pOrigMaterial->GetHasTransparency() )
	{
		shared_ptr<effShaderParams> parms = pOrigMaterial->GetShaderParams();
		if( parms )
		{
			effParamTexture* pTexParam = parms->m_pTransparencyMap;
			if( pTexParam ) pTex = pTexParam->GetTexture();
			effParamFloat* pVar = parms->m_pTransparency;
			if( pVar ) val = pVar->GetProperty().GetValue();
		}
		else
		{
			effShaderData* data = pOrigMaterial->GetEffectData();
			if( data )
			{
				pTex = data->GetTransparencyTexture();
				val = data->GetTransparencyValue();
			}
		}
	}

	pD3DEffect->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool((pTex!=NULL) ? true : false);
	pD3DEffect->GetVariableByName("TransparencyMap")->AsShaderResource()->SetResource(
		(pTex!=NULL) ? g3dVkTextureUtil::GetImage(pTex) : NULL);
	pD3DEffect->GetVariableByName("g_Transparency")->AsScalar()->SetFloat(val);


	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	m_CurrNodeIdx++;

	return nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
//----------------------------------------------------------------------------------------
int shdwPassMaterials::RenderHairNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_HairMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	pEffect->SetTechnique("Solid");

	matMaterial * currMat = (matMaterial*)i_Node.m_pNode->GetFragment()->GetMaterial();

	float myColor[4];

	float r = 0;
	float g = 0;
	float b = 0;

	maFunctions::HSV_to_RGB( m_HSVs[m_CurrNodeIdx].hue,
							 m_HSVs[m_CurrNodeIdx].sat,
							 m_HSVs[m_CurrNodeIdx].val,
							 r,g,b );
	myColor[0] = r;
	myColor[1] = g;
	myColor[2] = b;
	myColor[3] = 1;

	pEffect->SetupMaterial( pMaterial );

	pD3DEffect->GetVariableByName("solidColor")->AsVector()->SetFloatVector(myColor);
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	m_CurrNodeIdx++;


	return nTriangles;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
float shdwPassMaterials::GenerateHue(const std::string& i_MaterialName)
{
	std::string hashString = i_MaterialName;

	float hashNum = hash_material(i_MaterialName);
	float nextNum = 0;//GenerateNextNumber(0,1,*m_LastNumGenerated);

	int conflicts = 0;
	bool reset = false;

	std::map<std::string, float>::iterator it = m_ColorMap->begin();
	std::map<std::string, float>::iterator end = m_ColorMap->end();

	// inspecting hues in map
	while( it != end )
	{
		// if hue already present, make new hue
		if ( it->second == nextNum )
		{
			hashString.append( "*" );
			hashNum = hash_material(i_MaterialName);
			reset = true;
		}
		++it;

		// inspect map from beginning again
		if ( reset )
		{
			it = m_ColorMap->begin();
			reset = false;
		}
	}

	m_ColorMap->insert(std::pair<std::string, float>(hashString,hashNum));
	(*m_LastNumGenerated) = nextNum;
	(*m_TotalColors)++;

	return hashNum;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
float shdwPassMaterials::hash_material(const std::string i_MaterialName)
{
	float retVal = 0;
#ifdef USE_HASH_ONE
	retVal = hash_material_one(i_MaterialName)
#endif

#ifdef USE_HASH_TWO
	retVal = hash_material_two(i_MaterialName)
#endif

#ifdef USE_HASH_THREE
	retVal = hash_material_three(i_MaterialName)
#endif
	return retVal;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float shdwPassMaterials::hash_material_one(const std::string i_MaterialName)
{

	const uint32_t mask = 0x00ffffff;

	uint32_t result = 0;
	int len = i_MaterialName.length();
	if (len < 16)
	{
		for (int ind = 0; ind < len; ind++)
		{
			result = (result * 37) + i_MaterialName[ind];
			result &= mask;
		}
	}
	else
	{
		int skip = len / 8;
		for (int ind = 0; ind < len; ind+=skip)
		{
			result = (result * 39) + i_MaterialName[ind];
			result &= mask;
		}
	}

	return ((float)result) / ((float)mask);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
float shdwPassMaterials::hash_material_two(const std::string i_MaterialName)
{
	unsigned char * str = (unsigned char *)i_MaterialName.c_str();

	const uint32_t mask = 0x00ffffff;
    unsigned long result = 5381;
    int c;

    while (c = *str++ )
	{
        result = ((result << 5) + result) + c;
	}

	result &= mask;

	return ((float)result) / ((float)mask);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float shdwPassMaterials::hash_material_three(const std::string i_MaterialName)
{
	unsigned char * str = (unsigned char *)i_MaterialName.c_str();
	const uint32_t mask = 0x00ffffff;

	unsigned long result = 0;
	int c;

	while (c = *str++)
	{
		result = c + (result << 6) + (result << 16) - result;
	}

	result &= mask;

	return ((float)result) / ((float)mask);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
float shdwPassMaterials::GenerateNextNumAlternating(float i_min, float i_max, float i_LastNum)
{
	bool foundLastNum = false;
	float numerator = i_max - i_min;
	float denominator = numerator * 2;
	float lastNumerator = 0;

	while (true)
	{
		float currNum = numerator/denominator;

		if ( foundLastNum || i_LastNum == 0 )
		{
			return currNum;
		}

		if ( numerator == ((denominator/2) + 1) || currNum == 0.5 )
		{
			numerator = 1;
			denominator *= 2;
		}
		else
		{
			if ( currNum > 0.5 )
			{
				numerator = lastNumerator + 2;
			}
			else
			{
				lastNumerator = numerator;
				numerator = denominator - numerator;
			}
		}

		if ( currNum == i_LastNum )
		{
			foundLastNum = true;
		}

	}
}

void shdwPassMaterials::InitStates()
{
	st_AddBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT );
	st_AddNoBlend = new g3dBlendStateMgr::BlendState ( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
}

void shdwPassMaterials::CleanupStates()
{
	delete st_AddBlend;
	st_AddBlend = NULL;
	delete st_AddNoBlend;
	st_AddNoBlend = NULL;
}

}
