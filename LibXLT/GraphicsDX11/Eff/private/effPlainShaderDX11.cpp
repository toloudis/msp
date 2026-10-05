/*****************************************************************************
**  effPlainShaderDX11.cpp
**
**      matShaderEffect on top of a plain-HLSL fxEffectDX11.
\****************************************************************************/

#include "GraphicsDX11/eff/effPlainShaderDX11.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"

#include <string.h>

namespace
{
	// technique names the material techniques are looked up by, as in
	// effShaderBaseDX11::parse_techniques()
	const char* k_TechniqueNames[matShaderEffect::e_NumTechniques] =
	{
		"default",
		"singlelight",
		"projectedlight",
		"projectedlightsupersample",
		"projectedlightsupersample2",
		"projectedlightsupersample3",
		"dofprep",
		"glow",
		"matte",
		"environment",
	};
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
effPlainShaderDX11::effPlainShaderDX11(std::unique_ptr<fxEffectDX11> i_pEffect, std::string i_Name)
:	m_pEffect(std::move(i_pEffect)),
	m_Name(i_Name),
	m_CurrentTechnique(0)
{
	const std::vector<fxTechniqueDesc>& techniques = m_pEffect->GetDesc().m_Techniques;
	for (int t = 0; t < e_NumTechniques; t++)
	{
		m_TechniqueByEnum[t] = -1;
		for (size_t i = 0; i < techniques.size(); i++)
		{
			if (_stricmp(techniques[i].m_Name.c_str(), k_TechniqueNames[t]) == 0)
			{
				m_TechniqueByEnum[t] = (int)i;
				break;
			}
		}
	}
	// without a technique named Default, the first one is the default
	if (m_TechniqueByEnum[e_Default] < 0)
		m_TechniqueByEnum[e_Default] = 0;
}

effPlainShaderDX11::~effPlainShaderDX11()
{
}

//------------------------------------------------------------------------
// Techniques and passes
//------------------------------------------------------------------------
void effPlainShaderDX11::SetTechnique(Technique i_Technique) const
{
	int t = m_TechniqueByEnum[i_Technique];
	m_CurrentTechnique = (t >= 0) ? t : m_TechniqueByEnum[e_Default];
}

void effPlainShaderDX11::SetTechnique(const std::string& i_Technique) const
{
	m_CurrentTechnique = m_pEffect->FindTechnique(i_Technique);
	if (m_CurrentTechnique < 0)
		DBG_WARNING("Technique " << i_Technique << " not found in " << m_Name);
}

int effPlainShaderDX11::Begin() const
{
	return m_pEffect->GetPassCount(m_CurrentTechnique);
}

void effPlainShaderDX11::BeginPass(int i_Pass) const
{
	m_pEffect->Apply(m_CurrentTechnique, i_Pass, g2dDX11Global::g_pDeviceContext);
}

void effPlainShaderDX11::EndPass() const
{
}

void effPlainShaderDX11::End() const
{
}

//------------------------------------------------------------------------
// Parameters by index
//------------------------------------------------------------------------
int effPlainShaderDX11::GetParamIndex(const std::string& i_name) const
{
	for (size_t i = 0; i < m_Params.size(); i++)
		if (m_Params[i].m_Name == i_name)
			return (int)i;

	Param p;
	p.m_Name = i_name;
	p.m_Constant = m_pEffect->FindConstant(i_name);
	p.m_Resource = m_pEffect->FindResource(i_name);
	if (!p.m_Constant.IsValid() && !p.m_Resource.IsValid())
		return -1;
	m_Params.push_back(p);
	return (int)m_Params.size() - 1;
}

bool effPlainShaderDX11::GetParamUI(const std::string& i_name, matShaderParamUI& o_paramUI) const
{
	return false;
}

void effPlainShaderDX11::GetAllParamUIs(std::list<matShaderParamUI>& o_paramUI) const
{
}

void effPlainShaderDX11::SetMatrix(int i_param, const maMatrix4x4& i_matrix)
{
	if (i_param >= 0 && i_param < (int)m_Params.size())
		m_pEffect->SetMatrix(m_Params[i_param].m_Constant, i_matrix.Ptr());
}

void effPlainShaderDX11::SetFloat(int i_param, float i_float)
{
	if (i_param >= 0 && i_param < (int)m_Params.size())
		m_pEffect->SetConstant(m_Params[i_param].m_Constant, i_float);
}

void effPlainShaderDX11::SetVector(int i_param, const maVector4d& i_vector)
{
	if (i_param >= 0 && i_param < (int)m_Params.size())
		m_pEffect->SetConstant(m_Params[i_param].m_Constant, i_vector.Ptr(), 4 * sizeof(float));
}

void effPlainShaderDX11::SetData(int i_param, void* i_data, unsigned int i_nbytes)
{
	if (i_param >= 0 && i_param < (int)m_Params.size())
		m_pEffect->SetConstant(m_Params[i_param].m_Constant, i_data, i_nbytes);
}

void effPlainShaderDX11::SetBool(int i_param, bool i_bool)
{
	// an HLSL bool in a constant buffer is 4 bytes
	int value = i_bool ? 1 : 0;
	if (i_param >= 0 && i_param < (int)m_Params.size())
		m_pEffect->SetConstant(m_Params[i_param].m_Constant, value);
}

void effPlainShaderDX11::SetString(int i_param, std::string i_string)
{
	// plain HLSL has no string variables
}

void effPlainShaderDX11::SetTexture(int i_param, const matTexture* i_pTexture) const
{
	if (i_param >= 0 && i_param < (int)m_Params.size())
		m_pEffect->SetResource(m_Params[i_param].m_Resource, g3dDX11TextureUtil::GetD3DTexture(i_pTexture));
}
