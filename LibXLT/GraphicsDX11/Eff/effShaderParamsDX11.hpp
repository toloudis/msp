/*****************************************************************************
**  effShaderParamsDX11.hpp
**
**      effShaderParamsDX11
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_SHADERPARAMSDX11_HPP
#error effShaderParamsDX11.hpp multiply included
#endif
#define EFF_SHADERPARAMSDX11_HPP

#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/eff/effShaderParams.hpp"
#endif

#ifndef EFF_SHADERBASEDX11_HPP
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#endif

#ifndef G3D_DX11TEXTUREUTIL_HPP
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#endif

#ifndef G2D_DX11GLOBALWIN_HPP
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#endif

class effBindParamDX11
{
public:
	virtual bool Bind(ID3DX11Effect* i_pShader) = 0;

	virtual const effShaderParam* GetParam() = 0;
};

class effFloatBindingDX11 : public effBindParamDX11
{
public: 
	effFloatBindingDX11(const effParamFloat& i_Param, ID3DX11EffectScalarVariable* i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(ID3DX11Effect* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamFloat& m_Param;
	ID3DX11EffectScalarVariable* m_Handle;
};
class effIntBindingDX11 : public effBindParamDX11
{
public: 
	effIntBindingDX11(const effParamInt& i_Param, ID3DX11EffectScalarVariable* i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(ID3DX11Effect* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamInt& m_Param;
	ID3DX11EffectScalarVariable* m_Handle;
};
class effBoolBindingDX11 : public effBindParamDX11
{
public: 
	effBoolBindingDX11(const effParamBool& i_Param, ID3DX11EffectScalarVariable* i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(ID3DX11Effect* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamBool& m_Param;
	ID3DX11EffectScalarVariable* m_Handle;
};
class effColorBindingDX11 : public effBindParamDX11
{
public: 
	effColorBindingDX11(const effParamColor& i_Param, ID3DX11EffectVectorVariable* i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(ID3DX11Effect* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamColor& m_Param;
	ID3DX11EffectVectorVariable* m_Handle;
};
class effTextureBindingDX11 : public effBindParamDX11
{
public: 
	effTextureBindingDX11(const effParamTexture& i_Param, 
		ID3DX11EffectShaderResourceVariable* i_Handle, ID3DX11EffectScalarVariable* i_ExistVarHandle )
		: m_Param(i_Param), m_Handle(i_Handle), m_ExistVarHandle(i_ExistVarHandle)
	{
	}

	virtual bool Bind(ID3DX11Effect* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamTexture& m_Param;
	ID3DX11EffectShaderResourceVariable* m_Handle;
	ID3DX11EffectScalarVariable* m_ExistVarHandle;
};

class effShaderBindingsDX11 : public effShaderBindings
{
public:
	effShaderBindingsDX11(ID3DX11Effect* i_pEffect)
		: effShaderBindings(), m_pEffectD3D(i_pEffect)
	{
	}
	virtual ~effShaderBindingsDX11();


	// actually set all variables to shader.
	virtual void Bind()
	{
		int n = m_BindableParams.size();
		for (int i = 0; i < n; i++)
		{
			m_BindableParams[i]->Bind(m_pEffectD3D);
		}
	};

	effBindParamDX11* HasBinding(effShaderParam* i_Param)
	{
		int n = m_BindableParams.size();
		for (int i = 0; i < n; i++)
		{
			if (m_BindableParams[i]->GetParam() == i_Param)
				return m_BindableParams[i];
		}
		return NULL;
	}

	std::vector<effBindParamDX11*> m_BindableParams;
	ID3DX11Effect* m_pEffectD3D;
};

