#ifdef MAT_SHADERBASEGL_HPP
#error matShaderBaseGL.hpp multiply included
#endif
#define MAT_SHADERBASEGL_HPP

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include "Area18/ogl/oglTypes.hpp"

#include "Area18/tinyxml2.h"
using namespace tinyxml2;

#include <list>
#include <map>

class effGlowData;
class effNormalsData;
class effOutlineData;
class effOcclusionData;
struct g3dAmbientEnvState;
class effShaderBindings;
class matShaderBindingsGL;
class effShaderParams;
class effParamBool;
class effParamColor;
class effParamFloat;
class effParamInt;
class effParamTexture;
class effParamTextureManip;
class prtyPropertyUIInfo;
class matRenderTargetTexture;
class shdrPipeline;
class matShaderBaseGL : public matShaderEffect
{
public:

	//====================================================================
	//====================================================================
	matShaderBaseGL(const fsLocator& i_Directory, shdrPipeline* i_pEffect, 
		std::string i_name);

	//====================================================================
	//====================================================================
	virtual ~matShaderBaseGL();

	//====================================================================
	// Start effect, returns number of passes needed
	//====================================================================
	virtual int Begin() const;

	//====================================================================
	// Set up values for this pass
	//====================================================================
	virtual void BeginPass(int i_Pass) const;

	//====================================================================
	// Finish this pass, restoring states
	//====================================================================
	virtual void EndPass() const;

	//====================================================================
	// End effect, call even if number of passes is 0
	//====================================================================
	virtual void End() const;

	//====================================================================
	// Set Texture into effect
	//====================================================================
	virtual void SetTexture(int i_Layer, const matTexture* i_pTexture) const;

	//====================================================================
	// Start effect, returns number of passes needed
	//====================================================================
	virtual void SetTechnique(Technique i_Technique) const;
	virtual void SetTechnique(const std::string& i_Technique) const;

	//====================================================================
	// Map from name of param to (faster) int lookup index
	//====================================================================
	virtual int GetParamIndex(const std::string& i_name) const;

	//====================================================================
	// Find named param, and fill the ui info struct
	//====================================================================
	virtual bool GetParamUI(const std::string& i_name, matShaderParamUI& o_paramUI) const;

	//====================================================================
	// Find all named params with ui info
	//====================================================================
	virtual void GetAllParamUIs(std::list<matShaderParamUI>& o_paramUI) const;

	//====================================================================
	// Set a matrix param
	//====================================================================
	virtual void SetMatrix(int i_param, const maMatrix4x4& i_matrix);

	//====================================================================
	// Set a float param
	//====================================================================
	virtual void SetFloat(int i_param, float i_float);

	//====================================================================
	// Set a vector param
	//====================================================================
	virtual void SetVector(int i_param, const maVector4d& i_vector);

	//====================================================================
	// Set a struct param
	//====================================================================
	virtual void SetData(int i_param, void* i_data, unsigned int i_nbytes);

	//====================================================================
	// Set a bool param
	//====================================================================
	virtual void SetBool(int i_param, bool i_bool);

	//====================================================================
	// Set a string param
	//====================================================================
	virtual void SetString(int i_param, std::string i_string);

	//====================================================================
	// Set up shader for lighting and material given the material's 
	//	colors
	//====================================================================
	virtual void SetupLighting(const maAxisBox& i_BBox) const;

	//--------------------------------------------------------------------
	// Set up shader for lighting and material given the material's 
	//	colors
	//--------------------------------------------------------------------
	virtual void SetupAmbientLighting(const maAxisBox& i_BBox) const;

	//--------------------------------------------------------------------
	// Set up shader for lighting and material given the material's 
	//	colors
	//--------------------------------------------------------------------
	virtual void SetupSingleLight(const g3dLight* i_pLight,
		const g3dProjectedLight* i_pProjLight, bool i_bAllowShadows) const;

	//====================================================================
	// Install shader-specific parameters from material description.
	//====================================================================
	virtual void SetupMaterial(const matMaterial* i_Material,
							   int i_MaterialLayerIndex = 0) const;
	virtual void SetupParams(const effShaderData* i_Data) const;

	virtual std::string GetName() const {return m_Name;}
	virtual void SetName(std::string i_Name) {m_Name = i_Name;}

	// let client see the effect if they know about cg/opengl.
	shdrPipeline* GetEffect() const;
	//const D3DX11_EFFECT_DESC& EffectDesc() const {return m_EffectDesc;}

	enum EffStandardMatrices
	{
		e_ObjToWorld,		// world
		e_ObjToWorldIT,		// worldIT
		e_WorldToView,		// view
		e_WorldToViewIT,	// viewIT
		e_ViewToProj,		// proj
		e_ObjToView,		// world*view
		e_ObjToViewIT,		// (world*view)IT
		e_WorldToProj,		// view*proj
		e_ObjToProj,		// world*view*proj
		e_NumMatrices
	};

	//====================================================================
	// Set up standard matrix transforms
	//====================================================================
	virtual void SetupMatrices(const maMatrix4x4 &i_WorldMat,
		const maMatrix4x4 &i_CameraMat,
		const maMatrix4x4 &i_ProjMat,
		const maPoint3d &i_CameraPos) const;

	//====================================================================
	// Set up skinning matrix transforms
	//====================================================================
	virtual bool GetHasSkinning() const;
	virtual void SetupSkinningMatrices(const std::vector<maMatrix4x4> &i_MatrixPalette) const;

	virtual void SetTime(float i_Time) const;
	virtual void SetVertexUVBakeMode(bool i_bDoBaking) const;

	//====================================================================
	// Set up shader for ambient environment lighting
	//====================================================================
	virtual void SetupAmbientPass(const g3dAmbientEnvState& i_AmbientEnvState) const;
	virtual void SetupDOFPrep() const;
	virtual void SetIsDoubleSided(bool i_IsDoubleSided) const;
	virtual void SetupGlowPass(const effGlowData& i_GlowData) const;
	virtual void SetupOutlinePass(const effOutlineData& i_OutlineData) const;

	//--------------------------------------------------------------------
	// set factors that rescale uv space to [0,0]..[1,1] for texture bake.
	//--------------------------------------------------------------------
	virtual void SetBakingFactors(const maPoint2d& i_Scale, const maPoint2d& i_Translate) const;

	//--------------------------------------------------------------------
	// allocates mem. automatically creates bindings as well.
	//--------------------------------------------------------------------
	virtual int BuildPrtyObject(effShaderParams* o_pParams) const;

	//--------------------------------------------------------------------
	// bindings will be owned by the effShaderParams
	//--------------------------------------------------------------------
//	virtual void CreateBindings(effShaderParams* io_Params);

	void Bind(effShaderBindings* i_pBindings) const;

	//--------------------------------------------------------------------
	// does the shader hook into our dynamic reflection mapping?
	//--------------------------------------------------------------------
	virtual bool HasReflectionMap() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void SetIsolateReflections(bool i_bIsolateReflections) const;

	//--------------------------------------------------------------------
	// does the shader support hardware tessellation?
	//--------------------------------------------------------------------
	virtual bool HasHardwareTessellation() const;

	//====================================================================
	// Set up the shader for Hardware tessellation using a texture to store extra mesh data.
	//====================================================================
	virtual void SetupTessellatorMeshTexture( const matTexture* i_pTexture );

	//====================================================================
	// Set up the shader for Displacement map parameters.
	//====================================================================
	virtual void SetupDisplacementMap( const matTexture* i_pTexture, float i_Scale, float i_Bias, float i_Blur, const maVector2d& i_ObjUVScale );

	virtual bool HasDisplacementMap() const;

	//--------------------------------------------------------------------
	// set the global UV transform matrix
	//--------------------------------------------------------------------
	virtual void SetupUVTransform( const maMatrix4x4 &i_WorldMat );

	//====================================================================
	// Set up the shader for displacement mapping parameters
	//====================================================================
	virtual void SetupNormalMap( const matTexture* i_pNormalMap, float i_BumpScale );

	//====================================================================
	// Check if the shader supports outline
	//====================================================================
	virtual bool GetSupportOutline() const;

	//--------------------------------------------------------------------
	// return null if not found by name. otherwise add to o_params
	//--------------------------------------------------------------------
	effParamTexture* MapTextureParam(effShaderParams* o_pParams,
		const std::string& i_Name) const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetTessellateValue( float i_Value );

	//------------------------------------------------------------------------
	// function that will initialize the paint overlay texture for the shader
	//------------------------------------------------------------------------
	void SetupPaintOverlay(matRenderTargetTexture* i_pRenderTexture) const;

	//------------------------------------------------------------------------
	// global function that Sets the Alpha Test reference value (Greater Equal Compare Fixed)
	// less than 0 allows all values through (disabled),
	// 0 cuts out black,
	// less than 1 cuts all but solid,
	// 1 or greater kills all drawing for normal alpha values
	//------------------------------------------------------------------------
	virtual void SetAlphaTestRef(float i_Value) const;

	//--------------------------------------------------------------------
	// global function that set a user defined clip plane
	// Operates in world space and (0,0,0,1) disables clipping
	//--------------------------------------------------------------------
	virtual void SetClipPlane( const maVector4d& i_Plane ) const;


	//--------------------------------------------------------------------
	// GetImportantLight() gets the first eight important lights by
	// given axis box
	//--------------------------------------------------------------------
	void SetImportantLight(int i_num, const maAxisBox& i_BBox) const;

	//--------------------------------------------------------------------
	// global function that set a the hair tessellation value
	//--------------------------------------------------------------------
//	virtual void SetHairTessellationValue( const maVector2d& i_Value ) const;

protected:
	std::string m_Name;
	shdrPipeline* m_pEffect;

	// context-specific pipeline object
	// mutable for lazy init in Begin()
	mutable shdrPipeline* mContextPipeline;
private:

	//====================================================================
	// Get handles to parameters we recognize
	//====================================================================
	void parse_parameters(const fsLocator& i_Directory);

	//====================================================================
	// Get handles to techniques we recognize
	//====================================================================
	void parse_techniques();

	std::vector<GLint> m_params;
	std::map<std::string, int> m_paramnamemap;

	std::list<matTexture*> m_OwnedTextures;

	// redundant storage... these are in m_params too
	GLint m_stdMatrices[e_NumMatrices];

	GLint m_Techniques[matShaderEffect::e_NumTechniques];
	int m_NumTechniquePasses[matShaderEffect::e_NumTechniques];

protected:
	GLint m_UVTransformHandle;

	GLint m_FirstLightHandle;
	GLint m_LightInfoHandle;
	GLint m_ProjLightInfoHandle;
	GLint m_ProjLightTextureHandle;
	GLint m_ProjShadowMapHandle;
	GLint m_HasProjectedTextureHandle;
	GLint m_HasShadowMapHandle;

	void MapParameter(std::string i_Name, GLint& o_Handle);

	GLint m_TessellatorMeshTextureHandle;
	GLint m_hMeshDataTextureWidthHandle;
	GLint m_hMeshDataTextureHeightHandle;

private:
	GLint m_VecCameraPosHandle;

	GLint m_LightArrayHandle;
	GLint m_LightArrayNumHandle;

	GLint m_TimeHandle;
	GLint m_IsProjLtHandle;
	GLint m_IsDoubleSidedHandle;

	// ambient environment pass
//	ID3DX11EffectConstantBuffer* m_Env;
	GLint m_EnvHasDiffuseMapHandle;
	GLint m_EnvDiffuseMapHandle;
	GLint m_EnvDiffuseAngleHandle;
	GLint m_EnvDiffuseFactorHandle;
	GLint m_EnvDiffuseColorHandle;
	GLint m_EnvHasSpecularMapHandle;
	GLint m_EnvSpecularMapHandle;
	GLint m_EnvSpecularAngleHandle;
	GLint m_EnvSpecularFactorHandle;
	GLint m_EnvSpecularColorHandle;

	// depth of field
	GLint m_DOFHandle;
	GLint m_DOFBlurCutoffHandle;
	
	// glow pass info
	GLint m_hHasGlowMask;
	GLint m_hGlowMask;
	GLint m_hConstantGlow;
	GLint m_hGlowSize;

	// outline pass info
	GLint m_hOutlineDepthScale;
	GLint m_hOutlineMinAngle;
	GLint m_hOutlineMaxAngle;
	GLint m_hOutlineThickness;
	GLint m_hOutlineColor;
	GLint m_hOutlineViewSize;
	GLint m_hUseNormals;
	GLint m_hOutlineMinWidth;
	GLint m_hOutlineMaxWidth;

	// vertex uv bake mode
	GLint m_hBake;
	GLint m_hBakingTransform;

	// reflection pass rendering
	GLint m_hIsolateReflections;
	GLint m_hCubeMapEnabled;

	// reflection mapping
	GLint m_ReflectionMapIsPlanarHandle;
	GLint m_HasReflectionMapHandle;
	GLint m_CubeReflectionMapHandle;
	GLint m_PlanarReflectionMapHandle;
	GLint m_IsReflectionGenHandle;

	// skinning
	GLint m_SkinningMatrixPaletteHandle;

	// Hardware Tessellation
	GLint m_hHardwareTessellationHandle;	//only valid if the shader supports hardware tessellation
	GLint m_hTessValueHandle;

	// Displacement Mapping
	GLint m_hHasDisplacementMap;
	GLint m_hDisplacementMap;
	GLint m_hDisplacementScale;
	GLint m_hDisplacementBias;
	GLint m_hDisplacementBlur;
	GLint m_hDisplacementObjUVScale;

	GLint m_hNormalMap;
	GLint m_hBumpScale;
	GLint m_hHasNormalMap;

	// Paint Overlay
	GLint m_PaintOverlayMapHandle;

	// Alpha test Reference value
	GLint m_AlphaTestRefHandle;

	// User defined clip plane (world space) (0,0,0,1) = disabled
	GLint m_ClipPlaneHandle;

	//Hair Tessellation
//	GLint m_HairTessellationHandle;

	struct ShaderParamUIInfo
	{
		int Index;
		prtyPropertyUIInfo* UIInfo;
		bool operator < ( const ShaderParamUIInfo& i_Other )
		{
			return Index < i_Other.Index;
		}
	};

	bool GetShaderParamInfo(const tinyxml2::XMLElement* iMetaData, 
										  effShaderParams* o_pParams,
										  matShaderBindingsGL* o_pBindings,
										  std::list<ShaderParamUIInfo>& o_UIInfo) const;

	effParamTexture* MapTextureParam(effShaderParams* o_pParams,
		matShaderBindingsGL* o_pBindings,
		GLint i_hParam, 
		const XMLElement* param,
		const std::string& i_Name,
		bool i_bCreateBinding,
		bool i_bCreateUI,
		std::list<ShaderParamUIInfo>& o_UIInfo) const;
	effParamTextureManip* MapTextureManipParam(effShaderParams* o_pParams,
		matShaderBindingsGL* o_pBindings,
		GLint i_hParam, 
		const XMLElement* param,
		const std::string& i_Name,
		bool i_bCreateBinding,
		bool i_bCreateUI,
		std::list<ShaderParamUIInfo>& o_UIInfo) const;
	effParamFloat* MapFloatParam(effShaderParams* o_pParams,
		matShaderBindingsGL* o_pBindings,
		GLint i_hParam, 
		const XMLElement* param,
		const std::string& i_Name,
		bool i_bCreateBinding,
		bool i_bCreateUI,
		std::list<ShaderParamUIInfo>& o_UIInfo) const;
	effParamBool* MapBoolParam(effShaderParams* o_pParams,
		matShaderBindingsGL* o_pBindings,
		GLint i_hParam, 
		const XMLElement* param,
		const std::string& i_Name,
		bool i_bCreateBinding,
		bool i_bCreateUI,
		std::list<ShaderParamUIInfo>& o_UIInfo) const;
	effParamInt* MapEnumParam(effShaderParams* o_pParams,
		matShaderBindingsGL* o_pBindings,
		GLint i_hParam, 
		const XMLElement* param,
		const std::string& i_Name,
		bool i_bCreateBinding,
		bool i_bCreateUI,
		std::list<ShaderParamUIInfo>& o_UIInfo) const;
	effParamColor* MapColorParam(effShaderParams* o_pParams,
		matShaderBindingsGL* o_pBindings,
		GLint i_hParam, 
		const XMLElement* param,
		const std::string& i_Name,
		bool i_bCreateBinding,
		bool i_bCreateUI,
		std::list<ShaderParamUIInfo>& o_UIInfo) const;
	
	void SetupReflectionMap(bool i_bIsPlanar, matTexture* i_pReflectionMap) const;
	
	int FindShaderVersion();

	// one set of default shader params for each shader.
	effShaderParams* m_pDefaults;

	//====================================================================
	// Set up shader for lighting through a projected texture light.
	//====================================================================
	void SetupProjectedLight(const matTexture* i_pTexture,
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
									 float i_Aspect) const;
	void SetIsProjLight(bool i_IsProjLight) const;
};
