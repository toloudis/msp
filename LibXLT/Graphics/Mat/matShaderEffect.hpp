/*****************************************************************************
**  matShaderEffect.hpp
**
**    This is an abstract base class for a programmatic effect that may
**	involve multiple passes with different render settings.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef MAT_SHADEREFFECT_HPP
#error matShaderEffect.hpp multiply included
#endif
#define MAT_SHADEREFFECT_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif

#include <string>
#include <list>
#include <vector>

class effGlowData;
class effNormalsData;
class effRendermanOverrideData;
class effOutlineData;
class effOcclusionData;
class effShaderParams;
class fsLocator;
class fsResourceFinder;
struct g3dAmbientEnvState;
class g3dLight;
class g3dProjectedLight;
class maAxisBox;
class matMaterial;
class matTexture;
class matRenderTargetTexture;

struct matShaderParamUI
{
	enum UIType
	{
		e_None,
		e_Any,
		e_ColorPicker,
		e_Direction,
		e_ListPicker,
		e_Numeric,
		e_Slider,
		e_Checkbox,
		e_FolderPicker,
		e_TexturePicker
	};
	enum DataType
	{
		e_Unknown,
		e_Float,
		e_Vector,
		e_Matrix,
		e_Texture,
		e_Struct,
		e_Bool,
		e_String
	};

	DataType m_dataType;

	std::string m_name;
	std::string m_label;
	std::string m_desc;

	UIType m_uiType;
	float m_min, m_max, m_steps, m_power, m_stride;
	bool m_visible;

	// only data types with up to 4 floats (matrices,textures,structs unsupported right now)
	float m_fval[4];
	// save a string for the texture path if datatype is tex or string.
	std::string m_sval;

	bool Visible() {return m_visible && (m_uiType != e_None);}
};

class effUVTransform
{
public:
	effUVTransform() : m_UScale(1), m_VScale(1), m_UTrans(0), m_VTrans(0), m_UVAngle(0) {}

	effUVTransform(const effUVTransform& i_CopyFrom);
	effUVTransform& operator = (const effUVTransform& i_CopyFrom);

	// uv transformation parameters are a feature of all material shaders!
	maMatrix4x4 MakeUVTransform() const;
	float m_UScale, m_VScale, m_UTrans, m_VTrans, m_UVAngle;

};

//class effTextureFilter
//{
//public:
//	effTextureFilter() : m_bEnableMipmap(true){}
//
//	effTextureFilter(const effTextureFilter& i_CopyFrom);
//	effTextureFilter& operator = (const effTextureFilter& i_CopyFrom);
//
//	float m_bEnableMipmap;
//
//};

class effShaderData
{
public:
	effShaderData() {}
	virtual ~effShaderData() {}

	virtual effShaderData* Clone() const = 0;
	
	virtual bool HasTransparency() { return false; }

	// add textures to list
	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const {}
	virtual void GetTextureNames(std::vector<std::string>& io_Textures) const {}

	// load textures using matTextureMgr
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory) {}
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder) {}

	// set all textures to NULL
	virtual void RemoveTextures() {}

	// Colors used by SetupLighting() so they can be premultiplied by the light color.
	// If not overridden, then lightinfo.diffuse and spec will be the true light colors.
	virtual maFloatRGBA GetDiffuse() const {return maFloatRGBA(1,1,1,1);}
	virtual maFloatRGBA GetAmbient() const {return maFloatRGBA(1,1,1,1);}
	virtual maFloatRGBA GetEmissive() const {return maFloatRGBA(0,0,0,0);}
	virtual maFloatRGBA GetSpecular() const {return maFloatRGBA(1,1,1,0);}

	// override to make readable/writable
	virtual chDefs::Name GetChunkName() const {return 0;}

	virtual matTexture* GetTransparencyTexture(){ return NULL; }
	virtual float GetTransparencyValue() const { return 1.0f; }
};

class effShaderEffect
{
public:
	//====================================================================
	//====================================================================
	effShaderEffect() {};

	//====================================================================
	//====================================================================
	virtual ~effShaderEffect() {};

	//====================================================================
	// Start effect, returns number of passes needed
	//====================================================================
	virtual int Begin() const = 0;

	//====================================================================
	// Set up values for this pass
	//====================================================================
	virtual void BeginPass(int i_Pass) const = 0;

	//====================================================================
	// Finish this pass, restoring states
	//====================================================================
	virtual void EndPass() const = 0;

	//====================================================================
	// End effect, call even if number of passes is 0
	//====================================================================
	virtual void End() const = 0;

	//====================================================================
	// Map from name of param to (faster) int lookup index
	//====================================================================
	virtual int GetParamIndex(const std::string& i_name) const = 0;

	//====================================================================
	// Find named param, and fill the ui info struct
	//====================================================================
	virtual bool GetParamUI(const std::string& i_name, matShaderParamUI& o_paramUI) const = 0;

	//====================================================================
	// Find all named params with ui info
	//====================================================================
	virtual void GetAllParamUIs(std::list<matShaderParamUI>& o_paramUI) const = 0;

	//====================================================================
	// Set a matrix param
	//====================================================================
	virtual void SetMatrix(int i_param, const maMatrix4x4& i_matrix) = 0;

	//====================================================================
	// Set a float param
	//====================================================================
	virtual void SetFloat(int i_param, float i_float) = 0;

	//====================================================================
	// Set a vector param
	//====================================================================
	virtual void SetVector(int i_param, const maVector4d& i_vector) = 0;

	//====================================================================
	// Set a struct param
	//====================================================================
	virtual void SetData(int i_param, void* i_data, unsigned int i_nbytes) = 0;

	//====================================================================
	// Set a bool param
	//====================================================================
	virtual void SetBool(int i_param, bool i_bool) = 0;

	//====================================================================
	// Set a string param
	//====================================================================
	virtual void SetString(int i_param, std::string i_string) = 0;

	//====================================================================
	// Set Texture into effect
	//====================================================================
	virtual void SetTexture(int i_param, const matTexture* i_pTexture) const = 0;

	virtual std::string GetName() const = 0;
	virtual void SetName(std::string i_name) = 0;

	virtual effShaderData* CreateData(const matMaterial* i_Mat) {return NULL;}
	
	//====================================================================
	// Returns false if this effect only needs the ambient pass.
	//	Default is true.
	//====================================================================
	virtual bool DoesLighting() const { return true; }
};

class matShaderEffect : public effShaderEffect
{
public:
	//====================================================================
	//====================================================================
	matShaderEffect();

	//====================================================================
	//====================================================================
	virtual ~matShaderEffect();

	enum Technique
	{
		e_Default = 0,
		e_SingleLight,
		e_ProjectedLight,
		e_ProjectedLightSuperSample,
		e_ProjectedLightSuperSample2,
		e_ProjectedLightSuperSample3,
		e_DOFPrep,
		e_SpecularGlow,
		e_Matte,
		e_Environment,
		e_NumTechniques
	};

	//====================================================================
	// Tell shader which technique to use
	//====================================================================
	virtual void SetTechnique(Technique i_Technique) const = 0;
	virtual void SetTechnique(const std::string& i_Technique) const = 0;

	//====================================================================
	// Set up standard matrix transforms
	//====================================================================
	virtual void SetupMatrices(const maMatrix4x4 &i_WorldMat,
							   const maMatrix4x4 &i_CameraMat,
							   const maMatrix4x4 &i_ProjMat,
							   const maPoint3d &i_CameraPos) const = 0;

	//====================================================================
	// Set up skinning matrix transforms
	//====================================================================
	virtual bool GetHasSkinning() const = 0;
	virtual void SetupSkinningMatrices(const std::vector<maMatrix4x4> &i_MatrixPalette) const = 0;

	//====================================================================
	// Set up shader for ambient environment lighting
	//====================================================================
	virtual void SetupAmbientPass(const g3dAmbientEnvState& i_AmbientEnvState) const = 0;

	virtual void SetupDOFPrep() const = 0;

	//====================================================================
	// Set up shader for lighting and material
	//====================================================================
	virtual void SetupLighting(const maAxisBox& i_BBox) const = 0;

	//--------------------------------------------------------------------
	// Set up shader for lighting and material given the material's 
	//	colors
	//--------------------------------------------------------------------
	virtual void SetupAmbientLighting(const maAxisBox& i_BBox) const = 0;

	//--------------------------------------------------------------------
	// Set up shader for lighting and material given the material's 
	//	colors
	//--------------------------------------------------------------------
	virtual void SetupSingleLight(const g3dLight* i_pLight,
		const g3dProjectedLight* i_pProjLight, bool i_bAllowShadows) const = 0;

	//====================================================================
	// Install shader-specific parameters from material description.
	//====================================================================
	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const = 0;
	virtual void SetupParams(const effShaderData* i_Data) const = 0;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void SetIsolateReflections(bool i_bIsolateReflections) const = 0;

	//====================================================================
	// Install shader parameters for ambient occlusion rendering.
	// These are per-fragment, and so are decoupled from the matMaterial shader data.
	//====================================================================
	//virtual void SetupAO(const effOcclusionData* i_Data) const = 0;

	//====================================================================
	// Install time value for shaders that use it.
	//====================================================================
	virtual void SetTime(float i_Time) const = 0;

	//====================================================================
	// Tell shaders to bake render to texture (using uvs for vertex positions)
	//====================================================================
	virtual void SetVertexUVBakeMode(bool i_bDoBaking) const = 0;

	//====================================================================
	// Some shaders support double sided geometry shading.
	//====================================================================
	virtual void SetIsDoubleSided(bool i_IsDoubleSided) const = 0;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void SetupGlowPass(const effGlowData& i_GlowData) const = 0;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void SetupOutlinePass(const effOutlineData& i_OutlineData) const = 0;

	//--------------------------------------------------------------------
	// set factors that rescale uv space to [0,0]..[1,1] for texture bake.
	//--------------------------------------------------------------------
	virtual void SetBakingFactors(const maPoint2d& i_Scale, const maPoint2d& i_Translate) const = 0;

	//--------------------------------------------------------------------
	// allocates mem.
	//--------------------------------------------------------------------
	virtual int BuildPrtyObject(effShaderParams* o_pParams) const = 0;

	//--------------------------------------------------------------------
	// does the shader hook into our dynamic reflection mapping?
	//--------------------------------------------------------------------
	virtual bool HasReflectionMap() const = 0;

	//--------------------------------------------------------------------
	// does the shader support hardware tessellation?
	//--------------------------------------------------------------------
	virtual bool HasHardwareTessellation() const = 0;

	//====================================================================
	// Set up the shader for Hardware tessellation using a texture to store extra mesh data.
	//====================================================================
	virtual void SetupTessellatorMeshTexture( const matTexture* i_pTexture ) = 0;

	//====================================================================
	// Set up the shader for displacement mapping parameters
	//====================================================================
	virtual void SetupDisplacementMap( const matTexture* i_pTexture, float i_Scale, float i_Bias, float i_Blur, const maVector2d& i_ObjUVScale ) = 0;

	//------------------------------------------------------------------------
	// function that will initialize UV transform
	//------------------------------------------------------------------------
	virtual void SetupUVTransform( const maMatrix4x4 &i_WorldMat ) = 0;

	//------------------------------------------------------------------------
	// function that will initialize the paint overlay texture for the shader
	//------------------------------------------------------------------------
	virtual void SetupPaintOverlay(matRenderTargetTexture* i_pRenderTexture) const = 0;

	//====================================================================
	// Set up the shader for displacement mapping parameters
	//====================================================================
	virtual void SetupNormalMap( const matTexture* i_pNormalMap, float i_BumpScale ) = 0;

	//====================================================================
	// Check if the shader supports outline
	//====================================================================
	virtual bool GetSupportOutline() const = 0;

	//------------------------------------------------------------------------
	// global function that Sets the Alpha Test reference value (Greater Equal Compare Fixed)
	// less than 0 allows all values through (disabled),
	// 0 cuts out black,
	// less than 1 cuts all but solid,
	// 1 or greater kills all drawing for normal alpha values
	//------------------------------------------------------------------------
	virtual void SetAlphaTestRef(float i_Value) const {}

	//--------------------------------------------------------------------
	// global function that set a user defined clip plane
	// Operates in world space and (0,0,0,1) disables clipping
	//--------------------------------------------------------------------
	virtual void SetClipPlane( const maVector4d& i_Plane ) const {}

	//--------------------------------------------------------------------
	// global function that set a the hair tessellation value
	//--------------------------------------------------------------------
//	virtual void SetHairTessellationValue( const maVector2d& i_Value ) const {}
};
