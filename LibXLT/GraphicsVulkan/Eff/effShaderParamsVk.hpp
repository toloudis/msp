/*****************************************************************************
**  effShaderParamsVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11/Eff/effShaderParamsDX11.hpp
**	(namespace gvk): the bindings that set a material's parameters on
**	its effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_EFF_SHADERPARAMSVK_HPP
#error effShaderParamsVk.hpp multiply included
#endif
#define GVK_EFF_SHADERPARAMSVK_HPP

#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/eff/effShaderParams.hpp"
#endif

#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif

#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif


#ifndef GVK_FXMATERIALINSTANCE_HPP
#include "GraphicsVulkan/Fx/fxMaterialInstance.hpp"
#endif

#include <memory>

namespace gvk
{

class effBindParamVk
{
public:
	virtual bool Bind(fxEffect* i_pShader) = 0;

	virtual const effShaderParam* GetParam() = 0;
};

class effFloatBindingVk : public effBindParamVk
{
public: 
	effFloatBindingVk(const effParamFloat& i_Param, fxEffectVariable* i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(fxEffect* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamFloat& m_Param;
	fxEffectVariable* m_Handle;
};
class effIntBindingVk : public effBindParamVk
{
public: 
	effIntBindingVk(const effParamInt& i_Param, fxEffectVariable* i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(fxEffect* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamInt& m_Param;
	fxEffectVariable* m_Handle;
};
class effBoolBindingVk : public effBindParamVk
{
public: 
	effBoolBindingVk(const effParamBool& i_Param, fxEffectVariable* i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(fxEffect* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamBool& m_Param;
	fxEffectVariable* m_Handle;
};
class effColorBindingVk : public effBindParamVk
{
public: 
	effColorBindingVk(const effParamColor& i_Param, fxEffectVariable* i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(fxEffect* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamColor& m_Param;
	fxEffectVariable* m_Handle;
};
class effTextureBindingVk : public effBindParamVk
{
public: 
	effTextureBindingVk(const effParamTexture& i_Param, 
		fxEffectVariable* i_Handle, fxEffectVariable* i_ExistVarHandle )
		: m_Param(i_Param), m_Handle(i_Handle), m_ExistVarHandle(i_ExistVarHandle)
	{
	}

	virtual bool Bind(fxEffect* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamTexture& m_Param;
	fxEffectVariable* m_Handle;
	fxEffectVariable* m_ExistVarHandle;
};

class effShaderBindingsVk : public effShaderBindings
{
public:
	effShaderBindingsVk(fxEffect* i_pEffect)
		: effShaderBindings(), m_pEffectD3D(i_pEffect)
	{
	}
	virtual ~effShaderBindingsVk();


	// actually set all variables to shader.
	virtual void Bind()
	{
		if (m_pMaterial)
			m_pMaterial->Restore();
		int n = m_BindableParams.size();
		for (int i = 0; i < n; i++)
		{
			m_BindableParams[i]->Bind(m_pEffectD3D);
		}
		if (m_pMaterial)
			m_pMaterial->Capture();
	};

	effBindParamVk* HasBinding(effShaderParam* i_Param)
	{
		int n = m_BindableParams.size();
		for (int i = 0; i < n; i++)
		{
			if (m_BindableParams[i]->GetParam() == i_Param)
				return m_BindableParams[i];
		}
		return NULL;
	}

	std::vector<effBindParamVk*> m_BindableParams;
	fxEffect* m_pEffectD3D;
	// This material's own parameter block, for a converted (plain-HLSL)
	// material; null for an Effects one, which writes into the effect.
	std::unique_ptr<fxMaterialInstance> m_pMaterial;
};
}
