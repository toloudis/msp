/*****************************************************************************
**  fxEffectDesc.hpp
**
**      fxEffectDesc is the API-neutral description of a plain-HLSL effect,
**      read from its .effect.json manifest (see Tools/fx2hlsl): techniques
**      made of passes, each pass naming one entry point per pipeline stage,
**      plus sampler states and variable defaults.
**
**      Nothing here refers to a graphics API, so the same description can
**      drive the D3D11 backend (fxEffectDX11) and a later D3D12 or Vulkan one.
\****************************************************************************/

#ifdef FX_EFFECTDESC_HPP
#error fxEffectDesc.hpp multiply included
#endif
#define FX_EFFECTDESC_HPP

#include <string>
#include <vector>

//============================================================================
// Pipeline stages, in the order a pass lists them
//============================================================================
enum fxStage
{
	fx_VS = 0,
	fx_HS,
	fx_DS,
	fx_GS,
	fx_PS,
	fx_NumGraphicsStages,

	fx_CS = fx_NumGraphicsStages,
	fx_NumStages
};

//============================================================================
//============================================================================
struct fxStageDesc
{
	std::string m_Entry;		// entry point in the .hlsl; empty = no shader
	std::string m_Profile;		// e.g. ps_5_0

	bool IsUsed() const { return !m_Entry.empty(); }
};

struct fxPassDesc
{
	std::string m_Name;
	fxStageDesc m_Stages[fx_NumStages];
};

struct fxTechniqueDesc
{
	std::string m_Name;
	std::vector<fxPassDesc> m_Passes;
};

//============================================================================
// Sampler state, using the D3D state names as written in the original .fx
// (e.g. Filter = "MIN_MAG_LINEAR_MIP_POINT", Address = "CLAMP"). Unset
// fields keep the D3D defaults, which is what Effects used.
//============================================================================
struct fxSamplerDesc
{
	std::string m_Name;
	std::string m_Filter = "MIN_MAG_MIP_LINEAR";
	std::string m_Address[3] = { "CLAMP", "CLAMP", "CLAMP" };
	float m_MipLODBias = 0.0f;
	int m_MaxAnisotropy = 1;
	std::string m_ComparisonFunc = "NEVER";
	float m_BorderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float m_MinLOD = -3.402823466e+38f;
	float m_MaxLOD = 3.402823466e+38f;
};

//============================================================================
// A global variable. Plain HLSL ignores initializers on constant-buffer
// variables, so the default value the .fx declared is applied by the runtime.
//============================================================================
struct fxVariableDesc
{
	std::string m_Name;
	std::string m_Type;
	std::string m_Semantic;
	std::vector<double> m_Default;		// flattened components; empty = none
	int m_DefaultComponentsPerElement = 0;	// > 0 for array defaults
};

//============================================================================
//============================================================================
struct fxEffectDesc
{
	std::string m_Source;		// original .fx, relative to the shader root
	std::string m_Hlsl;			// converted .hlsl file name
	std::vector<fxTechniqueDesc> m_Techniques;
	std::vector<fxSamplerDesc> m_Samplers;
	std::vector<fxVariableDesc> m_Variables;

	// Parse an .effect.json manifest. Returns false and sets o_Error on failure.
	static bool Parse(const char* i_Json, fxEffectDesc& o_Desc, std::string& o_Error);

	const fxSamplerDesc* FindSampler(const std::string& i_Name) const;
	const fxVariableDesc* FindVariable(const std::string& i_Name) const;
};
