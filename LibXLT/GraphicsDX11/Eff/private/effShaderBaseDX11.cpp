/*****************************************************************************
**  effShaderBaseDX11.cpp
**
**      effShaderBaseDX11 is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"

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
#include "GraphicsDX11/eff/effShaderParamsDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matTextureMgrDX11.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
//#include "Graphics/Mat/matTexture.hpp"

#include <algorithm>
#include <string>


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
		const std::vector<g3dLight*>& light_list = g3dLightMgrDX11::Implementation()->GetLights();

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
		g3dLightMgrDX11::Implementation()->GetEnabledLights(light_list);
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
		g3dLightMgrDX11::Implementation()->GetEnabledLights(light_list);

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

	void GetUIStrings(ID3DX11Effect* i_pEffect, ID3DX11EffectVariable* i_hParam, const std::string& i_Name, 
										std::string& o_Category, std::string& o_Label, std::string& o_Desc)
	{
		LPCSTR pstr = NULL;
		ID3DX11EffectVariable* hAnnot = NULL;

		// the label for the control
		o_Label = i_Name;
		hAnnot = i_hParam->GetAnnotationByName("SasUiLabel");
		if (hAnnot)
		{
			hAnnot->AsString()->GetString(&pstr);
			o_Label = pstr;
		}

		// the category header that the control will go under (see prtyUIInfo)
		o_Category = o_Label;
		hAnnot = i_hParam->GetAnnotationByName("UiCategory");
		if (hAnnot)
		{
			hAnnot->AsString()->GetString(&pstr);
			o_Category = pstr;
		}

		// the description string for the ui control
		o_Desc = o_Label;
		hAnnot = i_hParam->GetAnnotationByName("SasUiDescription");
		if (hAnnot)
		{
			hAnnot->AsString()->GetString(&pstr);
			o_Desc = pstr;
		}
	}

	ID3DX11EffectScalarVariable* FindTextureExistVar(ID3DX11Effect* i_pEffect, ID3DX11EffectVariable* i_hTextureVar)
	{
		// conditional texture existence (null texture) flag
		ID3DX11EffectScalarVariable* hRetVal = NULL;
		ID3DX11EffectVariable* hExistVarAnnot = i_hTextureVar->GetAnnotationByName("ExistVar");
		if (hExistVarAnnot)
		{
			LPCSTR existVarName = NULL;
			hExistVarAnnot->AsString()->GetString(&existVarName);
			ID3DX11EffectVariable* hExistVar = NULL;
			hExistVar = i_pEffect->GetVariableByName(existVarName);
			if (hExistVar)
			{
				hRetVal = hExistVar->AsScalar();
				if (hRetVal != NULL)
				{
					// ensure that the var name points to a boolean.
					D3DX11_EFFECT_TYPE_DESC existVarDesc;
					ID3DX11EffectType* evType = hRetVal->GetType();
					evType->GetDesc(&existVarDesc);
					if (existVarDesc.Type != D3D10_SVT_BOOL)
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
	bool get_resource_name(ID3DX11EffectVariable* i_hParam, fsLocator &o_ResourceName)
	{
		// might also want to search for "resourceName" here (MetaSL backend does that)
		ID3DX11EffectVariable* hAnnot = i_hParam->GetAnnotationByName( "name" );
		if (hAnnot && hAnnot->IsValid())
		{
			LPCSTR pstrName = NULL;
			hAnnot->AsString()->GetString( &pstrName );
			if (pstrName != NULL)
			{
				o_ResourceName.Clear();
				fsFileUtil::ANSIFilenameToLocator(pstrName, o_ResourceName);
				return true;
			}
		}
		return false;
	}
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
effShaderBaseDX11::effShaderBaseDX11(const fsLocator& i_Directory, 
								   ID3DX11Effect* i_pEffect,
                                   std::string i_Name)
:	m_pEffect(NULL),
	m_VecCameraPosHandle(NULL),
	m_FirstLightHandle(NULL),
	m_LightInfoHandle(NULL), 
	m_LightArrayHandle(NULL),
	m_LightArrayNumHandle(NULL),
	m_ProjLightInfoHandle(NULL), m_ProjLightTextureHandle(NULL),
	m_ProjShadowMapHandle(NULL), m_HasProjectedTextureHandle(NULL),
	m_HasShadowMapHandle(NULL),
	m_TessellatorMeshTextureHandle(NULL),
	m_hMeshDataTextureWidthHandle(NULL),
	m_hMeshDataTextureHeightHandle(NULL),
	m_Name(""),
	m_TimeHandle(NULL),
	m_IsProjLtHandle(NULL),
	m_UVTransformHandle(NULL),
	m_EnvHasDiffuseMapHandle(NULL),
	m_EnvDiffuseMapHandle(NULL),
	m_EnvDiffuseAngleHandle(NULL),
	m_EnvDiffuseFactorHandle(NULL),
	m_EnvDiffuseColorHandle(NULL),
	m_EnvHasSpecularMapHandle(NULL),
	m_EnvSpecularMapHandle(NULL),
	m_EnvSpecularAngleHandle(NULL),
	m_EnvSpecularFactorHandle(NULL),
	m_EnvSpecularColorHandle(NULL),
	m_DOFHandle(NULL),
	m_DOFBlurCutoffHandle(NULL),
	m_IsDoubleSidedHandle(NULL),
	m_hBake(NULL),
	m_hIsolateReflections(NULL),
	m_hCubeMapEnabled(NULL),
	m_hBakingTransform(NULL),
	m_ReflectionMapIsPlanarHandle(NULL),
	m_HasReflectionMapHandle(NULL),
	m_CubeReflectionMapHandle(NULL),
	m_PlanarReflectionMapHandle(NULL),
	m_IsReflectionGenHandle(NULL),
	m_pDefaults(NULL),
	m_SkinningMatrixPaletteHandle(NULL),
	m_hHardwareTessellationHandle(NULL),
	m_hHasDisplacementMap(NULL),
	m_hDisplacementMap(NULL),
	m_hDisplacementScale(NULL),
	m_hDisplacementBias(NULL),
	m_hDisplacementBlur(NULL),
	m_hDisplacementObjUVScale(NULL),
	m_hNormalMap(NULL),
	m_hBumpScale(NULL),
	m_hHasNormalMap(NULL),
	m_hTessValueHandle(NULL),
	m_CurrentTechnique(NULL),
	m_AlphaTestRefHandle(NULL),
	m_ClipPlaneHandle(NULL)
//	m_HairTessellationHandle(NULL)
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
	
	m_pEffect->GetDesc( &m_EffectDesc );

	this->parse_parameters(i_Directory);
	this->parse_techniques();

	m_pDefaults = new effShaderParams;
	this->BuildPrtyObject(m_pDefaults);
	m_pDefaults->SetVersion(FindShaderVersion());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
effShaderBaseDX11::~effShaderBaseDX11()
{
	delete m_pDefaults;

	m_pEffect->Release();
	std::list<matTexture*>::iterator it = m_OwnedTextures.begin();
	while (it != m_OwnedTextures.end())
	{
		matTextureMgr::ReleaseTexture(*it);//delete (*it);
		it++;
	}

}


//--------------------------------------------------------------------
// Start effect, returns number of passes needed
//--------------------------------------------------------------------
int effShaderBaseDX11::Begin() const
{
	static D3DX11_TECHNIQUE_DESC techDesc;

	HRESULT op_result = m_CurrentTechnique->GetDesc( &techDesc );
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError( op_result );
		DBG_ASSERT( SUCCEEDED(op_result), "Couldn't get pass count! : " << m_Name );
	}

	return techDesc.Passes;
}

//--------------------------------------------------------------------
// Set up values for this pass
//--------------------------------------------------------------------
void effShaderBaseDX11::BeginPass(int i_Pass) const
{
	ID3DX11EffectPass* pass = m_CurrentTechnique->GetPassByIndex(i_Pass);
	HRESULT op_result = pass->Apply(0, g2dDX11Global::g_pDeviceContext);
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError( op_result );
		DBG_ASSERT( SUCCEEDED(op_result), "Couldn't set up pass " << i_Pass << ": " << m_Name );
	}
}

//--------------------------------------------------------------------
// Finish this pass, restoring states
//--------------------------------------------------------------------
void effShaderBaseDX11::EndPass() const
{
}

//--------------------------------------------------------------------
// End effect, call even if number of passes is 0
//--------------------------------------------------------------------
void effShaderBaseDX11::End() const
{
}

//--------------------------------------------------------------------
// Set Texture into effect
//--------------------------------------------------------------------
void effShaderBaseDX11::SetTexture(int i_param, const matTexture* i_pTexture) const
{
	ID3D11ShaderResourceView* texture = g3dDX11TextureUtil::GetD3DTexture(i_pTexture);

	ID3DX11EffectShaderResourceVariable* pVar = m_params[i_param]->AsShaderResource();
	pVar->SetResource(texture);
}

//--------------------------------------------------------------------
// Start effect, returns number of passes needed
//--------------------------------------------------------------------
void effShaderBaseDX11::SetTechnique(Technique i_Technique) const
{
	if (m_Techniques[i_Technique] != NULL)
	{
		m_CurrentTechnique = m_Techniques[i_Technique];
	}
	else
	{
//		DBG_WARNING2("Specified technique %d not found - using Default: %s", i_Technique, m_Name.c_str());
		m_CurrentTechnique = m_Techniques[e_Default];
	}
}
void effShaderBaseDX11::SetTechnique(const std::string& i_Technique) const
{
	m_CurrentTechnique = m_pEffect->GetTechniqueByName(i_Technique.c_str());
}

//--------------------------------------------------------------------
// Get handles to parameters we recognize
//--------------------------------------------------------------------
void effShaderBaseDX11::parse_parameters(const fsLocator& i_Directory)
{
    // Look at parameters for semantics and annotations that we know how to interpret
    D3DX11_EFFECT_VARIABLE_DESC ParamDesc;
    //D3DXPARAMETER_DESC AnnotDesc;
    ID3DX11EffectVariable* hParam;
    ID3DX11EffectType* pType;
	D3DX11_EFFECT_TYPE_DESC typeDesc;

    LPCSTR pstrName = NULL;
	LPCSTR pstrType = NULL;

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

	m_params.resize(m_EffectDesc.GlobalVariables);	
	for( UINT iParam = 0; iParam < m_EffectDesc.GlobalVariables; iParam++ )
    {
        hParam = m_pEffect->GetVariableByIndex( iParam );
        hParam->GetDesc( &ParamDesc );
		pType = hParam->GetType();
		pType->GetDesc(&typeDesc);

		m_params[iParam] = hParam;
		m_paramnamemap[ParamDesc.Name] = iParam;

		if ( (ParamDesc.Semantic != NULL) && 
			((typeDesc.Class == D3D10_SVC_MATRIX_ROWS) || (typeDesc.Class == D3D10_SVC_MATRIX_COLUMNS)) )
		{
            if( _strcmpi( ParamDesc.Semantic, "world" ) == 0 )
				m_stdMatrices[e_ObjToWorld] = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "view" ) == 0 )
                m_stdMatrices[e_WorldToView] = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "worldit" ) == 0 )
                m_stdMatrices[e_ObjToWorldIT] = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "viewit" ) == 0 )
                m_stdMatrices[e_WorldToViewIT] = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "projection" ) == 0 )
                m_stdMatrices[e_ViewToProj] = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "worldview" ) == 0 )
                m_stdMatrices[e_ObjToView] = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "worldviewit" ) == 0 )
                m_stdMatrices[e_ObjToViewIT] = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "viewprojection" ) == 0 )
                m_stdMatrices[e_WorldToProj] = hParam;
            else if( _strcmpi( ParamDesc.Semantic, "worldviewprojection" ) == 0 )
                m_stdMatrices[e_ObjToProj] = hParam;
			else if( _strcmpi( ParamDesc.Semantic, "bones" ) == 0 )
				m_SkinningMatrixPaletteHandle = hParam;
        }
		else if( ParamDesc.Semantic != NULL && ( typeDesc.Class == D3D10_SVC_VECTOR ))
        {
            if( _strcmpi( ParamDesc.Semantic, "camerapos" ) == 0 )
                m_VecCameraPosHandle = hParam;
        }
		else if( ParamDesc.Semantic != NULL && ( typeDesc.Class == D3D10_SVC_SCALAR ))
        {
			if (typeDesc.Type == D3D10_SVT_BOOL)
			{
				if( _strcmpi( ParamDesc.Semantic, "reflectionmapisplanar" ) == 0 )
					m_ReflectionMapIsPlanarHandle = hParam;
				else if( _strcmpi( ParamDesc.Semantic, "hasreflectionmap" ) == 0 )
					m_HasReflectionMapHandle = hParam;
				else if( _strcmpi( ParamDesc.Semantic, "hasprojectedtexture" ) == 0 )
					m_HasProjectedTextureHandle = hParam;
				else if( _strcmpi( ParamDesc.Semantic, "hasshadowmap" ) == 0 )
					m_HasShadowMapHandle = hParam;
				else if( _strcmpi( ParamDesc.Semantic, "isreflectiongen" ) == 0 )
					m_IsReflectionGenHandle = hParam;
			}
			else if(typeDesc.Type == D3D10_SVT_INT)
			{
				if( _strcmpi( ParamDesc.Semantic, "lightarraynum" ) == 0 )
					m_LightArrayNumHandle = hParam;
			}
		}
		else if( ParamDesc.Semantic != NULL && ( typeDesc.Class == D3D10_SVC_STRUCT ))
        {
            if( _strcmpi( ParamDesc.Semantic, "lightarray" ) == 0 )
                m_LightArrayHandle = hParam;
			else if( _strcmpi( ParamDesc.Semantic, "lightinfo" ) == 0 )
                m_LightInfoHandle = hParam;
			else if( _strcmpi( ParamDesc.Semantic, "projlightinfo" ) == 0 )
                m_ProjLightInfoHandle = hParam;
		}
		else if( typeDesc.Class == D3D10_SVC_OBJECT &&
			    (typeDesc.Type == D3D10_SVT_TEXTURE ||
				 typeDesc.Type == D3D10_SVT_TEXTURE1D ||
				 typeDesc.Type == D3D10_SVT_TEXTURE2D ||
				 typeDesc.Type == D3D10_SVT_TEXTURE3D ||
				 typeDesc.Type == D3D10_SVT_TEXTURECUBE ))
		{

			if( ParamDesc.Semantic && (_strcmpi( ParamDesc.Semantic, "cubereflectionmap" ) == 0) )
			{
				m_CubeReflectionMapHandle = hParam;
			}
			else if( ParamDesc.Semantic && (_strcmpi( ParamDesc.Semantic, "planarreflectionmap" ) == 0) )
			{
				m_PlanarReflectionMapHandle = hParam;
			}
			else if( ParamDesc.Semantic && (_strcmpi( ParamDesc.Semantic, "projlighttexture" ) == 0) )
			{
				m_ProjLightTextureHandle = hParam;
			}
			else if( ParamDesc.Semantic && (_strcmpi( ParamDesc.Semantic, "projshadowmap" ) == 0) )
			{
				m_ProjShadowMapHandle = hParam;
			}
			else if( ParamDesc.Semantic && (_strcmpi( ParamDesc.Semantic, "meshdatamap" ) == 0) )
			{
				m_TessellatorMeshTextureHandle = hParam;
			}
			else
			{
				// if hard coded texture file name then load it and set it
				ID3DX11EffectVariable* hAnnot = hParam->GetAnnotationByName( "name" );
				if (hAnnot && hAnnot->IsValid())
				{
					hAnnot->AsString()->GetString( &pstrName );
					if (pstrName != NULL)
					{
						DBG_LOG("Got effect texture " << std::string(pstrName));

						// Expand out path using directory from effect file
						fsLocator tex_fname = i_Directory;
						tex_fname.Push(pstrName);

		//				std::string filename;
		//				fsFileUtil::LocatorToANSIFilename( tex_fname, filename );

						matTexture* pTex = matTextureMgr::LoadTexture(tex_fname);
						// Set texture into effect
						ID3D11ShaderResourceView* texture = g3dDX11TextureUtil::GetD3DTexture(pTex);
						hParam->AsShaderResource()->SetResource(texture);
						m_OwnedTextures.push_back(pTex);
					}
				}
			}
		}
		else
		{
		}
	}
}

//--------------------------------------------------------------------
// Get handles to techniques we recognize
//--------------------------------------------------------------------
void effShaderBaseDX11::parse_techniques()
{
    ID3DX11EffectTechnique* hTechnique;
    D3DX11_TECHNIQUE_DESC TechniqueDesc;

	// Get techniques based on name
    for( UINT iTech = 0; iTech < m_EffectDesc.Techniques; iTech++ )
    {
        hTechnique = m_pEffect->GetTechniqueByIndex(iTech);
		hTechnique->GetDesc(&TechniqueDesc);

        if( _strcmpi( TechniqueDesc.Name, "default" ) == 0 )
			m_Techniques[e_Default] = hTechnique;
		else if( _strcmpi( TechniqueDesc.Name, "singlelight" ) == 0 )
			m_Techniques[e_SingleLight] = hTechnique;
		else if( _strcmpi( TechniqueDesc.Name, "projectedlight" ) == 0 )
			m_Techniques[e_ProjectedLight] = hTechnique;
		else if( _strcmpi( TechniqueDesc.Name, "projectedlightsupersample" ) == 0 )
			m_Techniques[e_ProjectedLightSuperSample] = hTechnique;
		else if( _strcmpi( TechniqueDesc.Name, "projectedlightsupersample2" ) == 0 )
			m_Techniques[e_ProjectedLightSuperSample2] = hTechnique;
		else if( _strcmpi( TechniqueDesc.Name, "projectedlightsupersample3" ) == 0 )
			m_Techniques[e_ProjectedLightSuperSample3] = hTechnique;
		else if( _strcmpi( TechniqueDesc.Name, "dofprep" ) == 0 )
			m_Techniques[e_DOFPrep] = hTechnique;
		else if( _strcmpi( TechniqueDesc.Name, "glow" ) == 0 )
			m_Techniques[e_SpecularGlow] = hTechnique;
		else if( _strcmpi( TechniqueDesc.Name, "matte" ) == 0 )
			m_Techniques[e_Matte] = hTechnique;
		else if( _strcmpi( TechniqueDesc.Name, "environment" ) == 0 )
			m_Techniques[e_Environment] = hTechnique;
    }

	// If no default technique by name, get first valid technique
	if (!m_Techniques[e_Default])
        m_Techniques[e_Default] = m_pEffect->GetTechniqueByIndex(0);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int effShaderBaseDX11::GetParamIndex(const std::string& i_name) const 
{
	std::map<std::string, int>::const_iterator found = m_paramnamemap.find(i_name);
    if (found != m_paramnamemap.end())
		return found->second;
	else
	{
//		DBG_ASSERT1(false, "effShaderBaseDX11: bad param name %s", i_name.c_str());
		return -1;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::GetAllParamUIs(std::list<matShaderParamUI>& o_paramUI) const
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
bool effShaderBaseDX11::GetParamUI(const std::string& i_name, matShaderParamUI& o_paramUI) const
{

	int index = -1;
	std::map<std::string, int>::const_iterator found = m_paramnamemap.find(i_name);
    if (found != m_paramnamemap.end())
		index = found->second;
	else
	{
//		DBG_ASSERT1(false, "effShaderBaseDX11: bad param name %s", i_name.c_str());
		return false;
	}

	o_paramUI.m_name = i_name;
	ID3DX11EffectVariable* hParam = m_params[index];
	ID3DX11EffectVariable* hAnnot = NULL;
    LPCSTR pstr = NULL;
	float fval = 0;
//	D3DXPARAMETER_DESC paramDesc;
//	m_pEffect->GetParameterDesc(hParam, &paramDesc);
	hAnnot = hParam->GetAnnotationByName("SasUiVisible");
	if (hAnnot)
	{
		hAnnot->AsString()->GetString(&pstr);
		if (pstr && !_strcmpi(pstr, "false"))
			o_paramUI.m_visible = false;
		else
			o_paramUI.m_visible = true;
	}
	else
	{
		o_paramUI.m_visible = true;
	}
	hAnnot = hParam->GetAnnotationByName("SasUiDescription");
	if (hAnnot)
	{
		hAnnot->AsString()->GetString(&pstr);
		o_paramUI.m_desc = pstr;
	}
	else
	{
		o_paramUI.m_desc = i_name;
	}

	hAnnot = hParam->GetAnnotationByName("SasUiLabel");
	if (hAnnot)
	{
		hAnnot->AsString()->GetString(&pstr);
		o_paramUI.m_label = pstr;
	}
	else
	{
		o_paramUI.m_label = i_name;
	}
	o_paramUI.m_dataType = matShaderParamUI::e_Unknown;
	hAnnot = hParam->GetAnnotationByName("SasUiControl");
	if (hAnnot)
	{
		hAnnot->AsString()->GetString(&pstr);
		if (pstr && !_strcmpi(pstr, "ColorPicker"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_ColorPicker;
			o_paramUI.m_dataType = matShaderParamUI::e_Vector;

			hParam->AsVector()->GetFloatVector(o_paramUI.m_fval);
		}
		else if (!_strcmpi(pstr, "Direction"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_Direction;
			o_paramUI.m_dataType = matShaderParamUI::e_Vector;

			hParam->AsVector()->GetFloatVector(o_paramUI.m_fval);
		}
		else if (!_strcmpi(pstr, "Numeric"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_Numeric;
			o_paramUI.m_dataType = matShaderParamUI::e_Float;

			hParam->AsScalar()->GetFloat(&(o_paramUI.m_fval[0]));
		}
		else if (!_strcmpi(pstr, "Slider"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_Slider;
			o_paramUI.m_dataType = matShaderParamUI::e_Float;

			hParam->AsScalar()->GetFloat(&(o_paramUI.m_fval[0]));
		}
		else if (!_strcmpi(pstr, "Checkbox"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_Checkbox;
			o_paramUI.m_dataType = matShaderParamUI::e_Bool;

			bool bval;
			hParam->AsScalar()->GetBool(&bval);
			o_paramUI.m_fval[0] = bval?1.0f:0.0f;
		}
		else if (!_strcmpi(pstr, "FolderPicker"))
		{
			o_paramUI.m_uiType = matShaderParamUI::e_FolderPicker;
			o_paramUI.m_dataType = matShaderParamUI::e_String;

			// At render time, it is a waste of cycles to assign string typed
			// variables into the effect.  So this GetString must have been Set
			// explicitly outside the render loop.
			LPCSTR pString;
			hParam->AsString()->GetString(&pString);
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
			hParam->AsString()->GetString(&pString);
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
	hAnnot = hParam->GetAnnotationByName("SasUiMax");
	if (hAnnot)
	{
		hAnnot->AsScalar()->GetFloat(&fval);
		o_paramUI.m_max = fval;
	}
	else
	{
		o_paramUI.m_max = 1;
	}
	hAnnot = hParam->GetAnnotationByName("SasUiMin");
	if (hAnnot)
	{
		hAnnot->AsScalar()->GetFloat(&fval);
		o_paramUI.m_min = fval;
	}
	else
	{
		o_paramUI.m_min = 0;
	}
	hAnnot = hParam->GetAnnotationByName("SasUiSteps");
	if (hAnnot)
	{
		hAnnot->AsScalar()->GetFloat( &fval);
		o_paramUI.m_steps = fval;
	}
	else
	{
		o_paramUI.m_steps = 0;
	}
	hAnnot = hParam->GetAnnotationByName("SasUiPower");
	if (hAnnot)
	{
		hAnnot->AsScalar()->GetFloat( &fval);
		o_paramUI.m_power = fval;
	}
	else
	{
		o_paramUI.m_power = 1;
	}
	hAnnot = hParam->GetAnnotationByName("SasUiStride");
	if (hAnnot)
	{
		hAnnot->AsScalar()->GetFloat( &fval);
		o_paramUI.m_stride = fval;
	}
	else
	{
		o_paramUI.m_stride = 1;
	}
	
	return true;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetMatrix(int i_param, const maMatrix4x4& i_matrix)
{
	ID3DX11EffectMatrixVariable* pVar = m_params[i_param]->AsMatrix();
	// check this cast, maybe need to add operator for it.
	pVar->SetMatrix(i_matrix.Ptr());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetFloat(int i_param, float i_float)
{
	ID3DX11EffectScalarVariable* pVar = m_params[i_param]->AsScalar();
	pVar->SetFloat(i_float);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetBool(int i_param, bool i_bool)
{
	ID3DX11EffectScalarVariable* pVar = m_params[i_param]->AsScalar();
	pVar->SetBool(i_bool);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetString(int i_param, std::string i_string)
{
	ID3DX11EffectStringVariable* pVar = m_params[i_param]->AsString();
	pVar->SetRawValue((void*)i_string.c_str(), 0, i_string.length());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetVector(int i_param, const maVector4d& i_vector)
{
	ID3DX11EffectVectorVariable* pVar = m_params[i_param]->AsVector();
	pVar->SetFloatVector(i_vector.Ptr());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetData(int i_param, void* i_data, unsigned int i_nbytes)
{
	m_params[i_param]->SetRawValue(i_data, 0, i_nbytes);
}

//--------------------------------------------------------------------
// Set up standard matrix transforms
//--------------------------------------------------------------------
void effShaderBaseDX11::SetupMatrices(const maMatrix4x4 &i_WorldMat,
									 const maMatrix4x4 &i_CameraMat,
									 const maMatrix4x4 &i_ProjMat,
									 const maPoint3d &i_CameraPos) const
{
	// let's set the target resolution here, too. it's sort of related!
	ID3DX11Effect* pEffect = GetD3DXEffect();
	pEffect->GetVariableByName("g_targetRes")->AsVector()->SetFloatVector( g3dSceneGlobal::g_TargetRes.Ptr() );

	ID3DX11EffectVariable* curHandle;

	curHandle = m_stdMatrices[e_ObjToWorld];
	if( curHandle != NULL && curHandle->IsValid() )
	{
		maMatrix4x4 mt = i_WorldMat;
		mt.Transpose();
		curHandle->AsMatrix()->SetMatrix(mt.Ptr() );
	}

	curHandle = m_stdMatrices[e_WorldToView];
    if( curHandle != NULL && curHandle->IsValid() )
	{
		maMatrix4x4 mt = i_CameraMat;
		mt.Transpose();
		curHandle->AsMatrix()->SetMatrix(mt.Ptr() );
	}

	curHandle = m_stdMatrices[e_ObjToWorldIT];
	if( curHandle != NULL && curHandle->IsValid() )
	{
		maMatrix4x4 mt = i_WorldMat;
		mt.Transpose();
		curHandle->AsMatrix()->SetMatrix(mt.Ptr() );
	}

	curHandle = m_stdMatrices[e_WorldToViewIT];
    if( curHandle != NULL && curHandle->IsValid() )
	{
		maMatrix4x4 mt = i_CameraMat;
		mt.Transpose();
		curHandle->AsMatrix()->SetMatrix(mt.Ptr() );
	}

	curHandle = m_stdMatrices[e_ViewToProj];
    if( curHandle != NULL && curHandle->IsValid() )
	{
		maMatrix4x4 mt = i_ProjMat;
		mt.Transpose();
		curHandle->AsMatrix()->SetMatrix(mt.Ptr() );
	}

	curHandle = m_stdMatrices[e_ObjToView];
    if( curHandle != NULL && curHandle->IsValid() )
    {
		maMatrix4x4 worldView = i_WorldMat * i_CameraMat;
		worldView.Transpose();
		curHandle->AsMatrix()->SetMatrix(worldView.Ptr() );
    }

	// assuming wvI == wvT therefore wvIT == wv.
	curHandle = m_stdMatrices[e_ObjToViewIT];
    if( curHandle != NULL && curHandle->IsValid() )
    {
		maMatrix4x4 worldView = i_WorldMat * i_CameraMat;
		worldView.Transpose();
		curHandle->AsMatrix()->SetMatrix(worldView.Ptr() );
    }

	curHandle = m_stdMatrices[e_WorldToProj];
    if( curHandle != NULL && curHandle->IsValid() )
    {
		// we could send in camera*proj in function parameters, it should
		// be already computed.
		maMatrix4x4 viewProj = i_CameraMat * i_ProjMat;
		viewProj.Transpose();
		curHandle->AsMatrix()->SetMatrix(viewProj.Ptr() );
    }

	curHandle = m_stdMatrices[e_ObjToProj];
    if( curHandle != NULL && curHandle->IsValid() )
    {
		// we could send in camera*proj in function parameters, it should
		// be already computed.
		maMatrix4x4 worldViewProj = i_WorldMat * i_CameraMat * i_ProjMat;
		worldViewProj.Transpose();
		curHandle->AsMatrix()->SetMatrix(worldViewProj.Ptr() );
    }


	if( m_VecCameraPosHandle != NULL  && m_VecCameraPosHandle->IsValid())
    {
		float vecPosition[4] = { i_CameraPos.m_X, i_CameraPos.m_Y, i_CameraPos.m_Z, 1.0f };              
		m_VecCameraPosHandle->AsVector()->SetFloatVector(vecPosition);
    }
}

//====================================================================
// Set up skinning matrix transforms
//====================================================================
bool effShaderBaseDX11::GetHasSkinning() const
{
	return (m_SkinningMatrixPaletteHandle != NULL) && m_SkinningMatrixPaletteHandle->IsValid(); 
}
void effShaderBaseDX11::SetupSkinningMatrices(const std::vector<maMatrix4x4> &i_MatrixPalette) const
{
	// all this input checking could probably be optimized! 
	DBG_ASSERT(m_SkinningMatrixPaletteHandle != NULL, "setupskinning on effect that doesn't support skinning");
	D3DX11_EFFECT_TYPE_DESC desc;
	m_SkinningMatrixPaletteHandle->GetType()->GetDesc(&desc);
	DBG_ASSERT(desc.Elements >= i_MatrixPalette.size(), "too many bones in palette");

	int n = min(desc.Elements, i_MatrixPalette.size());
	// set the data
	m_SkinningMatrixPaletteHandle->AsMatrix()->SetMatrixArray((float*)&i_MatrixPalette[0], 0, n);
}

//--------------------------------------------------------------------
// Set up shader for lighting and material given the material's 
//	colors
//--------------------------------------------------------------------
void effShaderBaseDX11::SetupLighting(const maAxisBox& i_BBox) const
{
	if( m_LightInfoHandle != NULL && m_LightInfoHandle->IsValid())
	{	
		const g3dLight* pLight = get_active_light();
		LightInfo light_info;
		if (pLight)
		{
			// Put active light into info structure
			get_light_info(pLight, light_info);
		}
		m_LightInfoHandle->SetRawValue(&light_info, 0, sizeof(light_info));
	}

	SetupAmbientLighting(i_BBox);
}

//--------------------------------------------------------------------
// Set up shader for lighting and material given the material's 
//	colors
//--------------------------------------------------------------------
void effShaderBaseDX11::SetupAmbientLighting(const maAxisBox& i_BBox) const
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"effShaderBaseDX11::SetupAmbientLight" );

	if( m_LightInfoHandle != NULL && m_LightInfoHandle->IsValid())
	{	
		const g3dLight* pLight = get_head_light();
		LightInfo light_info;
		if (pLight)
		{
			// Put active light into info structure
			get_light_info(pLight, light_info);
		}
		m_LightInfoHandle->SetRawValue(&light_info, 0, sizeof(light_info));
	}
	SetIsProjLight(false);

//	if (g3dSingleLightRendering::GetActiveLight() == NULL)
//	{
//
//		if( m_LightArrayHandle != NULL && m_LightArrayHandle->IsValid() )
//		{
//			LightInfo lights_info[8];
//			get_some_lights(lights_info, i_BBox);
//			ID3DX11Effect* pEffect = GetD3DXEffect();
//			m_LightArrayHandle->SetRawValue(lights_info, 0, sizeof(lights_info));
//		}
//	}
	D3DPERF_EndEvent();
}

//--------------------------------------------------------------------
// Set up shader for lighting through a projected texture light.
//--------------------------------------------------------------------
void effShaderBaseDX11::SetupProjectedLight(const matTexture* i_pTexture,
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
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"effShaderBaseDX11::SetupProjectedLight" );
	ID3DX11Effect* pEffect = GetD3DXEffect();
	if( m_ProjLightInfoHandle != NULL && m_ProjLightInfoHandle->IsValid())
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
		m_ProjLightInfoHandle->GetMemberByName("Pos")->AsVector()->SetFloatVector(proj_light_info.m_Position.Ptr());
		m_ProjLightInfoHandle->GetMemberByName("Matrix")->AsMatrix()->SetMatrix(proj_light_info.m_TextureMatrix.Ptr());
		m_ProjLightInfoHandle->GetMemberByName("LightSize")->AsScalar()->SetFloat(proj_light_info.m_LightSize);
		m_ProjLightInfoHandle->GetMemberByName("PCSSAdjust")->AsScalar()->SetFloat(proj_light_info.m_PCSSAdjust);
		m_ProjLightInfoHandle->GetMemberByName("Scale")->AsScalar()->SetFloat(proj_light_info.m_Scale);
		m_ProjLightInfoHandle->GetMemberByName("ShadowIntensity")->AsScalar()->SetFloat(proj_light_info.m_shadowIntensity);
		m_ProjLightInfoHandle->GetMemberByName("ShadowColor")->AsVector()->SetFloatVector(proj_light_info.m_ShadowColor.Ptr());
		m_ProjLightInfoHandle->GetMemberByName("NearFar")->AsVector()->SetFloatVector(&proj_light_info.m_Near);	
		m_ProjLightInfoHandle->GetMemberByName("MapSize")->AsScalar()->SetFloat(proj_light_info.m_MapSize);
		m_ProjLightInfoHandle->GetMemberByName("InnerAngle")->AsScalar()->SetFloat(proj_light_info.m_InnerAngle);
		m_ProjLightInfoHandle->GetMemberByName("OuterAngle")->AsScalar()->SetFloat(proj_light_info.m_OuterAngle);
		m_ProjLightInfoHandle->GetMemberByName("Aspect")->AsScalar()->SetFloat(proj_light_info.m_Aspect);
	}
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
//	pEffect->SetInt("g_nShadowSamples", i_nShadowSamples);
//	pEffect->SetInt("g_nBlockerSamples", i_nBlockerSamples);
	D3DPERF_EndEvent();	
}

//====================================================================
// Set up the shader for Hardware tessellation using a texture to store extra mesh data.
//====================================================================
void effShaderBaseDX11::SetupTessellatorMeshTexture( const matTexture* i_pTexture )
{
	ID3DX11Effect* pEffect = GetD3DXEffect();
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
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetAlphaTestRef(float i_Value) const
{
	m_AlphaTestRefHandle->AsScalar()->SetFloat(i_Value);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetClipPlane( const maVector4d& i_Plane ) const
{
	m_ClipPlaneHandle->AsVector()->SetFloatVector( i_Plane.Ptr() );
}

//--------------------------------------------------------------------
// GetImportantLight() gets the first eight important lights by
// given axis box
//--------------------------------------------------------------------
void effShaderBaseDX11::SetImportantLight(int i_num, const maAxisBox& i_BBox) const
{
	LightInfo lights_info[8];
	int light_num_max = get_some_lights(lights_info, i_BBox);
	
	int light_num = min(i_num, light_num_max);

	if (m_LightArrayNumHandle && m_LightArrayNumHandle->IsValid())
		m_LightArrayNumHandle->AsScalar()->SetInt(light_num);

	if (m_LightArrayHandle && m_LightArrayHandle->IsValid())
		m_LightArrayHandle->SetRawValue(lights_info, 0, sizeof(lights_info));
}

//--------------------------------------------------------------------
// global function that set a the hair tessellation value
//--------------------------------------------------------------------
/*
void effShaderBaseDX11::SetHairTessellationValue( const maVector2d& i_Value ) const
{
//	m_HairTessellationHandle->AsVector()->SetFloatVector( i_Value.Ptr() );
}
*/

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetTime(float i_Time) const
{
	m_TimeHandle->AsScalar()->SetFloat(i_Time);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetVertexUVBakeMode(bool i_bDoBaking) const
{
	m_hBake->AsScalar()->SetBool(i_bDoBaking);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetIsProjLight(bool i_IsProjLight) const
{
	m_IsProjLtHandle->AsScalar()->SetBool(i_IsProjLight);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetupAmbientPass(const g3dAmbientEnvState& i_AmbientEnvState) const
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"effShaderBaseDX11::SetupAmbientPass" );
	if (g3dPrefs::CurrentPrefs().m_bEnableDiffuseLighting)
	{
		m_EnvHasDiffuseMapHandle->AsScalar()->SetBool((i_AmbientEnvState.m_DiffuseMap != NULL));
		ID3D11ShaderResourceView* dTexture = g3dDX11TextureUtil::GetD3DTexture(i_AmbientEnvState.m_DiffuseMap);
		m_EnvDiffuseMapHandle->AsShaderResource()->SetResource(dTexture);
		m_EnvDiffuseFactorHandle->AsScalar()->SetFloat(i_AmbientEnvState.m_DiffuseFactor);
		m_EnvDiffuseAngleHandle->AsScalar()->SetFloat(i_AmbientEnvState.m_DiffuseAngle*maConstants::c_fAngleToRad);
		m_EnvDiffuseColorHandle->AsVector()->SetFloatVector( i_AmbientEnvState.m_DiffuseColor.Ptr() );
	}
	else
	{
		m_EnvHasDiffuseMapHandle->AsScalar()->SetBool( FALSE );
		m_EnvDiffuseMapHandle->AsShaderResource()->SetResource(NULL);
		m_EnvDiffuseFactorHandle->AsScalar()->SetFloat( 0 );
		m_EnvDiffuseAngleHandle->AsScalar()->SetFloat( 0 );
		float vDClr[4] = {0,0,0,0};
		m_EnvDiffuseColorHandle->AsVector()->SetFloatVector(vDClr);
	}

	if (g3dPrefs::CurrentPrefs().m_bEnableSpecularLighting)
	{
		m_EnvHasSpecularMapHandle->AsScalar()->SetBool((i_AmbientEnvState.m_SpecularMap != NULL));
		ID3D11ShaderResourceView* sTexture = g3dDX11TextureUtil::GetD3DTexture(i_AmbientEnvState.m_SpecularMap);
		m_EnvSpecularMapHandle->AsShaderResource()->SetResource(sTexture);
		m_EnvSpecularFactorHandle->AsScalar()->SetFloat(i_AmbientEnvState.m_SpecularFactor);
		m_EnvSpecularAngleHandle->AsScalar()->SetFloat(i_AmbientEnvState.m_SpecularAngle*maConstants::c_fAngleToRad);
		m_EnvSpecularColorHandle->AsVector()->SetFloatVector( i_AmbientEnvState.m_SpecularColor.Ptr() );
	}
	else
	{
		m_EnvHasSpecularMapHandle->AsScalar()->SetBool( FALSE );
		m_EnvSpecularMapHandle->AsShaderResource()->SetResource( NULL );
		m_EnvSpecularFactorHandle->AsScalar()->SetFloat( 0 );
		m_EnvSpecularAngleHandle->AsScalar()->SetFloat( 0 );
		float vSClr[4] = {0,0,0,0};
		m_EnvSpecularColorHandle->AsVector()->SetFloatVector(vSClr);
	}
	D3DPERF_EndEvent();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetupDOFPrep() const
{
	float vDofParams[4] = {
		g3dSceneGlobal::g_DOFParams.m_NearBlurDist,
		g3dSceneGlobal::g_DOFParams.m_NearFocalDist,
		g3dSceneGlobal::g_DOFParams.m_FarFocalDist,
		g3dSceneGlobal::g_DOFParams.m_FarBlurDist};
	m_DOFHandle->AsVector()->SetFloatVector(vDofParams);
	m_DOFBlurCutoffHandle->AsScalar()->SetFloat(g3dSceneGlobal::g_DOFParams.m_MaxFarBlur);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex) const
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"effShaderBaseDX1::SetupMaterial" );

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

	D3DPERF_EndEvent();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetIsolateReflections(bool i_bIsolateReflections) const
{
	m_hIsolateReflections->AsScalar()->SetBool( i_bIsolateReflections ? TRUE : FALSE );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetupParams(const effShaderData* i_Data) const
{
	int x = 1;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void effShaderBaseDX11::SetupGlowPass(const effGlowData& i_GlowData) const
{
	m_hHasGlowMask->AsScalar()->SetBool((i_GlowData.m_pGlowMask != NULL)?TRUE:FALSE);
	m_hGlowMask->AsShaderResource()->SetResource(g3dDX11TextureUtil::GetD3DTexture(i_GlowData.m_pGlowMask));
	m_hConstantGlow->AsScalar()->SetBool(i_GlowData.m_bConstantGlow);
	m_hGlowSize->AsScalar()->SetFloat(i_GlowData.m_GlowSize);
}

void effShaderBaseDX11::SetupOutlinePass(const effOutlineData& i_OutlineData) const
{
	m_hOutlineDepthScale->AsScalar()->SetFloat(i_OutlineData.m_OutlineDepthScale );
	m_hOutlineMinAngle->AsScalar()->SetFloat(i_OutlineData.m_OutlineMinAngle );
	m_hOutlineMaxAngle->AsScalar()->SetFloat(i_OutlineData.m_OutlineMaxAngle );
	m_hOutlineThickness->AsScalar()->SetFloat(i_OutlineData.m_OutlineThickness );
	m_hOutlineMinWidth->AsScalar()->SetFloat(i_OutlineData.m_OutlineMinWidth );
	m_hOutlineMaxWidth->AsScalar()->SetFloat(i_OutlineData.m_OutlineMaxWidth );

	m_hOutlineColor->AsVector()->SetFloatVector( i_OutlineData.m_OutlineColor.Ptr() );

	float vVSize[2] = {i_OutlineData.m_OutlineViewSize.GetX(), i_OutlineData.m_OutlineViewSize.GetY() };
	m_hOutlineViewSize->AsVector()->SetFloatVector(vVSize);
}


void effShaderBaseDX11::SetIsDoubleSided(bool i_IsDoubleSided) const
{
	m_IsDoubleSidedHandle->AsScalar()->SetBool(i_IsDoubleSided?TRUE:FALSE);
}

//--------------------------------------------------------------------
// Set up shader for lighting and material given the material's 
//	colors
//--------------------------------------------------------------------
void effShaderBaseDX11::SetupSingleLight(const g3dLight* i_pLight,
	const g3dProjectedLight* i_pProjLight, bool i_bAllowShadows) const
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"effShaderBaseDX11::SetupSingleLight" );
	if( m_LightInfoHandle != NULL && m_LightInfoHandle->IsValid())
	{	
		LightInfo light_info;
		if (i_pLight)
		{
			// Put active light into info structure
			get_light_info(i_pLight, light_info);
		}
		ID3DX11Effect* pEffect = GetD3DXEffect();
		m_LightInfoHandle->SetRawValue(&light_info, 0, sizeof(light_info));
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
	m_FirstLightHandle->AsScalar()->SetBool( g3dSingleLightRendering::IsFirstLight() );
	D3DPERF_EndEvent();
}

void effShaderBaseDX11::MapParameter(std::string i_Name, ID3DX11EffectVariable*& o_Handle)
{
	o_Handle = m_pEffect->GetVariableByName(i_Name.c_str());
//	DBG_ASSERT(o_Handle->IsValid(), "missing shader variable " << i_Name);
}
void effShaderBaseDX11::MapParameter(std::string i_Name, ID3DX11EffectScalarVariable*& o_Handle)
{
	o_Handle = m_pEffect->GetVariableByName(i_Name.c_str())->AsScalar();
//	DBG_ASSERT(o_Handle->IsValid(), "missing shader scalar variable " << i_Name);
}
void effShaderBaseDX11::MapParameter(std::string i_Name, ID3DX11EffectVectorVariable*& o_Handle)
{
	o_Handle = m_pEffect->GetVariableByName(i_Name.c_str())->AsVector();
//	DBG_ASSERT(o_Handle->IsValid(), "missing shader vector variable " << i_Name);
}
void effShaderBaseDX11::MapParameter(std::string i_Name, ID3DX11EffectMatrixVariable*& o_Handle)
{
	o_Handle = m_pEffect->GetVariableByName(i_Name.c_str())->AsMatrix();
//	DBG_ASSERT(o_Handle->IsValid(), "missing shader matrix variable " << i_Name);
}
void effShaderBaseDX11::MapParameter(std::string i_Name, ID3DX11EffectShaderResourceVariable*& o_Handle)
{
	o_Handle = m_pEffect->GetVariableByName(i_Name.c_str())->AsShaderResource();
//	DBG_ASSERT(o_Handle->IsValid(), "missing shader resource variable " << i_Name);
}

//--------------------------------------------------------------------
// set factors that rescale uv space to [0,0]..[1,1] for texture bake.
//--------------------------------------------------------------------
void effShaderBaseDX11::SetBakingFactors(const maPoint2d& i_Scale, const maPoint2d& i_Translate) const
{
	float vBakingXForm[4] = {i_Scale.m_X, i_Scale.m_Y, i_Translate.m_X, i_Translate.m_Y};
	m_hBakingTransform->AsVector()->SetFloatVector(vBakingXForm);
}

void effShaderBaseDX11::Bind(effShaderBindings* i_pBindings) const
{
	i_pBindings->Bind();
}

//--------------------------------------------------------------------
// allocates mem inside shaderparams.
//--------------------------------------------------------------------
int effShaderBaseDX11::BuildPrtyObject(effShaderParams* o_pParams) const
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
	effShaderBindingsDX11* bindings = new effShaderBindingsDX11(this->m_pEffect);
	o_pParams->m_pShaderBindings = bindings;
	o_pParams->m_pPrtyUI = new prtyObject;

	std::string name;
	int index;
	ID3DX11EffectVariable* hParam;

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
void effShaderBaseDX11::CreateBindings(effShaderParams* io_Params)
{
	// delete old bindings.
	delete io_Params->m_pShaderBindings;
	io_Params->m_pShaderBindings = NULL;

	int n = io_Params->m_Params.size();
	if (n < 1)
		return;

	effShaderBindingsDX11* bindings = new effShaderBindingsDX11(this->m_pEffect);
	io_Params->m_pShaderBindings = bindings;

	for (int i = 0; i < n; i++)
	{
		// current param from list
		effShaderParam* p = io_Params->m_Params[i];

		// get a matching effect param handle based on name
		ID3DX11EffectVariable* h = m_pEffect->GetParameterByName(NULL, p->m_Name.c_str());
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
					ID3DX11EffectVariable* hExistVar = FindTextureExistVar(h);
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

bool effShaderBaseDX11::GetShaderParamInfo(ID3DX11EffectVariable* i_hParam, 
										  effShaderParams* o_pParams,
										  effShaderBindingsDX11* o_pBindings,
										  std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	bool foundParam = false;
	float fval = 0;

	effParamInt* pInt = NULL;
	effParamBool* pBool = NULL;
	effParamFloat* pFloat = NULL;
	effParamTexture* pTexture = NULL;
	effParamColor* pColor = NULL;

	D3DX11_EFFECT_VARIABLE_DESC paramDesc;
	i_hParam->GetDesc(&paramDesc);
	std::string name(paramDesc.Name);

	ID3DX11EffectVariable* hAnnot = NULL;
/*		
	bool bVisible = true;
	hAnnot = hParam->GetAnnotationByName("SasUiVisible");
	if (hAnnot)
	{
		hAnnot->AsString()->GetString( &pstr);
		if (!_strcmpi(pstr, "false"))
			bVisible = false;
		else
			bVisible = true;
	}
*/

	// this is the master annotation that defines that we have a user parameter that gets a prty entry.
	std::string sControl;
	hAnnot = i_hParam->GetAnnotationByName("SasUiControl");
	if (hAnnot && hAnnot->IsValid())
	{
		LPCSTR psControl = NULL;
		hAnnot->AsString()->GetString( &psControl);
		if (psControl)
			sControl = psControl;
	}
	else
	{
		// early out
		return false;
	}

	std::string semantic;
	if (paramDesc.Semantic != NULL)
	{
		semantic = paramDesc.Semantic;
		std::transform(semantic.begin(), semantic.end(), semantic.begin(), /*std::*/tolower);
	}


	if (sControl == "ColorPicker")
	{
		ID3DX11EffectVectorVariable* pColorVar = i_hParam->AsVector();
		if (pColorVar)
		{
			foundParam = true;

			effParamColor* effParam = MapColorParam(o_pParams, o_pBindings, pColorVar, name, true, true, o_UIInfo);

			// check to see if this is a "special" color:
			if (semantic == "materialdiffuse" )
				o_pParams->m_pDiffuseColor = effParam;
		}
	}
	else if (sControl == "Direction")
	{
		float f[4];
		i_hParam->AsVector()->GetFloatVector(f);
	}
	else if (sControl == "Numeric")
	{
		float f;
		i_hParam->AsScalar()->GetFloat(&f);


		float fstride = 1;
		hAnnot = i_hParam->GetAnnotationByName("SasUiStride");
		if (hAnnot)
		{
			hAnnot->AsScalar()->GetFloat(&fstride);
		}

	}
	else if (sControl == "Slider")
	{
		ID3DX11EffectScalarVariable* pFloatVar = i_hParam->AsScalar();
		if (pFloatVar)
		{
			foundParam = true;
		
			effParamFloat* effParam = MapFloatParam(o_pParams, o_pBindings, pFloatVar,
				name, true, true, o_UIInfo);

			// check to see if this is a "special" param:
			if (semantic == "opacity")
				o_pParams->m_pTransparency = effParam;
		}
		else
		{
			DBG_WARNING("Shader Slider can only be mapped to a float variable. Check " << name);
		}
	}
	else if (sControl == "CheckBox")
	{
		ID3DX11EffectScalarVariable* pBoolVar = i_hParam->AsScalar();
		if (pBoolVar)
		{
			foundParam = true;

			effParamBool* effParam = MapBoolParam(o_pParams, o_pBindings, pBoolVar, name, true, true, o_UIInfo);
		}
	}
	else if (sControl == "FilePicker")
	{
		// textures!!!!
		ID3DX11EffectShaderResourceVariable* pTextureVar = i_hParam->AsShaderResource();
		if (pTextureVar)
		{
			foundParam = true;

			effParamTexture* effParam = MapTextureParam(o_pParams, o_pBindings, pTextureVar, name, true, true, o_UIInfo);

			// check to see if this is a "special" param:
			if (semantic == "diffusetexture" )
				o_pParams->m_pDiffuseMap = effParam;
			else if(semantic == "opacitytexture" )
				o_pParams->m_pTransparencyMap = effParam;

			// look for a default value and store with property
			fsLocator resource_name;
			if (get_resource_name(i_hParam, resource_name))
				effParam->SetShaderResourceName( resource_name );
		}
		else
		{
			DBG_WARNING("Shader FilePicker can only be mapped to a texture variable. Check " << name);
		}
	}
	else if (sControl == "TextureFilePicker")
	{
		// textures!!!!
		ID3DX11EffectShaderResourceVariable* pTextureVar = i_hParam->AsShaderResource();
		if (pTextureVar)
		{
			foundParam = true;

			effParamTexture* effParam = MapTextureParam(o_pParams, o_pBindings, pTextureVar, name, true, true, o_UIInfo);

			// check to see if this is a "special" param:
			if (semantic == "diffusetexture" )
				o_pParams->m_pDiffuseMap = effParam;
			else if(semantic == "opacitytexture" )
				o_pParams->m_pTransparencyMap = effParam;

			// look for a default value and store with property
			fsLocator resource_name;
			if (get_resource_name(i_hParam, resource_name))
				effParam->SetShaderResourceName( resource_name );
		}
		else
		{
			DBG_WARNING("Shader FilePicker can only be mapped to a texture variable. Check " << name);
		}
	}
	else if (sControl == "ListPicker")
	{
		ID3DX11EffectScalarVariable* pEnumVar = i_hParam->AsScalar();
		if (pEnumVar)
		{
			foundParam = true;
	
			effParamInt* effParam = MapEnumParam(o_pParams, o_pBindings, pEnumVar, name, true, true, o_UIInfo);
		}
	}
	else if (sControl == "Any")
	{
	}
	else if (sControl == "None")
	{
	}

	return foundParam;
}

effParamTexture* effShaderBaseDX11::MapTextureParam(effShaderParams* o_pParams,
												   effShaderBindingsDX11* o_pBindings,
												   ID3DX11EffectShaderResourceVariable* i_hParam, 
												   const std::string& i_Name,
												   bool i_bCreateBinding,
												   bool i_bCreateUI,
												   std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	std::string uiCategory, uiLabel, uiDesc;
	GetUIStrings(m_pEffect, i_hParam, i_Name, uiCategory, uiLabel, uiDesc);

	effParamTexture* effParam = o_pParams->FindTextureParam(i_Name);
	if (effParam)
	{
		effParam->Property().SetPropertyName(uiLabel);
	}
	else
	{
		effParam = new effParamTexture(i_Name, uiLabel, itString(""));
		o_pParams->AddParam(effParam);
	}

	//parse texture type
	D3DX11_EFFECT_TYPE_DESC desc;
	i_hParam->GetType()->GetDesc(&desc );

	TEXTURE_TYPE type = TEXTURE_TYPE_UNKNOWN;

	switch( desc.Type )
	{
		case D3D10_SVT_TEXTURE1D:{ type = TEXTURE_TYPE_1D; break;}
		case D3D10_SVT_TEXTURE2D:{ type = TEXTURE_TYPE_2D; break;}
		case D3D10_SVT_TEXTURECUBE:{ type = TEXTURE_TYPE_CUBE; break;}
		case D3D10_SVT_TEXTURE3D:{ type = TEXTURE_TYPE_3D; break;}
	}
	effParam->SetType( type );

	if (i_bCreateBinding)
	{

		// can we assume binding doesn't already exist here?
		// do not add a binding twice!
		DBG_ASSERT(!o_pBindings->HasBinding(effParam), "Binding already exists for " << i_Name);
		ID3DX11EffectScalarVariable* hExistVar = FindTextureExistVar(m_pEffect, i_hParam);
		o_pBindings->m_BindableParams.push_back(new effTextureBindingDX11(*effParam, i_hParam, hExistVar));
	}

	if (i_bCreateUI)
	{
		ID3DX11EffectVariable* hAnnot = NULL;

		prtyTextureFileChooserUIInfo* pPUII;
		pPUII = new prtyTextureFileChooserUIInfo(effParam->GetBaseProperty(), uiCategory, uiDesc);
		pPUII->SetDirectoryCategory("Textures");
		pPUII->AddItem(pPUII->e_Ramp);
		pPUII->AddItem(pPUII->e_Paint);
		ShaderParamUIInfo info;
		info.UIInfo = pPUII;
		info.Index = 0;
		hAnnot = i_hParam->GetAnnotationByName("UiIndex");
		if (hAnnot)
		{
			hAnnot->AsScalar()->GetInt(&info.Index);
		}
		o_UIInfo.push_back(info);
//		o_pParams->m_pPrtyUI->AddProperty( pPUII );
		// note file resource uiinfos for later usage
		o_pParams->m_TextureParamUIs.push_back(effShaderParams::effTextureUI(pPUII, effParam));
	}

	return effParam;
}

effParamFloat* effShaderBaseDX11::MapFloatParam(effShaderParams* o_pParams,
											   effShaderBindingsDX11* o_pBindings,
											   ID3DX11EffectScalarVariable* i_hParam, 
											   const std::string& i_Name,
											   bool i_bCreateBinding,
											   bool i_bCreateUI,
											   std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	std::string uiCategory, uiLabel, uiDesc;
	GetUIStrings(m_pEffect, i_hParam, i_Name, uiCategory, uiLabel, uiDesc);

	effParamFloat* effParam = o_pParams->FindFloatParam(i_Name);
	if (effParam)
	{
		effParam->Property().SetPropertyName(uiLabel);
	}
	else
	{
		float f;
		HRESULT hres = i_hParam->GetFloat(&f);
		effParam = new effParamFloat(i_Name, uiLabel, f);
		o_pParams->AddParam(effParam);
	}

	if (i_bCreateBinding)
	{
		// can we assume binding doesn't already exist here?
		// do not add a binding twice!
		DBG_ASSERT(!o_pBindings->HasBinding(effParam), "Binding already exists for " << i_Name);
		o_pBindings->m_BindableParams.push_back(new effFloatBindingDX11(*effParam, i_hParam));
	}

	if (i_bCreateUI)
	{
		ID3DX11EffectVariable* hAnnot = NULL;

		// get the slider (ranged float) params
		float fmax=1, fmin=0, fsteps=100, fstepspower=1;
		hAnnot = i_hParam->GetAnnotationByName("SasUiMax");
		if (hAnnot)
		{
			hAnnot->AsScalar()->GetFloat( &fmax);
		}
		hAnnot = i_hParam->GetAnnotationByName("SasUiMin");
		if (hAnnot)
		{
			hAnnot->AsScalar()->GetFloat( &fmin);
		}
		hAnnot = i_hParam->GetAnnotationByName("SasUiSteps");
		if (hAnnot)
		{
			hAnnot->AsScalar()->GetFloat( &fsteps);
		}
		hAnnot = i_hParam->GetAnnotationByName("SasUiPower");
		if (hAnnot)
		{
			hAnnot->AsScalar()->GetFloat( &fstepspower);
		}
		prtyRangedFloatUIInfo* pRFUII = NULL;
		pRFUII  = new prtyRangedFloatUIInfo(effParam->GetBaseProperty(), uiCategory, uiDesc);
		pRFUII->SetMinimum(fmin);
		pRFUII->SetMaximum(fmax);
		pRFUII->SetNumTicks((short)fsteps);
		pRFUII->SetExponent((short)fstepspower);
		ShaderParamUIInfo info;
		info.UIInfo = pRFUII;
		info.Index = 0;
		hAnnot = i_hParam->GetAnnotationByName("UiIndex");
		if (hAnnot)
		{
			hAnnot->AsScalar()->GetInt(&info.Index);
		}
		o_UIInfo.push_back(info);
//		o_pParams->m_pPrtyUI->AddProperty( pRFUII );
	}

	return effParam;
}

effParamBool* effShaderBaseDX11::MapBoolParam(effShaderParams* o_pParams,
	effShaderBindingsDX11* o_pBindings,
	ID3DX11EffectScalarVariable* i_hParam, 
	const std::string& i_Name,
	bool i_bCreateBinding,
	bool i_bCreateUI,
	std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	std::string uiCategory, uiLabel, uiDesc;
	GetUIStrings(m_pEffect, i_hParam, i_Name, uiCategory, uiLabel, uiDesc);

	effParamBool* effParam = o_pParams->FindBoolParam(i_Name);
	if (effParam)
	{
		effParam->Property().SetPropertyName(uiLabel);
	}
	else
	{
		bool bval;
		HRESULT hres = i_hParam->GetBool(&bval);
		effParam = new effParamBool(i_Name, uiLabel, (bval)?true:false);
		o_pParams->AddParam(effParam);
	}

	if (i_bCreateBinding)
	{
		// can we assume binding doesn't already exist here?
		// do not add a binding twice!
		DBG_ASSERT(!o_pBindings->HasBinding(effParam), "Binding already exists for " << i_Name);
		o_pBindings->m_BindableParams.push_back(new effBoolBindingDX11(*effParam, i_hParam));
	}

	if (i_bCreateUI)
	{
		ID3DX11EffectVariable* hAnnot = NULL;

		prtyCheckBoxUIInfo* pRFUII = NULL;
		pRFUII  = new prtyCheckBoxUIInfo(effParam->GetBaseProperty(), uiCategory, uiDesc);
		ShaderParamUIInfo info;
		info.UIInfo = pRFUII;
		info.Index = 0;
		hAnnot = i_hParam->GetAnnotationByName("UiIndex");
		if (hAnnot)
		{
			hAnnot->AsScalar()->GetInt(&info.Index);
		}
		o_UIInfo.push_back(info);
//		o_pParams->m_pPrtyUI->AddProperty( pPUII );
	}

	return effParam;
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


effParamInt* effShaderBaseDX11::MapEnumParam(effShaderParams* o_pParams,
	effShaderBindingsDX11* o_pBindings,
	ID3DX11EffectScalarVariable* i_hParam, 
	const std::string& i_Name,
	bool i_bCreateBinding,
	bool i_bCreateUI,
	std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	std::string uiCategory, uiLabel, uiDesc;
	GetUIStrings(m_pEffect, i_hParam, i_Name, uiCategory, uiLabel, uiDesc);

	effParamInt* effParam = o_pParams->FindIntParam(i_Name);
	if (effParam)
	{
		effParam->Property().SetPropertyName(uiLabel);
	}
	else
	{
		int ival;
		HRESULT hres = i_hParam->GetInt(&ival);
		effParam = new effParamInt(i_Name, uiLabel, ival);
		o_pParams->AddParam(effParam);
	}

	if (i_bCreateBinding)
	{
		// can we assume binding doesn't already exist here?
		// do not add a binding twice!
		DBG_ASSERT(!o_pBindings->HasBinding(effParam), "Binding already exists for " << i_Name);
		o_pBindings->m_BindableParams.push_back(new effIntBindingDX11(*effParam, i_hParam));
	}

	if (i_bCreateUI)
	{
		ID3DX11EffectVariable* hAnnot = NULL;

		std::vector<std::string> entries;
		hAnnot = i_hParam->GetAnnotationByName("SasUiEnum");
		if (hAnnot)
		{
			LPCSTR cStr = NULL;
			hAnnot->AsString()->GetString( &cStr);
			std::string s(cStr);
			DBG_ASSERT(s.length() > 0, "no enum entries for SasUiEnum in shader");
			
			split(s, std::string(","), entries);
		}

		prtyComboBoxUIInfo* pCBUII = NULL;
		pCBUII  = new prtyComboBoxUIInfo(effParam->GetBaseProperty(), uiCategory, uiDesc);
		int n = entries.size();
		for (int i = 0; i < n; i++)
		{
			pCBUII->AddItem(entries[i]);
		}
		ShaderParamUIInfo info;
		info.UIInfo = pCBUII;
		info.Index = 0;
		hAnnot = i_hParam->GetAnnotationByName("UiIndex");
		if (hAnnot)
		{
			hAnnot->AsScalar()->GetInt(&info.Index);
		}
		o_UIInfo.push_back(info);
//		o_pParams->m_pPrtyUI->AddProperty( pPUII );
	}

	return effParam;
}

effParamColor* effShaderBaseDX11::MapColorParam(effShaderParams* o_pParams,
	effShaderBindingsDX11* o_pBindings,
	ID3DX11EffectVectorVariable* i_hParam, 
	const std::string& i_Name,
	bool i_bCreateBinding,
	bool i_bCreateUI,
	std::list<ShaderParamUIInfo>& o_UIInfo) const
{
	std::string uiCategory, uiLabel, uiDesc;
	GetUIStrings(m_pEffect, i_hParam, i_Name, uiCategory, uiLabel, uiDesc);

	// make the param
	effParamColor* effParam = o_pParams->FindColorParam(i_Name);
	if (effParam)
	{
		effParam->Property().SetPropertyName(uiLabel);
	}
	else
	{
		float f[4];
		HRESULT hres = i_hParam->GetFloatVector(f);
		effParam = new effParamColor(i_Name, uiLabel, maFloatRGBA(f[0],f[1],f[2],f[3]));
		o_pParams->AddParam(effParam);
	}

	// make the binding
	if (i_bCreateBinding)
	{
		// can we assume binding doesn't already exist here?
		// do not add a binding twice!
		DBG_ASSERT(!o_pBindings->HasBinding(effParam), "Binding already exists for " << i_Name);
		o_pBindings->m_BindableParams.push_back(new effColorBindingDX11(*effParam, i_hParam));
	}

	// make the uiinfo
	if (i_bCreateUI)
	{
		ID3DX11EffectVariable* hAnnot = NULL;

		prtyPropertyUIInfo* pPUII;
		pPUII  = new prtyColorRGBEditUIInfo(effParam->GetBaseProperty(), uiCategory, uiDesc);
		ShaderParamUIInfo info;
		info.UIInfo = pPUII;
		info.Index = 0;
		hAnnot = i_hParam->GetAnnotationByName("UiIndex");
		if (hAnnot)
		{
			hAnnot->AsScalar()->GetInt(&info.Index);
		}
		o_UIInfo.push_back(info);
//		o_pParams->m_pPrtyUI->AddProperty( pPUII );
	}

	return effParam;
}

void effShaderBaseDX11::SetupReflectionMap(bool i_bIsPlanar, matTexture* i_pReflectionMap) const
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"effShaderBaseDX11::SetupReflectionMap" );
	if (g3dSingleLightRendering::GetDoReflectionGen() ||
		!g3dPrefs::CurrentPrefs().m_bEnableReflection ||
		g3dPassBuffers::GetDoingFileRefl() 
		) 
	{
		// during reflection generation, don't try to assign the map!
		if (m_hCubeMapEnabled)
			m_hCubeMapEnabled->AsScalar()->SetBool(FALSE);
		if (m_HasReflectionMapHandle)
			m_HasReflectionMapHandle->AsScalar()->SetBool(FALSE);
		if (m_ReflectionMapIsPlanarHandle)
			m_ReflectionMapIsPlanarHandle->AsScalar()->SetBool((i_bIsPlanar)?TRUE:FALSE);
		if (m_CubeReflectionMapHandle)
			m_CubeReflectionMapHandle->AsShaderResource()->SetResource( NULL );
		if (m_PlanarReflectionMapHandle)
			m_PlanarReflectionMapHandle->AsShaderResource()->SetResource( NULL );
		if (m_IsReflectionGenHandle)
			m_IsReflectionGenHandle->AsScalar()->SetBool( (BOOL)g3dSingleLightRendering::GetDoReflectionGen() );
	}
	else
	{
		if (m_hCubeMapEnabled)
			m_hCubeMapEnabled->AsScalar()->SetBool(TRUE);
		if (m_HasReflectionMapHandle)
			m_HasReflectionMapHandle->AsScalar()->SetBool((i_pReflectionMap != NULL)?TRUE:FALSE);
		if (m_ReflectionMapIsPlanarHandle)
			m_ReflectionMapIsPlanarHandle->AsScalar()->SetBool((i_bIsPlanar)?TRUE:FALSE);
		if (m_CubeReflectionMapHandle && m_PlanarReflectionMapHandle)
		{
			if (i_bIsPlanar)
			{
				ID3D11ShaderResourceView* texture = g3dDX11TextureUtil::GetD3DTexture(i_pReflectionMap);
				m_CubeReflectionMapHandle->AsShaderResource()->SetResource( NULL );
				m_PlanarReflectionMapHandle->AsShaderResource()->SetResource( texture );
			}
			else
			{
				ID3D11ShaderResourceView* texture = g3dDX11TextureUtil::GetD3DTexture(i_pReflectionMap);
				m_CubeReflectionMapHandle->AsShaderResource()->SetResource( texture );
				m_PlanarReflectionMapHandle->AsShaderResource()->SetResource( NULL );
			}
		}
		if (m_IsReflectionGenHandle)
			m_IsReflectionGenHandle->AsScalar()->SetBool( FALSE );
	}
	D3DPERF_EndEvent();
}

//--------------------------------------------------------------------
// does the shader hook into our dynamic reflection mapping?
//--------------------------------------------------------------------
bool effShaderBaseDX11::HasReflectionMap() const
{
	return (m_ReflectionMapIsPlanarHandle != NULL) && m_ReflectionMapIsPlanarHandle->IsValid() &&
		(m_HasReflectionMapHandle != NULL) && m_HasReflectionMapHandle->IsValid() &&
		(m_CubeReflectionMapHandle != NULL) && m_CubeReflectionMapHandle->IsValid() &&
		(m_PlanarReflectionMapHandle != NULL) && m_PlanarReflectionMapHandle->IsValid();
}

//------------------------------------------------------------------------
// function that will initialize the paint overlay texture for the shader
//------------------------------------------------------------------------
void  effShaderBaseDX11::SetupPaintOverlay(matRenderTargetTexture* i_pRenderTexture) const
{
	/*D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"effShaderBaseDX11::SetupPaintOverlay" );
	g2dD3D11BaseTexturePtr texture = g3dDX11TextureUtil::GetD3DTexture(i_pRenderTexture);
 
	if(texture != NULL)
		m_pEffect->SetTexture(m_PaintOverlayMapHandle, texture);

	D3DPERF_EndEvent();*/

//	ID3D11ShaderResourceView* texture = g3dDX11TextureUtil::GetD3DTexture(i_pRenderTexture);
//	m_PaintOverlayMapHandle->AsShaderResource()->SetResource(texture);

}

bool effShaderBaseDX11::HasHardwareTessellation() const
{
	return (m_hHardwareTessellationHandle != NULL);
}

int effShaderBaseDX11::FindShaderVersion()
{
	// get the shader revision param.
	ID3DX11EffectVariable* hGlobal = m_pEffect->GetVariableBySemantic("SasGlobal");
	if (hGlobal && hGlobal->IsValid())
	{
		ID3DX11EffectVariable* hAnnot = hGlobal->GetAnnotationByName("SasEffectRevision");
		if (hAnnot && hAnnot->IsValid())
		{
		    LPCSTR pstrRev = NULL;
			hAnnot->AsString()->GetString( &pstrRev );
			if (pstrRev)
			{
				std::stringstream ss(pstrRev);
				int version = 0;
				if(!(ss >> version).fail())
				{ 
					return version;
				}
			}
		}
	}
	return 0;
}

//--------------------------------------------------------------------
// return null if not found by name. otherwise add to o_params
//--------------------------------------------------------------------
effParamTexture* effShaderBaseDX11::MapTextureParam(effShaderParams* o_pParams,
	const std::string& i_Name) const
{
	ID3DX11EffectShaderResourceVariable* hParam = m_pEffect->GetVariableByName(i_Name.c_str())->AsShaderResource();
	if (!hParam->IsValid())
		return NULL;

	effParamTexture* effParam = o_pParams->FindTextureParam(i_Name);
	if (effParam)
		return effParam;

	effParam = new effParamTexture(i_Name, i_Name, itString(""));
	o_pParams->AddParam(effParam);

	//parse texture type
	D3DX11_EFFECT_TYPE_DESC desc;
	hParam->GetType()->GetDesc(&desc );
	TEXTURE_TYPE type = TEXTURE_TYPE_UNKNOWN;

	switch( desc.Type )
	{
		case D3D10_SVT_TEXTURE1D:{ type = TEXTURE_TYPE_1D; break;}
		case D3D10_SVT_TEXTURE2D:{ type = TEXTURE_TYPE_2D; break;}
		case D3D10_SVT_TEXTURECUBE:{ type = TEXTURE_TYPE_CUBE; break;}
		case D3D10_SVT_TEXTURE3D:{ type = TEXTURE_TYPE_3D; break;}
	}
	effParam->SetType( type );

	ID3DX11EffectScalarVariable* hExistVar = FindTextureExistVar(m_pEffect, hParam);

	effShaderBindingsDX11* pBindings = dynamic_cast<effShaderBindingsDX11*>(o_pParams->m_pShaderBindings);
	pBindings->m_BindableParams.push_back(new effTextureBindingDX11(*effParam, hParam, hExistVar));

	return effParam;
}

//====================================================================
// Set up the shader for displacement mapping parameters
//====================================================================
void effShaderBaseDX11::SetupDisplacementMap( const matTexture* i_pTexture, float i_Scale, float i_Bias, float i_Blur, const maVector2d& i_ObjUVScale )
{
	if( m_hDisplacementMap )
	{
		ID3D11ShaderResourceView* texture = g3dDX11TextureUtil::GetD3DTexture(i_pTexture);
		m_hDisplacementMap->AsShaderResource()->SetResource( texture );

		if( i_pTexture )
		{
			float displSize[2] = { (float)i_pTexture->GetWidth(), (float)i_pTexture->GetHeight() };
			ID3DX11EffectVariable* dms = m_pEffect->GetVariableByName("g_DisplacementMapSize");
			dms->AsVector()->SetFloatVector(displSize);
		}
	}
	if( m_hDisplacementScale->IsValid() ) m_hDisplacementScale->AsScalar()->SetFloat( i_Scale );
	if( m_hDisplacementBias->IsValid() ) m_hDisplacementBias->AsScalar()->SetFloat( i_Bias );
	if( m_hDisplacementBlur->IsValid() ) m_hDisplacementBlur->AsScalar()->SetFloat( i_Blur );
	if( m_hHasDisplacementMap->IsValid() ) m_hHasDisplacementMap->AsScalar()->SetBool( i_pTexture != NULL );
	if( m_hDisplacementObjUVScale->IsValid() )
	{
		float objSize[2] = { i_ObjUVScale.GetX(), i_ObjUVScale.GetY() };
		m_hDisplacementObjUVScale->AsVector()->SetFloatVector( objSize );
	}
}

bool effShaderBaseDX11::HasDisplacementMap() const
{
	if( m_hDisplacementMap )
	{
	}
	return false;
}

void effShaderBaseDX11::SetupUVTransform( const maMatrix4x4 &i_WorldMat )
{
	m_UVTransformHandle->AsMatrix()->SetMatrix(i_WorldMat.Ptr());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void effShaderBaseDX11::SetTessellateValue( float i_Value )
{
	if (m_hTessValueHandle)
	{
		float v[4] = {i_Value, i_Value, g3dPrefs::CurrentPrefs().m_PixelSubdivLimit,0};
		m_hTessValueHandle->AsVector()->SetFloatVector(v);
	}
}

void effShaderBaseDX11::SetupNormalMap( const matTexture* i_pNormalMap, float i_BumpScale )
{
	ID3D11ShaderResourceView* normalMap = g3dDX11TextureUtil::GetD3DTexture(i_pNormalMap);

	if( m_hNormalMap )
	{
		m_hNormalMap->AsShaderResource()->SetResource(normalMap);
	}

	if ( m_hBumpScale )
	{
		m_hBumpScale->AsScalar()->SetFloat(i_BumpScale);
	}

	if( m_hHasNormalMap )
	{	
		m_hHasNormalMap->AsScalar()->SetBool( normalMap==NULL?FALSE:TRUE );		
	}

}

//====================================================================
// Check if the shader supports outline
//====================================================================
bool effShaderBaseDX11::GetSupportOutline() const
{
	bool bValue = false;

	if (m_pEffect)
	{
		ID3DX11EffectVariable* hGlobal = m_pEffect->GetVariableBySemantic("SasGlobal");
		if (hGlobal != NULL)
		{
			ID3DX11EffectVariable* hAnnot = NULL;
			hAnnot = hGlobal->GetAnnotationByName("SupportsOutline");
			if (hAnnot)
			{
			    LPCSTR pstrName = NULL;
				ID3DX11EffectStringVariable* hString = hAnnot->AsString();
				if (hString)
					hString->GetString( &pstrName );
				if (pstrName)
					bValue = _stricmp( pstrName, "true" ) == 0 ? true : false;
				else 
					bValue = false;
			}
		}
	}
	return bValue;
}
