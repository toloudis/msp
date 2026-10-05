/*****************************************************************************
**  effPlainShaderDX11.hpp
**
**      effPlainShaderDX11 implements matShaderEffect on top of an
**      fxEffectDX11, i.e. a plain-HLSL effect with no D3DX11 Effects
**      underneath. Effects that have been converted from .fx derive from
**      this instead of effShaderBaseDX11.
**
**      Only what post-processing effects need is implemented so far:
**      techniques and passes, and parameters by index. The material and
**      lighting setup calls do nothing; they get filled in as material
**      shaders are converted.
\****************************************************************************/

#ifdef EFF_PLAINSHADERDX11_HPP
#error effPlainShaderDX11.hpp multiply included
#endif
#define EFF_PLAINSHADERDX11_HPP

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#ifndef FX_EFFECTDX11_HPP
#include "GraphicsDX11/Fx/fxEffectDX11.hpp"
#endif

#include <memory>
#include <string>
#include <vector>

class effPlainShaderDX11 : public matShaderEffect
{
public:
	effPlainShaderDX11(std::unique_ptr<fxEffectDX11> i_pEffect, std::string i_Name);
	virtual ~effPlainShaderDX11();

	fxEffectDX11* GetEffect() const { return m_pEffect.get(); }

	// techniques and passes
	virtual void SetTechnique(Technique i_Technique) const;
	virtual void SetTechnique(const std::string& i_Technique) const;
	virtual int Begin() const;
	virtual void BeginPass(int i_Pass) const;
	virtual void EndPass() const;
	virtual void End() const;

	// parameters by index
	virtual int GetParamIndex(const std::string& i_name) const;
	virtual bool GetParamUI(const std::string& i_name, matShaderParamUI& o_paramUI) const;
	virtual void GetAllParamUIs(std::list<matShaderParamUI>& o_paramUI) const;
	virtual void SetMatrix(int i_param, const maMatrix4x4& i_matrix);
	virtual void SetFloat(int i_param, float i_float);
	virtual void SetVector(int i_param, const maVector4d& i_vector);
	virtual void SetData(int i_param, void* i_data, unsigned int i_nbytes);
	virtual void SetBool(int i_param, bool i_bool);
	virtual void SetString(int i_param, std::string i_string);
	virtual void SetTexture(int i_param, const matTexture* i_pTexture) const;

	virtual std::string GetName() const { return m_Name; }
	virtual void SetName(std::string i_Name) { m_Name = i_Name; }

	// material and lighting setup: not used by converted effects yet
	virtual void SetupMatrices(const maMatrix4x4 &i_WorldMat,
		const maMatrix4x4 &i_CameraMat,
		const maMatrix4x4 &i_ProjMat,
		const maPoint3d &i_CameraPos) const {}
	virtual bool GetHasSkinning() const { return false; }
	virtual void SetupSkinningMatrices(const std::vector<maMatrix4x4> &i_MatrixPalette) const {}
	virtual void SetupAmbientPass(const g3dAmbientEnvState& i_AmbientEnvState) const {}
	virtual void SetupDOFPrep() const {}
	virtual void SetupLighting(const maAxisBox& i_BBox) const {}
	virtual void SetupAmbientLighting(const maAxisBox& i_BBox) const {}
	virtual void SetupSingleLight(const g3dLight* i_pLight,
		const g3dProjectedLight* i_pProjLight, bool i_bAllowShadows) const {}
	virtual void SetupMaterial(const matMaterial* i_Material,
		int i_MaterialLayerIndex = 0) const {}
	virtual void SetupParams(const effShaderData* i_Data) const {}
	virtual void SetIsolateReflections(bool i_bIsolateReflections) const {}
	virtual void SetTime(float i_Time) const {}
	virtual void SetVertexUVBakeMode(bool i_bDoBaking) const {}
	virtual void SetIsDoubleSided(bool i_IsDoubleSided) const {}
	virtual void SetupGlowPass(const effGlowData& i_GlowData) const {}
	virtual void SetupOutlinePass(const effOutlineData& i_OutlineData) const {}
	virtual void SetBakingFactors(const maPoint2d& i_Scale, const maPoint2d& i_Translate) const {}
	virtual int BuildPrtyObject(effShaderParams* o_pParams) const { return 0; }
	virtual bool HasReflectionMap() const { return false; }
	virtual bool HasHardwareTessellation() const { return false; }
	virtual void SetupTessellatorMeshTexture(const matTexture* i_pTexture) {}
	virtual void SetupDisplacementMap(const matTexture* i_pTexture, float i_Scale, float i_Bias,
		float i_Blur, const maVector2d& i_ObjUVScale) {}
	virtual void SetupUVTransform(const maMatrix4x4 &i_WorldMat) {}
	virtual void SetupPaintOverlay(matRenderTargetTexture* i_pRenderTexture) const {}
	virtual void SetupNormalMap(const matTexture* i_pNormalMap, float i_BumpScale) {}
	virtual bool GetSupportOutline() const { return false; }

protected:
	std::unique_ptr<fxEffectDX11> m_pEffect;
	std::string m_Name;
	mutable int m_CurrentTechnique;

private:
	int m_TechniqueByEnum[e_NumTechniques];

	struct Param
	{
		std::string m_Name;
		fxEffectDX11::Constant m_Constant;
		fxEffectDX11::Resource m_Resource;
	};
	mutable std::vector<Param> m_Params;
};
