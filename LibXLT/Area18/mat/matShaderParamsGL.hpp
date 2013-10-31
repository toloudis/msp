/*****************************************************************************
**  matShaderParamsGL.hpp
**
**      matShaderParamsGL
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_SHADERPARAMSGL_HPP
#error matShaderParamsGL.hpp multiply included
#endif
#define MAT_SHADERPARAMSGL_HPP

#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/eff/effShaderParams.hpp"
#endif

#include "Area18/ogl/oglTypes.hpp"

class shdrPipeline;

class matBindParamGL
{
public:
	virtual bool Bind(shdrPipeline* i_pShader) = 0;

	virtual const effShaderParam* GetParam() = 0;
};

class matFloatBindingGL : public matBindParamGL
{
public: 
	matFloatBindingGL(const effParamFloat& i_Param, GLint i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(shdrPipeline* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamFloat& m_Param;
	GLint m_Handle;
};
class matIntBindingGL : public matBindParamGL
{
public: 
	matIntBindingGL(const effParamInt& i_Param, GLint i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(shdrPipeline* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamInt& m_Param;
	GLint m_Handle;
};
class matBoolBindingGL : public matBindParamGL
{
public: 
	matBoolBindingGL(const effParamBool& i_Param, GLint i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(shdrPipeline* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamBool& m_Param;
	GLint m_Handle;
};
class matColorBindingGL : public matBindParamGL
{
public: 
	matColorBindingGL(const effParamColor& i_Param, GLint i_Handle)
		: m_Param(i_Param), m_Handle(i_Handle)
	{
	}

	virtual bool Bind(shdrPipeline* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamColor& m_Param;
	GLint m_Handle;
};
class matTextureBindingGL : public matBindParamGL
{
public: 
	matTextureBindingGL(const effParamTexture& i_Param, 
		GLint i_Handle, GLint i_ExistVarHandle )
		: m_Param(i_Param), m_Handle(i_Handle), m_ExistVarHandle(i_ExistVarHandle)
	{
	}

	virtual bool Bind(shdrPipeline* i_pShader);

	virtual const effShaderParam* GetParam() {return &m_Param;}

	const effParamTexture& m_Param;
	GLint m_Handle;
	GLint m_ExistVarHandle;
};

class matShaderBindingsGL : public effShaderBindings
{
public:
	matShaderBindingsGL(shdrPipeline* i_pEffect)
		: effShaderBindings(), mEffectGL(i_pEffect)
	{
	}
	virtual ~matShaderBindingsGL();


	// actually set all variables to shader.
	virtual void Bind()
	{
		int n = m_BindableParams.size();
		for (int i = 0; i < n; i++)
		{
			m_BindableParams[i]->Bind(mEffectGL);
		}
	};

	matBindParamGL* HasBinding(effShaderParam* i_Param)
	{
		int n = m_BindableParams.size();
		for (int i = 0; i < n; i++)
		{
			if (m_BindableParams[i]->GetParam() == i_Param)
				return m_BindableParams[i];
		}
		return NULL;
	}

	std::vector<matBindParamGL*> m_BindableParams;
	shdrPipeline* mEffectGL;
};

