/*****************************************************************************
**  matShaderBaseGL.cpp
**
**      matShaderBaseGL is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Area18/mat/matShaderBaseGL.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effGlowData.hpp"
#include "Graphics/eff/effRendermanOverrideData.hpp"
#include "Graphics/eff/effNormalsData.hpp"
#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/eff/effOutlineData.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dPassBuffers.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Area18/g3d/g3dLightMgrOGL.hpp"
#include "Area18/mat/matShaderParamsGL.hpp"
#include "Area18/ogl/oglContext.h"
#include "Area18/shdr/shdrPipeline.hpp"
#include "Area18/shdr/shdrShader.hpp"

#include <algorithm>
#include <string>

#define GLTRANSPOSE GL_FALSE

//============================================================================
//============================================================================
namespace 
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	struct LightInfo
	{
		maPoint4d m_Position;
		maFloatRGBA m_Diffuse;
		maFloatRGBA m_Specular;
		maVector4d m_Attenuation;
		maVector4d m_ConeInfo;
		float m_FalloffStart;

		LightInfo() : m_Position(0,0,0,1), 
			m_Diffuse(0,0,0,1), m_Specular(0,0,0,0), 
			m_Attenuation(1,0,0,0), m_ConeInfo(0,0,1,0),
			m_FalloffStart(0.0f)
		{
		}
	};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	struct ProjLightInfo
	{
		maPoint4d m_Position;
		maMatrix4x4 m_TextureMatrix;
		float m_LightSize, m_PCSSAdjust, m_Scale, m_shadowIntensity;
		maFloatRGBA m_ShadowColor;
		float m_Near, m_Far;
		float m_MapSize;
		float m_InnerAngle;
		float m_OuterAngle;
		float m_Aspect;

		ProjLightInfo() : m_Position(0,0,0,1), m_LightSize(0.0f), m_PCSSAdjust(0.0f), m_Scale(200.0f),
			m_shadowIntensity(1.0f), m_Near(1), m_Far(1000), m_MapSize(1024), 
			m_InnerAngle(180), m_OuterAngle(90), m_Aspect(1)

		{
		}
	};

	//--------------------------------------------------------------------
	// sortable strudture for evaluating which are the best lights
	//--------------------------------------------------------------------
	struct sLightDistanceInfo
	{
		g3dLight *m_pLight;
		float m_DistSqr;
	};
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool operator<( const sLightDistanceInfo& a, const sLightDistanceInfo& b )
	{
		return (a.m_DistSqr < b.m_DistSqr);
	}

	//--------------------------------------------------------------------
	// Get an evaluation of how important this light is to this bbox
	// in order to sort the lights later to pick the best 8 lights.
	// Since this is in the non-shadow pass, then it is not critical
	// to be exact. So, using the distance from the target to the bbox center
	// as an approximation. This will at least make sure that lights aimed
	// directly at a small object will show up.
	//--------------------------------------------------------------------
	float evaluate_light(g3dLight *i_pLight,
						 const maAxisBox& i_BBox)
	{
		maPoint3d center = i_BBox.GetCenter();
		const g3dProjectedLight* proj_light = dynamic_cast<const g3dProjectedLight*>(i_pLight);
		if (proj_light)
		{
			return (proj_light->GetTarget() - center).LengthSqr();
		}

		const g3dPointLight* point_light = dynamic_cast<const g3dPointLight*>(i_pLight);
		if (point_light)
		{
			return (point_light->GetPosition() - center).LengthSqr();
		}

		// Directional lights affect everything, might as well include them also.
		return 0.0f;
	}


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void get_light_info(const g3dLight* i_pLight,
						LightInfo &o_Info)
	{
		// dmt - why aren't these virtual functions? we don't really need these casts... do we?

		const g3dPointLight* point_light = dynamic_cast<const g3dPointLight*>(i_pLight);
		if (point_light)
		{
			maPoint3d pos = point_light->GetPosition();
			o_Info.m_Position.Set(pos.m_X, pos.m_Y, pos.m_Z, 1.0f);

			//o_Info.m_Attenuation.Set(point_light->GetFalloff0(), point_light->GetFalloff1(), point_light->GetFalloff2());
			o_Info.m_Attenuation.Set(point_light->GetFalloff0(), point_light->GetFalloff1(), point_light->GetFalloff2(), point_light->GetFalloff3());
			o_Info.m_FalloffStart = point_light->GetFalloffStart();
			//o_Info.m_Range = point_light->GetRange();
		}
		const g3dDirectionalLight* dir_light = dynamic_cast<const g3dDirectionalLight*>(i_pLight);
		if (dir_light)
		{
			maVector3d dir = dir_light->GetDirection(); 
			// handy to reverse the direction
			o_Info.m_Position.Set(-dir.m_X, -dir.m_Y, -dir.m_Z, 0.0f);
		}
		const g3dProjectedLight* proj_light = dynamic_cast<const g3dProjectedLight*>(i_pLight);
		if (proj_light)
		{
			maPoint3d pos = proj_light->GetPosition();
			maVector3d dir = proj_light->GetDirection(); 

			if (proj_light->GetIsDirectional())
				o_Info.m_Position.Set(pos.m_X, pos.m_Y, pos.m_Z, 0.0f);
			else
				o_Info.m_Position.Set(pos.m_X, pos.m_Y, pos.m_Z, 1.0f);

			o_Info.m_ConeInfo.Set(-dir.m_X, -dir.m_Y, -dir.m_Z, 
				cosf(0.5f * proj_light->GetAngle() * maConstants::c_fAngleToRad));

			//o_Info.m_Attenuation.Set(proj_light->GetFalloff0(), proj_light->GetFalloff1(), proj_light->GetFalloff2());
			o_Info.m_Attenuation.Set(proj_light->GetFalloff0(), proj_light->GetFalloff1(), proj_light->GetFalloff2(), proj_light->GetFalloff3());
			//o_Info.m_Range = proj_light->GetRange();
			o_Info.m_FalloffStart = proj_light->GetFalloffStart();
		}

		maFloatRGBA light_color = i_pLight->GetIntensity();
//		if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererTypes::e_HDR)
		light_color *= i_pLight->GetIntensityFactor();
		light_color.SetAlpha(1.0f); // make sure light doesn't change alpha of color

		if (i_pLight->IsDiffuseEnabled() && g3dPrefs::CurrentPrefs().m_bEnableDiffuseLighting)
		{
			o_Info.m_Diffuse = light_color;
		}
		else 
		{
			o_Info.m_Diffuse.Set(0,0,0,1);
		}

		// when baking, turn off specular lighting contribution.
		if (i_pLight->IsSpecularEnabled() 
			&& (g3dPrefs::CurrentPrefs().m_bEnableSpecularLighting || g3dSingleLightRendering::GetDoGlowPass())
			&& !g3dSingleLightRendering::GetDoBaking())
		{
			o_Info.m_Specular = light_color;
			o_Info.m_Specular.SetAlpha(0.0f);  // need to keep alpha
		}
		else 
		{
			o_Info.m_Specular.Set(0,0,0,0);
		}
	}

	//------------------------------------------------------------------------
	// return single light for shaders
	//------------------------------------------------------------------------
	const g3dLight* get_active_light()
	{
		const g3dLight* pActive = g3dSingleLightRendering::GetActiveLight();
		if (pActive) 
			return pActive;

		// This could be set up to get the best light, but just choosing
		//	first light for now.
		const std::vector<g3dLight*>& light_list = g3dLightMgrOGL::Implementation()->GetLights();

		std::vector<g3dLight*>::const_iterator it = light_list.begin();
		std::vector<g3dLight*>::const_iterator end = light_list.end();
		for (; it != end; ++it)
		{
			if ((*it)->IsEnabled())
				return (*it);
		}

		return NULL;
	}

	//------------------------------------------------------------------------
	// return headlight light for shaders
	//------------------------------------------------------------------------
	const g3dLight* get_head_light()
	{
		std::vector<g3dLight*> light_list;
		g3dLightMgrOGL::Implementation()->GetEnabledLights(light_list);
		if (light_list.size() > 0)
			return light_list[0];
		else
			return NULL;
	}

	//------------------------------------------------------------------------
	// get up to 8 pos lights from scene. Pass in materials' diffuse and 
	//	specular color to combine the two
	//------------------------------------------------------------------------
	int get_some_lights(LightInfo o_LightsInfo[8], 
						const maAxisBox& i_BBox)
	{
		std::vector<g3dLight*> light_list;
		g3dLightMgrOGL::Implementation()->GetEnabledLights(light_list);

		for (int i=0; i<8; i++)
		{
			o_LightsInfo[i].m_Position.Set(0,0,0,1);
			o_LightsInfo[i].m_Diffuse.Set(0,0,0,1);
			o_LightsInfo[i].m_Specular.Set(0,0,0,1);
			o_LightsInfo[i].m_ConeInfo.Set(0,0,1,0);
		}

		// Sortable vector of light info in order to get the best lights
		std::vector<sLightDistanceInfo> light_sorter;
		{
			std::vector<g3dLight*>::iterator it = light_list.begin();
			std::vector<g3dLight*>::iterator end = light_list.end();
			for (; it != end; ++it)
			{
				g3dLight *pLight = (*it);
				if (pLight->IsEnabled())
				{
					sLightDistanceInfo info = { *it, evaluate_light(*it, i_BBox) };
					light_sorter.push_back( info );
				}
			}
		}

		std::sort(light_sorter.begin(), light_sorter.end());

		// This could be set up to get the best couple of lights,
		// but for now just get first ones found
		int num_lights = 0;
		{
			std::vector<sLightDistanceInfo>::iterator it = light_sorter.begin();
			std::vector<sLightDistanceInfo>::iterator end = light_sorter.end();
			for (; it != end; ++it)
			{
				get_light_info(it->m_pLight, o_LightsInfo[num_lights]);
				num_lights++;
				if (num_lights >= 8) break;
			}
		}

		return num_lights;
	}
#if 0
	void GetUIStrings(shdrCgPipeline* i_pEffect, GLint i_hParam, const std::string& i_Name, 
										std::string& o_Category, std::string& o_Label, std::string& o_Desc)
	{
		LPCSTR pstr = NULL;

		CGannotation hAnnot = NULL;

		// the label for the control
		o_Label = i_Name;
		hAnnot = cgGetNamedParameterAnnotation( i_hParam, "SasUiLabel" );
		if (hAnnot)
		{
			pstr = cgGetStringAnnotationValue(hAnnot);
			o_Label = pstr;
		}

		// the category header that the control will go under (see prtyUIInfo)
		o_Category = o_Label;
		hAnnot = cgGetNamedParameterAnnotation( i_hParam, "UiCategory" );
		if (hAnnot)
		{
			pstr = cgGetStringAnnotationValue(hAnnot);
			o_Category = pstr;
		}

		// the description string for the ui control
		o_Desc = o_Label;
		hAnnot = cgGetNamedParameterAnnotation( i_hParam, "SasUiDescription" );
		if (hAnnot)
		{
			pstr = cgGetStringAnnotationValue(hAnnot);
			o_Desc = pstr;
		}
	}

	GLint FindTextureExistVar(shdrCgPipeline* i_pEffect, GLint i_hTextureVar)
	{
		// conditional texture existence (null texture) flag
		GLint hRetVal = NULL;
		CGannotation hExistVarAnnot = cgGetNamedParameterAnnotation( i_hTextureVar, "ExistVar" );
		if (hExistVarAnnot)
		{
			LPCSTR existVarName = NULL;
			existVarName = cgGetStringAnnotationValue(hExistVarAnnot);
			GLint hExistVar = NULL;
			hExistVar = cgGetNamedEffectParameter(i_pEffect->Effect(), existVarName);
			if (hExistVar)
			{
				hRetVal = hExistVar;
				if (hRetVal != NULL)
				{
					// ensure that the var name points to a boolean.
					if (cgGetParameterType(hExistVar) != CG_BOOL)
					{
						//DBG_WARNING1("ExistVar for texture %s is not bool", name.c_str());
						//hExistVar = NULL;
					}
				}
			}
		}
		else
		{
			DBG_WARNING("This parameter does not have an ExistVar annotation");
		}
		return hRetVal;
	}

	// get resource filename from annotation for a parameter
	bool get_resource_name(GLint i_hParam, fsLocator &o_ResourceName)
	{
		// might also want to search for "resourceName" here (MetaSL backend does that)
		CGannotation hAnnot = cgGetNamedParameterAnnotation( i_hParam, "name" );
		if (hAnnot && cgIsAnnotation(hAnnot))
		{
			LPCSTR pstrName = NULL;
			pstrName = cgGetStringAnnotationValue(hAnnot);
			if (pstrName != NULL)
			{
				o_ResourceName.Clear();
				fsFileUtil::ANSIFilenameToLocator(pstrName, o_ResourceName);
				return true;
			}
		}
		return false;
	}
#endif
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matShaderBaseGL::matShaderBaseGL(const fsLocator& i_Directory, 
								   shdrPipeline* i_pEffect,
								   std::string i_Name)
:	m_pEffect(NULL),
	mContextPipeline(NULL),
	m_VecCameraPosHandle(-1),
	m_FirstLightHandle(-1),
	m_LightInfoHandle(-1), 
	m_LightArrayHandle(-1),
	m_LightArrayNumHandle(-1),
	m_ProjLightInfoHandle(-1), m_ProjLightTextureHandle(-1),
	m_ProjShadowMapHandle(-1), m_HasProjectedTextureHandle(-1),
	m_HasShadowMapHandle(-1),
	m_TessellatorMeshTextureHandle(-1),
	m_hMeshDataTextureWidthHandle(-1),
	m_hMeshDataTextureHeightHandle(-1),
	m_Name(""),
	m_TimeHandle(-1),
	m_IsProjLtHandle(-1),
	m_UVTransformHandle(-1),
	m_EnvHasDiffuseMapHandle(-1),
	m_EnvDiffuseMapHandle(-1),
	m_EnvDiffuseAngleHandle(-1),
	m_EnvDiffuseFactorHandle(-1),
	m_EnvDiffuseColorHandle(-1),
	m_EnvHasSpecularMapHandle(-1),
	m_EnvSpecularMapHandle(-1),
	m_EnvSpecularAngleHandle(-1),
	m_EnvSpecularFactorHandle(-1),
	m_EnvSpecularColorHandle(-1),
	m_DOFHandle(-1),
	m_DOFBlurCutoffHandle(-1),
	m_IsDoubleSidedHandle(-1),
	m_hBake(-1),
	m_hIsolateReflections(-1),
	m_hCubeMapEnabled(-1),
	m_hBakingTransform(-1),
	m_ReflectionMapIsPlanarHandle(-1),
	m_HasReflectionMapHandle(-1),
	m_CubeReflectionMapHandle(-1),
	m_PlanarReflectionMapHandle(-1),
	m_IsReflectionGenHandle(-1),
	m_pDefaults(NULL),
	m_SkinningMatrixPaletteHandle(-1),
	m_hHardwareTessellationHandle(-1),
	m_hHasDisplacementMap(-1),
	m_hDisplacementMap(-1),
	m_hDisplacementScale(-1),
	m_hDisplacementBias(-1),
	m_hDisplacementBlur(-1),
	m_hDisplacementObjUVScale(-1),
	m_hNormalMap(-1),
	m_hBumpScale(-1),
	m_hHasNormalMap(-1),
	m_hTessValueHandle(-1),
	m_AlphaTestRefHandle(-1),
	m_ClipPlaneHandle(-1)
//	m_HairTessellationHandle(-1)
{
	m_pEffect = i_pEffect;
	m_Name = i_Name;

	std::string dirstr;
	fsFileUtil::LocatorToANSIFilename(i_Directory, dirstr);
	DBG_ASSERT(i_pEffect, "Effect pointer is NULL: " << dirstr.c_str() << "\\" << m_Name.c_str() );

	int i;
	for (i = 0; i < e_NumTechniques; i++)
	{
		m_Techniques[i] = NULL;
		m_NumTechniquePasses[i] = 0;
	}
	for (i = 0; i < e_NumMatrices; i++)
		m_stdMatrices[i] = NULL;
	
	this->parse_parameters(i_Directory);
	this->parse_techniques();

	m_pDefaults = new effShaderParams;
	this->BuildPrtyObject(m_pDefaults);
	m_pDefaults->SetVersion(FindShaderVersion());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matShaderBaseGL::~matShaderBaseGL()
{
	delete m_pDefaults;

	// this owns the effect!!!
	delete m_pEffect;

	std::list<matTexture*>::iterator it = m_OwnedTextures.begin();
	while (it != m_OwnedTextures.end())
	{
		matTextureMgr::ReleaseTexture(*it);//delete (*it);
		it++;
	}

}

shdrPipeline* matShaderBaseGL::GetEffect() const
{
	if (mContextPipeline)
		return mContextPipeline;
	else 
		return m_pEffect;
}

//--------------------------------------------------------------------
// Start effect, returns number of passes needed
//--------------------------------------------------------------------
int matShaderBaseGL::Begin() const
{
	mContextPipeline = oglContext::currentContext()->getShader(this->m_Name);
	mContextPipeline->Bind(NULL);
//	mCurrentPass = cgGetFirstPass( m_CurrentTechnique );
	return 0;
}

//--------------------------------------------------------------------
// Set up values for this pass
//--------------------------------------------------------------------
void matShaderBaseGL::BeginPass(int i_Pass) const
{
//	cgSetPassState(mCurrentPass);
}

//--------------------------------------------------------------------
// Finish this pass, restoring states
//--------------------------------------------------------------------
void matShaderBaseGL::EndPass() const
{
//	mCurrentPass = cgGetNextPass(mCurrentPass);
}

//--------------------------------------------------------------------
// End effect, call even if number of passes is 0
//--------------------------------------------------------------------
void matShaderBaseGL::End() const
{
	mContextPipeline->Unbind();
}

//--------------------------------------------------------------------
// Set Texture into effect
//--------------------------------------------------------------------
void matShaderBaseGL::SetTexture(int i_param, const matTexture* i_pTexture) const
{
//	ID3D11ShaderResourceView* texture = g3dDX11TextureUtil::GetD3DTexture(i_pTexture);

//	ID3DX11EffectShaderResourceVariable* pVar = m_params[i_param]->AsShaderResource();
//	pVar->SetResource(texture);
}

//--------------------------------------------------------------------
// Start effect, returns number of passes needed
//--------------------------------------------------------------------
void matShaderBaseGL::SetTechnique(Technique i_Technique) const
{
	if (m_Techniques[i_Technique] != NULL)
	{
//		m_CurrentTechnique = m_Techniques[i_Technique];
	}
	else
	{
//		DBG_WARNING2("Specified technique %d not found - using Default: %s", i_Technique, m_Name.c_str());
//		m_CurrentTechnique = m_Techniques[e_Default];
	}
}
void matShaderBaseGL::SetTechnique(const std::string& i_Technique) const
{
//	m_CurrentTechnique = cgGetNamedTechnique( m_pEffect->Effect(), i_Technique.c_str());
}

//--------------------------------------------------------------------
// Get handles to parameters we recognize
//--------------------------------------------------------------------
void matShaderBaseGL::parse_parameters(const fsLocator& i_Directory)
{
	// Look at parameters for semantics and annotations that we know how to interpret

	MapParameter("g_ClipPlane", m_ClipPlaneHandle );
	MapParameter("g_AlphaTestRef", m_AlphaTestRefHandle );
	MapParameter("g_time_0_X", m_TimeHandle);
	MapParameter("g_bProjLt", m_IsProjLtHandle );
	MapParameter("g_uvTransform", m_UVTransformHandle );
	MapParameter("g_bDoubleSided", m_IsDoubleSidedHandle );

	MapParameter("g_FirstLight", m_FirstLightHandle );

	MapParameter("g_bHasDiffuseEnvMap", m_EnvHasDiffuseMapHandle );
	MapParameter("diffuseEnvMap", m_EnvDiffuseMapHandle );
	MapParameter("g_bHasSpecularEnvMap", m_EnvHasSpecularMapHandle );
	MapParameter("specularEnvMap", m_EnvSpecularMapHandle );
	MapParameter("g_diffuseFactor", m_EnvDiffuseFactorHandle );
	MapParameter("g_specularFactor", m_EnvSpecularFactorHandle );
	MapParameter("g_diffuseEnvAngle", m_EnvDiffuseAngleHandle );
	MapParameter("g_specularEnvAngle", m_EnvSpecularAngleHandle );
	MapParameter("g_envDiffuseColor", m_EnvDiffuseColorHandle );
	MapParameter("g_envSpecularColor", m_EnvSpecularColorHandle );

	MapParameter("g_vDofParams", m_DOFHandle );
	MapParameter("g_fDofBlurCutoff", m_DOFBlurCutoffHandle );
	MapParameter("g_bHasMask", m_hHasGlowMask );
	MapParameter("glowMask", m_hGlowMask );
	MapParameter("g_bConstGlow", m_hConstantGlow );
	MapParameter("g_glowSize", m_hGlowSize );

	MapParameter("g_depthScale", m_hOutlineDepthScale );
	MapParameter("g_minAngle", m_hOutlineMinAngle );
	MapParameter("g_maxAngle", m_hOutlineMaxAngle );
	MapParameter("g_thickness", m_hOutlineThickness );
	MapParameter("g_outlineClr", m_hOutlineColor );
	MapParameter("g_ViewportDimensions", m_hOutlineViewSize );
	MapParameter("g_minWidth", m_hOutlineMinWidth );
	MapParameter("g_maxWidth", m_hOutlineMaxWidth );

	MapParameter("g_bake", m_hBake);
	MapParameter("g_bakeTransform", m_hBakingTransform);

	MapParameter("g_IsolateReflection", m_hIsolateReflections);
	MapParameter("g_bCubeMapEnabled", m_hCubeMapEnabled);

	MapParameter("hasHardwareTessellation", m_hHardwareTessellationHandle );
	MapParameter("g_MeshDataTextureWidth", m_hMeshDataTextureWidthHandle );
	MapParameter("g_MeshDataTextureHeight", m_hMeshDataTextureHeightHandle );

	MapParameter("g_DisplacementScale", m_hDisplacementScale );
	MapParameter("g_DisplacementBias", m_hDisplacementBias );
	MapParameter("g_DisplacementBlur", m_hDisplacementBlur );
	MapParameter("g_DisplacementMap", m_hDisplacementMap );
	MapParameter("g_hasDisplacementMap", m_hHasDisplacementMap );
	MapParameter("g_ObjectUVScale", m_hDisplacementObjUVScale );

	MapParameter("normalMap", m_hNormalMap );
	MapParameter("g_bumpMapScale", m_hBumpScale );
	MapParameter("hasNormalMap", m_hHasNormalMap );

	MapParameter("g_vTessellationFactor", m_hTessValueHandle);
//	MapParameter("g_HairTessellationValue", m_HairTessellationHandle );

	MapParameter("g_world", m_stdMatrices[e_ObjToWorld]);
	MapParameter("g_view", m_stdMatrices[e_WorldToView]);
	MapParameter("g_worldIT", m_stdMatrices[e_ObjToWorldIT]);
	MapParameter("g_viewIT", m_stdMatrices[e_WorldToViewIT]);
	MapParameter("g_proj", m_stdMatrices[e_ViewToProj]);
	MapParameter("g_wv", m_stdMatrices[e_ObjToView]);
	MapParameter("g_wvIT", m_stdMatrices[e_ObjToViewIT]);
	MapParameter("g_vp", m_stdMatrices[e_WorldToProj]);
	MapParameter("g_wvp", m_stdMatrices[e_ObjToProj]);

	LPCSTR pstrName = NULL;
	LPCSTR pstrType = NULL;

#if 0
	GLint hParam = cgGetFirstEffectParameter( m_pEffect->Effect() );
	int iParam = 0;
	while( hParam )
	{
		GLintclass paramClass = cgGetParameterClass(hParam);
		CGtype paramType = cgGetParameterType(hParam);

		m_params.push_back(hParam);
		const char* paramName = cgGetParameterName(hParam);
		std::string sparamName(paramName);
		m_paramnamemap[sparamName] = iParam;

		const char* paramSemantic = cgGetParameterSemantic(hParam);
		if ( (paramSemantic != NULL) && 
			((paramType == CG_FLOAT4x4)) )
		{
			if( _strcmpi( paramSemantic, "world" ) == 0 )
				m_stdMatrices[e_ObjToWorld] = hParam;
			else if( _strcmpi( paramSemantic, "view" ) == 0 )
				m_stdMatrices[e_WorldToView] = hParam;
			else if( _strcmpi( paramSemantic, "worldit" ) == 0 )
				m_stdMatrices[e_ObjToWorldIT] = hParam;
			else if( _strcmpi( paramSemantic, "viewit" ) == 0 )
				m_stdMatrices[e_WorldToViewIT] = hParam;
			else if( _strcmpi( paramSemantic, "projection" ) == 0 )
				m_stdMatrices[e_ViewToProj] = hParam;
			else if( _strcmpi( paramSemantic, "worldview" ) == 0 )
				m_stdMatrices[e_ObjToView] = hParam;
			else if( _strcmpi( paramSemantic, "worldviewit" ) == 0 )
				m_stdMatrices[e_ObjToViewIT] = hParam;
			else if( _strcmpi( paramSemantic, "viewprojection" ) == 0 )
				m_stdMatrices[e_WorldToProj] = hParam;
			else if( _strcmpi( paramSemantic, "worldviewprojection" ) == 0 )
				m_stdMatrices[e_ObjToProj] = hParam;
			else if( _strcmpi( paramSemantic, "bones" ) == 0 )
				m_SkinningMatrixPaletteHandle = hParam;
		}
		else if( paramSemantic != NULL && ( paramType == CG_FLOAT4 ))
		{
			if( _strcmpi( paramSemantic, "camerapos" ) == 0 )
				m_VecCameraPosHandle = hParam;
		}
		else if( paramSemantic != NULL && ( paramClass == CG_PARAMETERCLASS_SCALAR ))
		{
			if (paramType == CG_BOOL)
			{
				if( _strcmpi( paramSemantic, "reflectionmapisplanar" ) == 0 )
					m_ReflectionMapIsPlanarHandle = hParam;
				else if( _strcmpi( paramSemantic, "hasreflectionmap" ) == 0 )
					m_HasReflectionMapHandle = hParam;
				else if( _strcmpi( paramSemantic, "hasprojectedtexture" ) == 0 )
					m_HasProjectedTextureHandle = hParam;
				else if( _strcmpi( paramSemantic, "hasshadowmap" ) == 0 )
					m_HasShadowMapHandle = hParam;
				else if( _strcmpi( paramSemantic, "isreflectiongen" ) == 0 )
					m_IsReflectionGenHandle = hParam;
			}
			else if(paramType == CG_INT)
			{
				if( _strcmpi( paramSemantic, "lightarraynum" ) == 0 )
					m_LightArrayNumHandle = hParam;
			}
		}
		else if( paramSemantic != NULL && ( paramClass == CG_PARAMETERCLASS_STRUCT ))
		{
			if( _strcmpi( paramSemantic, "lightarray" ) == 0 )
				m_LightArrayHandle = hParam;
			else if( _strcmpi( paramSemantic, "lightinfo" ) == 0 )
				m_LightInfoHandle = hParam;
			else if( _strcmpi( paramSemantic, "projlightinfo" ) == 0 )
				m_ProjLightInfoHandle = hParam;
		}
		else if( paramClass == CG_PARAMETERCLASS_SAMPLER &&
			(paramType == CG_SAMPLER ||
				 paramType == CG_SAMPLER1D ||
				 paramType == CG_SAMPLER2D ||
				 paramType == CG_SAMPLER3D ||
				 paramType == CG_SAMPLERCUBE ))
		{

			if( paramSemantic && (_strcmpi( paramSemantic, "cubereflectionmap" ) == 0) )
			{
				m_CubeReflectionMapHandle = hParam;
			}
			else if( paramSemantic && (_strcmpi( paramSemantic, "planarreflectionmap" ) == 0) )
			{
				m_PlanarReflectionMapHandle = hParam;
			}
			else if( paramSemantic && (_strcmpi( paramSemantic, "projlighttexture" ) == 0) )
			{
				m_ProjLightTextureHandle = hParam;
			}
			else if( paramSemantic && (_strcmpi( paramSemantic, "projshadowmap" ) == 0) )
			{
				m_ProjShadowMapHandle = hParam;
			}
			else if( paramSemantic && (_strcmpi( paramSemantic, "meshdatamap" ) == 0) )
			{
				m_TessellatorMeshTextureHandle = hParam;
			}
			else
			{
				// if hard coded texture file name then load it and set it
				CGannotation hAnnot = cgGetNamedParameterAnnotation(hParam, "name" );
				if (hAnnot)
				{
					pstrName = cgGetStringAnnotationValue(hAnnot);
					if (pstrName != NULL)
					{
						DBG_LOG("Got effect texture " << std::string(pstrName));

						// Expand out path using directory from effect file
						fsLocator tex_fname = i_Directory;
						tex_fname.Push(pstrName);

						matTexture* pTex = matTextureMgr::LoadTexture(tex_fname);
						// Set texture into effect
//						ID3D11ShaderResourceView* texture = g3dDX11TextureUtil::GetD3DTexture(pTex);
//						hParam->AsShaderResource()->SetResource(texture);
						m_OwnedTextures.push_back(pTex);
					}
				}
			}
		}
		else
		{
		}

		/* do something with param */
		hParam = cgGetNextParameter( hParam );
		iParam = iParam + 1;
	}
#endif
}

//--------------------------------------------------------------------
// Get handles to techniques we recognize
//--------------------------------------------------------------------
void matShaderBaseGL::parse_techniques()
{
#if 0
	int iTech = 0;
	CGtechnique hTechnique = cgGetFirstTechnique(m_pEffect->Effect());
	CGtechnique first = hTechnique;
	while (hTechnique) {
		const char* name = cgGetTechniqueName(hTechnique);

		// Do something with each technique
		if( _strcmpi( name, "default" ) == 0 )
			m_Techniques[e_Default] = hTechnique;
		else if( _strcmpi( name, "singlelight" ) == 0 )
			m_Techniques[e_SingleLight] = hTechnique;
		else if( _strcmpi( name, "projectedlight" ) == 0 )
			m_Techniques[e_ProjectedLight] = hTechnique;
		else if( _strcmpi( name, "projectedlightsupersample" ) == 0 )
			m_Techniques[e_ProjectedLightSuperSample] = hTechnique;
		else if( _strcmpi( name, "projectedlightsupersample2" ) == 0 )
			m_Techniques[e_ProjectedLightSuperSample2] = hTechnique;
		else if( _strcmpi( name, "projectedlightsupersample3" ) == 0 )
			m_Techniques[e_ProjectedLightSuperSample3] = hTechnique;
		else if( _strcmpi( name, "dofprep" ) == 0 )
			m_Techniques[e_DOFPrep] = hTechnique;
		else if( _strcmpi( name, "glow" ) == 0 )
			m_Techniques[e_SpecularGlow] = hTechnique;
		else if( _strcmpi( name, "matte" ) == 0 )
			m_Techniques[e_Matte] = hTechnique;
		else if( _strcmpi( name, "environment" ) == 0 )
			m_Techniques[e_Environment] = hTechnique;

		hTechnique = cgGetNextTechnique(hTechnique);
		iTech = iTech+1;
	}


	// If no default technique by name, get first valid technique
	if (!m_Techniques[e_Default])
		m_Techniques[e_Default] = first;
#endif
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int matShaderBaseGL::GetParamIndex(const std::string& i_name) const 
{
	std::map<std::string, int>::const_iterator found = m_paramnamemap.find(i_name);
	if (found != m_paramnamemap.end())
		return found->second;
	else
	{
//		DBG_ASSERT1(false, "matShaderBaseGL: bad param name %s", i_name.c_str());
		return -1;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::GetAllParamUIs(std::list<matShaderParamUI>& o_paramUI) const
{
	std::map<std::string, int>::const_iterator iter = m_paramnamemap.begin();
	while (iter != m_paramnamemap.end())
	{
		matShaderParamUI ui;
		if (GetParamUI(iter->first, ui))
		{
			o_paramUI.push_back(ui);
		}
		iter++;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool matShaderBaseGL::GetParamUI(const std::string& i_name, matShaderParamUI& o_paramUI) const
{
#if 0

	int index = -1;
	std::map<std::string, int>::const_iterator found = m_paramnamemap.find(i_name);
	if (found != m_paramnamemap.end())
		index = found->second;
	else
	{
//		DBG_ASSERT1(false, "matShaderBaseGL: bad param name %s", i_name.c_str());
		return false;
	}

	o_paramUI.m_name = i_name;
	GLint hParam = m_params[index];
	CGannotation hAnnot = NULL;
	LPCSTR pstr = NULL;
	float fval = 0;
//	D3DXPARAMETER_DESC paramDesc;
	hAnnot = cgGetNamedParameterAnnotation(hParam, "SasUiVisible");
	if (hAnnot)
	{
		pstr = cgGetStringAnnotationValue(hAnnot);
		if (pstr && !_strcmpi(pstr, "false"))
			o_paramUI.m_visible = false;
		else
			o_paramUI.m_visible = true;
	}
	else
	{
		o_paramUI.m_visible = true;
	}
	hAnnot = cgGetNamedParameterAnnotation(hParam, "SasUiDescription");
	if (hAnnot)
	{
		pstr = cgGetStringAnnotationValue(hAnnot);
		o_paramUI.m_desc = pstr;
	}
	else
	{
		o_paramUI.m_desc = i_name;
	}

	hAnnot = cgGetNamedParameterAnnotation(hParam, "SasUiLabel");
	if (hAnnot)
	{
		pstr = cgGetStringAnnotationValue(hAnnot);
		o_paramUI.m_label = pstr;
	}
	else
	{
		o_paramUI.m_label = i_name;
	}
	o_paramUI.m_dataType = matShaderParamUI::e_Unknown;
	hAnnot = cgGetNamedParameterAnnotation(hParam, "SasUiControl");
	if (hAnnot)
	{
		pstr = cgGetStringAnnotationValue(hAnnot);
		if (pstr && !_strcmpi(pstr, "ColorPicker"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_ColorPicker;
			o_paramUI.m_dataType = matShaderParamUI::e_Vector;

			int i = cgGetParameterDefaultValuefr( hParam, 4, o_paramUI.m_fval);
		}
		else if (!_strcmpi(pstr, "Direction"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_Direction;
			o_paramUI.m_dataType = matShaderParamUI::e_Vector;

			int i = cgGetParameterDefaultValuefr( hParam, 4, o_paramUI.m_fval);
		}
		else if (!_strcmpi(pstr, "Numeric"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_Numeric;
			o_paramUI.m_dataType = matShaderParamUI::e_Float;

			int i = cgGetParameterDefaultValuefr( hParam, 1, &(o_paramUI.m_fval[0]));
		}
		else if (!_strcmpi(pstr, "Slider"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_Slider;
			o_paramUI.m_dataType = matShaderParamUI::e_Float;

			int i = cgGetParameterDefaultValuefr( hParam, 1, &(o_paramUI.m_fval[0]));
		}
		else if (!_strcmpi(pstr, "Checkbox"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_Checkbox;
			o_paramUI.m_dataType = matShaderParamUI::e_Bool;

			int i = cgGetParameterDefaultValuefr( hParam, 1, &(o_paramUI.m_fval[0]));
		}
		else if (!_strcmpi(pstr, "FolderPicker"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_FolderPicker;
			o_paramUI.m_dataType = matShaderParamUI::e_String;
			
			// At render time, it is a waste of cycles to assign string typed
			// variables into the effect.  So this GetString must have been Set
			// explicitly outside the render loop.
			LPCSTR pString;
			pString = cgGetStringParameterValue(hParam);
			o_paramUI.m_sval = pString;
		}
		else if (!_strcmpi(pstr, "TexturePicker"))
		{
			// We use a naming convention here.
			// TexturePicker means this is a string var for a texture
			// filename.
			// String vars that begin with "texPath_" are expected to 
			// have a corresponding texture variable.
			
			// In other words, this parameter had better be a string 
			// variable whose name begins with "texPath_" and ends 
			// with the name of a texture variable.

			o_paramUI.m_uiType = matShaderParamUI::e_TexturePicker;
			o_paramUI.m_dataType = matShaderParamUI::e_Texture;

			// At render time, it is a waste of cycles to assign string typed
			// variables into the effect.  So this GetString must have been Set
			// explicitly outside the render loop.
			LPCSTR pString;
			pString = cgGetStringParameterValue(hParam);
			o_paramUI.m_sval = pString;
		}
		else if (!_strcmpi(pstr, "ListPicker"))
			o_paramUI.m_uiType = matShaderParamUI::e_ListPicker;
		else if (!_strcmpi(pstr, "Any"))
			o_paramUI.m_uiType = matShaderParamUI::e_Any;
		else if (!_strcmpi(pstr, "None"))
			o_paramUI.m_uiType = matShaderParamUI::e_None;
	}
	else
	{
		o_paramUI.m_uiType = matShaderParamUI::e_None;
	}
	hAnnot = cgGetNamedParameterAnnotation(hParam, "SasUiMax");
	if (hAnnot)
	{
		int nval= 0;
		const float * f = cgGetFloatAnnotationValues( hAnnot, &nval);
		fval = *f;
		o_paramUI.m_max = fval;
	}
	else
	{
		o_paramUI.m_max = 1;
	}
	hAnnot = cgGetNamedParameterAnnotation(hParam, "SasUiMin");
	if (hAnnot)
	{
		int nval= 0;
		const float * f = cgGetFloatAnnotationValues( hAnnot, &nval);
		fval = *f;
		o_paramUI.m_min = fval;
	}
	else
	{
		o_paramUI.m_min = 0;
	}
	hAnnot = cgGetNamedParameterAnnotation(hParam, "SasUiSteps");
	if (hAnnot)
	{
		int nval= 0;
		const float * f = cgGetFloatAnnotationValues( hAnnot, &nval);
		fval = *f;
		o_paramUI.m_steps = fval;
	}
	else
	{
		o_paramUI.m_steps = 0;
	}
	hAnnot = cgGetNamedParameterAnnotation(hParam, "SasUiPower");
	if (hAnnot)
	{
		int nval= 0;
		const float * f = cgGetFloatAnnotationValues( hAnnot, &nval);
		fval = *f;
		o_paramUI.m_power = fval;
	}
	else
	{
		o_paramUI.m_power = 1;
	}
	hAnnot = cgGetNamedParameterAnnotation(hParam, "SasUiStride");
	if (hAnnot)
	{
		int nval= 0;
		const float * f = cgGetFloatAnnotationValues( hAnnot, &nval);
		fval = *f;
		o_paramUI.m_stride = fval;
	}
	else
	{
		o_paramUI.m_stride = 1;
	}
#endif
	
	return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetMatrix(int i_param, const maMatrix4x4& i_matrix)
{
	GLint pVar = m_params[i_param];
	// check this cast, maybe need to add operator for it.
	glUniformMatrix4fv(pVar, 1, GL_FALSE, i_matrix.Ptr());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetFloat(int i_param, float i_float)
{
	GLint pVar = m_params[i_param];
	glUniform1f(pVar, i_float);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetBool(int i_param, bool i_bool)
{
	GLint pVar = m_params[i_param];
	glUniform1i(pVar, i_bool ? 1 : 0);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetString(int i_param, std::string i_string)
{
//	GLint pVar = m_params[i_param];
//	cgSetStringParameterValue(pVar, i_string.c_str());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetVector(int i_param, const maVector4d& i_vector)
{
	GLint pVar = m_params[i_param];
	glUniform4fv(pVar, 1, i_vector.Ptr());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetData(int i_param, void* i_data, unsigned int i_nbytes)
{
	//m_params[i_param]->SetRawValue(i_data, 0, i_nbytes);
}

//--------------------------------------------------------------------
// Set up standard matrix transforms
//--------------------------------------------------------------------
void matShaderBaseGL::SetupMatrices(const maMatrix4x4 &i_WorldMat,
									 const maMatrix4x4 &i_CameraMat,
									 const maMatrix4x4 &i_ProjMat,
									 const maPoint3d &i_CameraPos) const
{
	// let's set the target resolution here, too. it's sort of related!
//	CGeffect pEffect = m_pEffect->Effect();
//	cgSetParameter4fv(cgGetNamedEffectParameter(pEffect, "g_targetRes"), g3dSceneGlobal::g_TargetRes.Ptr() );
	CHECKGLERROR();
	GLint curHandle;

	GLboolean transpose = GLTRANSPOSE;

	curHandle = m_stdMatrices[e_ObjToWorld];
	if( curHandle > -1)
	{
		maMatrix4x4 mt = i_WorldMat;
		mt.Transpose();
		glUniformMatrix4fv(curHandle, 1, transpose, mt.Ptr());
	}

	curHandle = m_stdMatrices[e_WorldToView];
	if( curHandle > -1)
	{
		maMatrix4x4 mt = i_CameraMat;
		mt.Transpose();
		glUniformMatrix4fv(curHandle, 1, transpose, mt.Ptr());
	}

	curHandle = m_stdMatrices[e_ObjToWorldIT];
	if( curHandle > -1)
	{
		maMatrix4x4 mt = i_WorldMat;
		mt.Transpose();
		glUniformMatrix4fv(curHandle, 1, transpose, mt.Ptr());
	}

	curHandle = m_stdMatrices[e_WorldToViewIT];
	if( curHandle > -1)
	{
		maMatrix4x4 mt = i_CameraMat;
		mt.Transpose();
		glUniformMatrix4fv(curHandle, 1, transpose, mt.Ptr());
	}

	curHandle = m_stdMatrices[e_ViewToProj];
	if( curHandle > -1)
	{
		maMatrix4x4 mt = i_ProjMat;
		mt.Transpose();
		glUniformMatrix4fv(curHandle, 1, transpose, mt.Ptr());
	}

	curHandle = m_stdMatrices[e_ObjToView];
	if( curHandle > -1)
	{
		maMatrix4x4 worldView = i_WorldMat * i_CameraMat;
		worldView.Transpose();
		glUniformMatrix4fv(curHandle, 1, transpose, worldView.Ptr());
	}

	// assuming wvI == wvT therefore wvIT == wv.
	curHandle = m_stdMatrices[e_ObjToViewIT];
	if( curHandle > -1)
	{
		maMatrix4x4 worldView = i_WorldMat * i_CameraMat;
		worldView.Transpose();
		glUniformMatrix4fv(curHandle, 1, transpose, worldView.Ptr());
	}

	curHandle = m_stdMatrices[e_WorldToProj];
	if( curHandle > -1)
	{
		// we could send in camera*proj in function parameters, it should
		// be already computed.
		maMatrix4x4 viewProj = i_CameraMat * i_ProjMat;
		viewProj.Transpose();
		glUniformMatrix4fv(curHandle, 1, transpose, viewProj.Ptr());
	}

	curHandle = m_stdMatrices[e_ObjToProj];
	if( curHandle > -1)
	{
		// we could send in camera*proj in function parameters, it should
		// be already computed.
		maMatrix4x4 worldViewProj = i_WorldMat * i_CameraMat * i_ProjMat;
		worldViewProj.Transpose();
		glUniformMatrix4fv(curHandle, 1, transpose, worldViewProj.Ptr());
	}


	if( m_VecCameraPosHandle > -1)
	{
		float vecPosition[4] = { i_CameraPos.m_X, i_CameraPos.m_Y, i_CameraPos.m_Z, 1.0f };    
		glUniform4fv(m_VecCameraPosHandle, 1, vecPosition);
	}

	CHECKGLERROR();
}

//====================================================================
// Set up skinning matrix transforms
//====================================================================
bool matShaderBaseGL::GetHasSkinning() const
{
	return (m_SkinningMatrixPaletteHandle != -1); 
}
void matShaderBaseGL::SetupSkinningMatrices(const std::vector<maMatrix4x4> &i_MatrixPalette) const
{
	// all this input checking could probably be optimized! 
//	DBG_ASSERT(m_SkinningMatrixPaletteHandle != NULL, "setupskinning on effect that doesn't support skinning");
//	D3DX11_EFFECT_TYPE_DESC desc;
//	m_SkinningMatrixPaletteHandle->GetType()->GetDesc(&desc);
//	DBG_ASSERT(desc.Elements >= i_MatrixPalette.size(), "too many bones in palette");

//	int n = min(desc.Elements, i_MatrixPalette.size());
	// set the data
//	m_SkinningMatrixPaletteHandle->AsMatrix()->SetMatrixArray((float*)&i_MatrixPalette[0], 0, n);
}

//--------------------------------------------------------------------
// Set up shader for lighting and material given the material's 
//	colors
//--------------------------------------------------------------------
void matShaderBaseGL::SetupLighting(const maAxisBox& i_BBox) const
{
	if( m_LightInfoHandle > -1)
	{	
		const g3dLight* pLight = get_active_light();
		LightInfo light_info;
		if (pLight)
		{
			// Put active light into info structure
			get_light_info(pLight, light_info);
		}
//		m_LightInfoHandle->SetRawValue(&light_info, 0, sizeof(light_info));
	}

	SetupAmbientLighting(i_BBox);
}

//--------------------------------------------------------------------
// Set up shader for lighting and material given the material's 
//	colors
//--------------------------------------------------------------------
void matShaderBaseGL::SetupAmbientLighting(const maAxisBox& i_BBox) const
{
	if( m_LightInfoHandle > -1)
	{	
		const g3dLight* pLight = get_head_light();
		LightInfo light_info;
		if (pLight)
		{
			// Put active light into info structure
			get_light_info(pLight, light_info);
		}
//		m_LightInfoHandle->SetRawValue(&light_info, 0, sizeof(light_info));
	}
	SetIsProjLight(false);

}

//--------------------------------------------------------------------
// Set up shader for lighting through a projected texture light.
//--------------------------------------------------------------------
void matShaderBaseGL::SetupProjectedLight(const matTexture* i_pTexture,
									const matTexture* i_pShadowMap,
									const maMatrix4x4& i_TextureMatrix, 
									const maPoint3d& i_Position,
									float i_LightSize,
									float i_PCSSAdjust,
									float i_Scale,
									float i_SceneScale,
									float i_shadowIntensity,
									const maFloatRGBA& i_ShadowColor,
									float i_Near, float i_Far,
									float i_InnerAngle, float i_OuterAngle,
									float i_Aspect) const
{
#if 0
	CGeffect pEffect = m_pEffect->Effect();
	if( m_ProjLightInfoHandle > -1)
	{	
		ProjLightInfo proj_light_info;
		proj_light_info.m_Position.Set(i_Position.m_X, i_Position.m_Y, i_Position.m_Z, 1.0f);
		proj_light_info.m_TextureMatrix = i_TextureMatrix;
		proj_light_info.m_TextureMatrix.Transpose(); // needed?
		proj_light_info.m_LightSize = i_LightSize;
		proj_light_info.m_PCSSAdjust = i_PCSSAdjust;
		proj_light_info.m_Scale = i_Scale;
		proj_light_info.m_shadowIntensity = i_shadowIntensity;
		proj_light_info.m_ShadowColor = i_ShadowColor;
		proj_light_info.m_Near = i_Near;
		proj_light_info.m_Far = i_Far;
		if (i_pShadowMap)
			proj_light_info.m_MapSize = (float)(i_pShadowMap->GetWidth());
		proj_light_info.m_InnerAngle = i_InnerAngle*maConstants::c_fAngleToRad;
		proj_light_info.m_OuterAngle = i_OuterAngle*maConstants::c_fAngleToRad;
		proj_light_info.m_Aspect = i_Aspect;

		// TODO: use a cbuffer here!

//		m_ProjLightInfoHandle->SetRawValue(&proj_light_info, 0, sizeof(proj_light_info));
		
		// from shader:
//	float4 Pos;
//	float4x4 Matrix;
//	float LightSize;
//	float PCSSAdjust;
//	float Scale;
//	float ShadowIntensity;
//	float4 ShadowColor;
//	float2 NearFar;
		cgSetParameter4fv(cgGetNamedStructParameter(m_ProjLightInfoHandle, "Pos"), proj_light_info.m_Position.Ptr());
		cgSetMatrixParameterfr(cgGetNamedStructParameter(m_ProjLightInfoHandle, "Matrix"), proj_light_info.m_TextureMatrix.Ptr());
		cgSetParameter1f(cgGetNamedStructParameter(m_ProjLightInfoHandle, "LightSize"), proj_light_info.m_LightSize);
		cgSetParameter1f(cgGetNamedStructParameter(m_ProjLightInfoHandle, "PCSSAdjust"), proj_light_info.m_PCSSAdjust);
		cgSetParameter1f(cgGetNamedStructParameter(m_ProjLightInfoHandle, "Scale"), proj_light_info.m_Scale);
		cgSetParameter1f(cgGetNamedStructParameter(m_ProjLightInfoHandle, "ShadowIntensity"), proj_light_info.m_shadowIntensity);
		cgSetParameter4fv(cgGetNamedStructParameter(m_ProjLightInfoHandle, "ShadowColor"), proj_light_info.m_ShadowColor.Ptr());
		cgSetParameter4fv(cgGetNamedStructParameter(m_ProjLightInfoHandle, "NearFar"), &proj_light_info.m_Near);	
		cgSetParameter1f(cgGetNamedStructParameter(m_ProjLightInfoHandle, "MapSize"), proj_light_info.m_MapSize);
		cgSetParameter1f(cgGetNamedStructParameter(m_ProjLightInfoHandle, "InnerAngle"), proj_light_info.m_InnerAngle);
		cgSetParameter1f(cgGetNamedStructParameter(m_ProjLightInfoHandle, "OuterAngle"), proj_light_info.m_OuterAngle);
		cgSetParameter1f(cgGetNamedStructParameter(m_ProjLightInfoHandle, "Aspect"), proj_light_info.m_Aspect);
	}
/*
ID3D11ShaderResourceView* projLightTexture = g3dDX11TextureUtil::GetD3DTexture(i_pTexture);
	ID3D11ShaderResourceView* shadowMap = 
		g3dPrefs::CurrentPrefs().m_bEnableShadows ? g3dDX11TextureUtil::GetD3DTexture(i_pShadowMap) : NULL;

	if( m_ProjLightTextureHandle != NULL && m_ProjLightTextureHandle->IsValid())
	{	
		m_ProjLightTextureHandle->AsShaderResource()->SetResource( projLightTexture );
	}
	if( m_HasProjectedTextureHandle != NULL && m_HasProjectedTextureHandle->IsValid())
	{	
		m_HasProjectedTextureHandle->AsScalar()->SetBool( projLightTexture==NULL?FALSE:TRUE );		
	}
	if( m_HasShadowMapHandle != NULL && m_HasShadowMapHandle->IsValid())
	{	
		m_HasShadowMapHandle->AsScalar()->SetBool( shadowMap==NULL?FALSE:TRUE );	
	}
	if ( m_ProjShadowMapHandle != NULL && m_ProjShadowMapHandle->IsValid())
	{
		m_ProjShadowMapHandle->AsShaderResource()->SetResource( shadowMap );

		ID3D11ShaderResourceView* texture = g3dDX11TextureUtil::GetD3DTexture(matTextureMgrDX11::GetPoisson());
		pEffect->GetVariableByName("g_Poisson")->AsShaderResource()->SetResource(texture);
	}
*/
	//	pEffect->SetInt("g_nShadowSamples", i_nShadowSamples);
//	pEffect->SetInt("g_nBlockerSamples", i_nBlockerSamples);
#endif
}

//====================================================================
// Set up the shader for Hardware tessellation using a texture to store extra mesh data.
//====================================================================
void matShaderBaseGL::SetupTessellatorMeshTexture( const matTexture* i_pTexture )
{
	/*
	shdrCgPipeline* pEffect = GetD3DXEffect();
	if( i_pTexture && pEffect )
	{
		if( m_TessellatorMeshTextureHandle != NULL && m_TessellatorMeshTextureHandle->IsValid())
		{
			ID3D11ShaderResourceView* meshTexture = g3dDX11TextureUtil::GetD3DTexture(i_pTexture);
			m_TessellatorMeshTextureHandle->AsShaderResource()->SetResource( meshTexture );
		}
		if( m_hMeshDataTextureWidthHandle && m_hMeshDataTextureWidthHandle->IsValid())
		{
			m_hMeshDataTextureWidthHandle->AsScalar()->SetFloat( (float)i_pTexture->GetWidth() );
		}
		if( m_hMeshDataTextureHeightHandle && m_hMeshDataTextureHeightHandle->IsValid())
		{
			m_hMeshDataTextureHeightHandle->AsScalar()->SetFloat( (float)i_pTexture->GetHeight() );
		}
	}
	*/
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetAlphaTestRef(float i_Value) const
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetClipPlane( const maVector4d& i_Plane ) const
{
	glUniform4fv(m_ClipPlaneHandle, 1, i_Plane.GetPtr());
}

//--------------------------------------------------------------------
// GetImportantLight() gets the first eight important lights by
// given axis box
//--------------------------------------------------------------------
void matShaderBaseGL::SetImportantLight(int i_num, const maAxisBox& i_BBox) const
{
	LightInfo lights_info[8];
	int light_num_max = get_some_lights(lights_info, i_BBox);
	
	int light_num = min(i_num, light_num_max);

}

//--------------------------------------------------------------------
// global function that set a the hair tessellation value
//--------------------------------------------------------------------
/*
void matShaderBaseGL::SetHairTessellationValue( const maVector2d& i_Value ) const
{
//	m_HairTessellationHandle->AsVector()->SetFloatVector( i_Value.Ptr() );
}
*/

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetTime(float i_Time) const
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetVertexUVBakeMode(bool i_bDoBaking) const
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetIsProjLight(bool i_IsProjLight) const
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetupAmbientPass(const g3dAmbientEnvState& i_AmbientEnvState) const
{
	if (g3dPrefs::CurrentPrefs().m_bEnableDiffuseLighting)
	{
	}
	else
	{
	}

	if (g3dPrefs::CurrentPrefs().m_bEnableSpecularLighting)
	{
	}
	else
	{
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetupDOFPrep() const
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex) const
{

	// set up reflection mapping (will null out variables if no refl map):
	const effReflectionMap& pData = i_Material->GetReflectionData();
	SetupReflectionMap(pData.m_bIsPlanar, pData.m_ReflectionMap);

	// set up shader parameters:
	shared_ptr<effShaderParams> p = i_Material->GetShaderParams(i_MaterialLayerIndex);
	if (p)
	{
		//		p->Dump();
		if (!p->m_pShaderBindings)
		{
			BuildPrtyObject(p.get());
		}

		Bind(p->m_pShaderBindings);
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetIsolateReflections(bool i_bIsolateReflections) const
{
//	m_hIsolateReflections->AsScalar()->SetBool( i_bIsolateReflections ? TRUE : FALSE );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetupParams(const effShaderData* i_Data) const
{
	int x = 1;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void matShaderBaseGL::SetupGlowPass(const effGlowData& i_GlowData) const
{
}

void matShaderBaseGL::SetupOutlinePass(const effOutlineData& i_OutlineData) const
{
}


void matShaderBaseGL::SetIsDoubleSided(bool i_IsDoubleSided) const
{
//	m_IsDoubleSidedHandle->AsScalar()->SetBool(i_IsDoubleSided?TRUE:FALSE);
}

//--------------------------------------------------------------------
// Set up shader for lighting and material given the material's 
//	colors
//--------------------------------------------------------------------
void matShaderBaseGL::SetupSingleLight(const g3dLight* i_pLight,
	const g3dProjectedLight* i_pProjLight, bool i_bAllowShadows) const
{
	if( m_LightInfoHandle > -1)
	{	
		LightInfo light_info;
		if (i_pLight)
		{
			// Put active light into info structure
			get_light_info(i_pLight, light_info);
		}
//		shdrCgPipeline* pEffect = GetD3DXEffect();
//		m_LightInfoHandle->SetRawValue(&light_info, 0, sizeof(light_info));
	}
	SetIsProjLight(i_pProjLight != NULL);
	if (i_pProjLight != NULL)
	{
		SetupProjectedLight(i_pProjLight->GetTexture(), 
			i_bAllowShadows ? i_pProjLight->GetShadowMap() : NULL, 
			i_pProjLight->GetTotalMatrix(), 
			i_pProjLight->GetPosition(),
			i_pProjLight->GetLightSize(),
			i_pProjLight->GetPCSSAdjust(),
			i_pProjLight->GetScale(),
			i_pProjLight->GetSceneScale(),
			i_pProjLight->GetShadowIntensity(),
			i_pProjLight->GetShadowColor(),
			i_pProjLight->GetScale(),
			i_pProjLight->GetIsDirectional() ? i_pProjLight->GetRange() : i_pProjLight->GetScale()+i_pProjLight->GetRange(),
			i_pProjLight->GetInnerAngle(), i_pProjLight->GetAngle(),
			i_pProjLight->GetAspect());
	}
//	m_FirstLightHandle->AsScalar()->SetBool( g3dSingleLightRendering::IsFirstLight() );
}

void matShaderBaseGL::MapParameter(std::string i_Name, GLint& o_Handle)
{
	o_Handle = -1;
	o_Handle = glGetUniformLocation(m_pEffect->vs()->GetShader(), i_Name.c_str());
	if (o_Handle < 0) {
		o_Handle = glGetUniformLocation(m_pEffect->ps()->GetShader(), i_Name.c_str());
	}

//	o_Handle = cgGetNamedEffectParameter(m_pEffect->Effect(), i_Name.c_str());
//	DBG_ASSERT(o_Handle->IsValid(), "missing shader variable " << i_Name);
}

//--------------------------------------------------------------------
// set factors that rescale uv space to [0,0]..[1,1] for texture bake.
//--------------------------------------------------------------------
void matShaderBaseGL::SetBakingFactors(const maPoint2d& i_Scale, const maPoint2d& i_Translate) const
{
	float vBakingXForm[4] = {i_Scale.m_X, i_Scale.m_Y, i_Translate.m_X, i_Translate.m_Y};
}

void matShaderBaseGL::Bind(effShaderBindings* i_pBindings) const
{
	i_pBindings->Bind();
}

//--------------------------------------------------------------------
// allocates mem inside shaderparams.
//--------------------------------------------------------------------
int matShaderBaseGL::BuildPrtyObject(effShaderParams* o_pParams) const
{
	// if the default params have their bindings, then reset the shader to its defaults.
	// if the default params are not bound yet, then we must be building them now!
	if (m_pDefaults->m_pShaderBindings != NULL)
	{
		Bind(m_pDefaults->m_pShaderBindings);
	}
	else
	{
		DBG_ASSERT(m_pDefaults == o_pParams, "Bad shader state in BuildPrtyObject");
	}

	int numParamsFound = 0;

	DBG_ASSERT(o_pParams->m_pShaderBindings == NULL, "bindings already set");
	DBG_ASSERT(o_pParams->m_pPrtyUI == NULL, "prtyUI already set");
	matShaderBindingsGL* bindings = new matShaderBindingsGL(this->m_pEffect);
	o_pParams->m_pShaderBindings = bindings;
	o_pParams->m_pPrtyUI = new prtyObject;

	std::string name;
	int index;
	GLint hParam;

	std::list<ShaderParamUIInfo> uiInfos;

	// gather params.
	std::map<std::string, int>::const_iterator iter = m_paramnamemap.begin();
	while (iter != m_paramnamemap.end())
	{
		name = iter->first;
		index = iter->second;
		hParam = m_params[index];

		if (GetShaderParamInfo(hParam, o_pParams, bindings, uiInfos))
			numParamsFound++;

		iter++;
	}

	// now sort the params by their index
	uiInfos.sort();
	// and add them to the prty object!
	std::list<ShaderParamUIInfo>::iterator uiter;
	for (uiter = uiInfos.begin(); uiter != uiInfos.end(); ++uiter)
		o_pParams->m_pPrtyUI->AddProperty( (*uiter).UIInfo );

	// the shader version in m_pDefaults is current.
	// If o_pParams == m_pDefaults, then we will be correcting 
	// this after returning from this func.
	o_pParams->SetVersion(m_pDefaults->GetVersion());
	return numParamsFound;
}
#if 0
//--------------------------------------------------------------------
// bindings will be owned by the effShaderParams
//--------------------------------------------------------------------
void matShaderBaseGL::CreateBindings(effShaderParams* io_Params)
{
	// delete old bindings.
	delete io_Params->m_pShaderBindings;
	io_Params->m_pShaderBindings = NULL;

	int n = io_Params->m_Params.size();
	if (n < 1)
		return;

	matShaderBindingsGL* bindings = new matShaderBindingsGL(this->m_pEffect);
	io_Params->m_pShaderBindings = bindings;

	for (int i = 0; i < n; i++)
	{
		// current param from list
		effShaderParam* p = io_Params->m_Params[i];

		// get a matching effect param handle based on name
		GLint h = m_pEffect->GetParameterByName(NULL, p->m_Name.c_str());
		if (h != NULL)
		{
			// better validation: check types and ensure there is a match.
			//D3DXPARAMETER_DESC desc;
			//m_pEffect->GetParameterDesc(h, &desc);

			// i'll just query the param for its type and then set up the binding.
			effParamFloat* pFloat = dynamic_cast<effParamFloat*>(p);
			if (pFloat)
			{
				bindings->m_BindableParams.push_back(new effFloatBindingDX11(*pFloat, h));
			}
			else
			{
				effParamTexture* pTexture = dynamic_cast<effParamTexture*>(p);
				if (pTexture)
				{
					GLint hExistVar = FindTextureExistVar(h);
					bindings->m_BindableParams.push_back(new effTextureBindingDX11(*pTexture, h, hExistVar));
				}
				else
				{
					effParamColor* pColor = dynamic_cast<effParamColor*>(p);
					if (pColor)
					{
						bindings->m_BindableParams.push_back(new effColorBindingDX11(*pColor, h));
					}
					else
					{
						// unknown type
					}
				}
			}
		}
		else
		{
			// could not find param
			// no binding added.
			DBG_WARNING1("No binding found in shader for variable %s", p->m_Name.c_str());
		}
	}
}
#endif

bool matShaderBaseGL::GetShaderParamInfo(GLint i_hParam, 
										  effShaderParams* o_pParams,
										  matShaderBindingsGL* o_pBindings,
										  std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	return false;
}

effParamTexture* matShaderBaseGL::MapTextureParam(effShaderParams* o_pParams,
												   matShaderBindingsGL* o_pBindings,
												   GLint i_hParam, 
												   const std::string& i_Name,
												   bool i_bCreateBinding,
												   bool i_bCreateUI,
												   std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	return NULL;
}

effParamFloat* matShaderBaseGL::MapFloatParam(effShaderParams* o_pParams,
											   matShaderBindingsGL* o_pBindings,
											   GLint i_hParam, 
											   const std::string& i_Name,
											   bool i_bCreateBinding,
											   bool i_bCreateUI,
											   std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	return NULL;
}

effParamBool* matShaderBaseGL::MapBoolParam(effShaderParams* o_pParams,
	matShaderBindingsGL* o_pBindings,
	GLint i_hParam, 
	const std::string& i_Name,
	bool i_bCreateBinding,
	bool i_bCreateUI,
	std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	return NULL;
}

void split(std::string & text, std::string & separators, std::vector<std::string> & words)
{
	int n = text.length();
	int start, stop;
	start = text.find_first_not_of(separators);
	while ((start >= 0) && (start < n))
	{
		stop = text.find_first_of(separators, start);
		if ((stop < 0) || (stop > n)) 
			stop = n;
		words.push_back(text.substr(start, stop - start));
		start = text.find_first_not_of(separators, stop+1);
	}
}


effParamInt* matShaderBaseGL::MapEnumParam(effShaderParams* o_pParams,
	matShaderBindingsGL* o_pBindings,
	GLint i_hParam, 
	const std::string& i_Name,
	bool i_bCreateBinding,
	bool i_bCreateUI,
	std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	return NULL;
}

effParamColor* matShaderBaseGL::MapColorParam(effShaderParams* o_pParams,
	matShaderBindingsGL* o_pBindings,
	GLint i_hParam, 
	const std::string& i_Name,
	bool i_bCreateBinding,
	bool i_bCreateUI,
	std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	return NULL;
}

void matShaderBaseGL::SetupReflectionMap(bool i_bIsPlanar, matTexture* i_pReflectionMap) const
{
	if (g3dSingleLightRendering::GetDoReflectionGen() ||
		!g3dPrefs::CurrentPrefs().m_bEnableReflection ||
		g3dPassBuffers::GetDoingFileRefl() 
		) 
	{
		// during reflection generation, don't try to assign the map!
	}
	else
	{
	}
}

//--------------------------------------------------------------------
// does the shader hook into our dynamic reflection mapping?
//--------------------------------------------------------------------
bool matShaderBaseGL::HasReflectionMap() const
{
	return false;
}

//------------------------------------------------------------------------
// function that will initialize the paint overlay texture for the shader
//------------------------------------------------------------------------
void  matShaderBaseGL::SetupPaintOverlay(matRenderTargetTexture* i_pRenderTexture) const
{
	/*
	g2dD3D11BaseTexturePtr texture = g3dDX11TextureUtil::GetD3DTexture(i_pRenderTexture);
 
	if(texture != NULL)
		m_pEffect->SetTexture(m_PaintOverlayMapHandle, texture);

	*/

//	ID3D11ShaderResourceView* texture = g3dDX11TextureUtil::GetD3DTexture(i_pRenderTexture);
//	m_PaintOverlayMapHandle->AsShaderResource()->SetResource(texture);

}

bool matShaderBaseGL::HasHardwareTessellation() const
{
	return false;
}

int matShaderBaseGL::FindShaderVersion()
{
	// get the shader revision param.
	return 0;
}

//--------------------------------------------------------------------
// return null if not found by name. otherwise add to o_params
//--------------------------------------------------------------------
effParamTexture* matShaderBaseGL::MapTextureParam(effShaderParams* o_pParams,
	const std::string& i_Name) const
{
	return NULL;
}

//====================================================================
// Set up the shader for displacement mapping parameters
//====================================================================
void matShaderBaseGL::SetupDisplacementMap( const matTexture* i_pTexture, float i_Scale, float i_Bias, float i_Blur, const maVector2d& i_ObjUVScale )
{
}

bool matShaderBaseGL::HasDisplacementMap() const
{
	if( m_hDisplacementMap )
	{
	}
	return false;
}

void matShaderBaseGL::SetupUVTransform( const maMatrix4x4 &i_WorldMat )
{
	glUniformMatrix4fv(m_UVTransformHandle, 1, GLTRANSPOSE, i_WorldMat.Ptr());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void matShaderBaseGL::SetTessellateValue( float i_Value )
{
	//if (m_hTessValueHandle)
	//{
	//	float v[4] = {i_Value, i_Value, g3dPrefs::CurrentPrefs().m_PixelSubdivLimit,0};
	//	m_hTessValueHandle->AsVector()->SetFloatVector(v);
	//}
}

void matShaderBaseGL::SetupNormalMap( const matTexture* i_pNormalMap, float i_BumpScale )
{
	//ID3D11ShaderResourceView* normalMap = g3dDX11TextureUtil::GetD3DTexture(i_pNormalMap);

	//if( m_hNormalMap )
	//{
	//	m_hNormalMap->AsShaderResource()->SetResource(normalMap);
	//}

	//if ( m_hBumpScale )
	//{
	//	m_hBumpScale->AsScalar()->SetFloat(i_BumpScale);
	//}

	//if( m_hHasNormalMap )
	//{	
	//	m_hHasNormalMap->AsScalar()->SetBool( normalMap==NULL?FALSE:TRUE );		
	//}

}

//====================================================================
// Check if the shader supports outline
//====================================================================
bool matShaderBaseGL::GetSupportOutline() const
{
	bool bValue = false;
	return bValue;
}
