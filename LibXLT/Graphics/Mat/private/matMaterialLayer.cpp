/*****************************************************************************
**  matMaterialLayer.hpp
**
**      The matMaterialLayer represents one layer of a multi-layered 
**	material. This corresponds to a single effects shader type and
**	so controls for how to blend this layer with the other layers.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mat/matMaterialLayer.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	bool test_has_transformation(const maVector2d& i_Translation, 
								 const maVector2d& i_Scale, 
								 float i_Rotation)
	{
		if( !(i_Translation == maVector2d(0, 0)) ) return true;
		if( !(i_Scale == maVector2d(1, 1)) ) return true;
		if( i_Rotation != 0.0f ) return true;
		return false;
	}
}

//--------------------------------------------------------------------
//	Default constructor
//--------------------------------------------------------------------
matMaterialLayer::matMaterialLayer()
{
	m_Flags.m_bHasSpecular = true;
	m_Flags.m_bAdditive = false;

	m_pShaderData = NULL;
	m_EffectID = "Phong.fx";
	SetShaderEffect("Phong.fx");
	m_pShaderData = new effPhongData;

	m_Flags.m_bForceTransparent = false;
	this->test_transparency();
}

//--------------------------------------------------------------------
//	convenience constructor that will allow you to set the initial color
//	values
//--------------------------------------------------------------------
matMaterialLayer::matMaterialLayer(const maFloatRGBA& i_Diffuse,
						 const maFloatRGBA& i_Ambient,
						 const maFloatRGBA& i_Emissive)
{
	m_Flags.m_bHasSpecular = false;
	m_Flags.m_bAdditive = false;

	m_pShaderData = NULL;
	m_EffectID = "Phong.fx";
	SetShaderEffect("Phong.fx");
	m_pShaderData = new effPhongData;
	((effPhongData*)m_pShaderData)->m_ColorDiffuse = i_Diffuse;
	((effPhongData*)m_pShaderData)->m_ColorAmbient = i_Ambient;
	((effPhongData*)m_pShaderData)->m_ColorEmissive = i_Emissive;
	((effPhongData*)m_pShaderData)->m_Transparency = i_Diffuse.GetAlpha();

	m_Flags.m_bForceTransparent = false;
	this->test_transparency();
}

//--------------------------------------------------------------------
//	convenience constructor that will allow you to set the shader
//  and its data
//--------------------------------------------------------------------
matMaterialLayer::matMaterialLayer(const std::string& i_EffectID,
			effShaderData* i_ShaderData /* = NULL */)
:	m_EffectID(i_EffectID),
	m_pShaderData(i_ShaderData)
{
	m_Flags.m_bHasSpecular = true;
	m_Flags.m_bAdditive = false;

	if ((m_pShaderData == NULL) && (!m_EffectID.empty()))
		m_pShaderData = matShaderMgr::CreateData(m_EffectID);

	m_Flags.m_bForceTransparent = false;
	this->test_transparency();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matMaterialLayer::matMaterialLayer(const fsLocator& i_EffectID)
:	//m_EffectID(i_EffectID),
	m_pShaderData(NULL)
{
	m_Flags.m_bHasSpecular = true;
	m_Flags.m_bAdditive = false;

	m_pShaderParams.reset(new effShaderParams);

	m_Flags.m_bForceTransparent = false;
	this->test_transparency();
}


//--------------------------------------------------------------------
//	The copy constructor does copy the material animations, if
//	any.
//--------------------------------------------------------------------
matMaterialLayer::matMaterialLayer(const matMaterialLayer& i_CopyFrom)
: m_pShaderData(NULL)
{
	(*this) = i_CopyFrom;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
matMaterialLayer::~matMaterialLayer()
{
	delete m_pShaderData;
}

//--------------------------------------------------------------------
//	The operator = does copy the material animations, if any.
//--------------------------------------------------------------------
matMaterialLayer& matMaterialLayer::operator = (const matMaterialLayer& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	this->m_Flags = i_CopyFrom.m_Flags;
	this->m_EffectID = i_CopyFrom.m_EffectID;

	delete this->m_pShaderData;
	this->m_pShaderData = NULL;
	if (i_CopyFrom.m_pShaderData)
		this->m_pShaderData = i_CopyFrom.m_pShaderData->Clone();

	return *this;
}

//--------------------------------------------------------------------
//	SimpleCopy() copies values from given material, but
//	does not copy the material animations, if any.
//--------------------------------------------------------------------
void matMaterialLayer::SimpleCopy(const matMaterialLayer& i_CopyFrom)
{
	m_Flags = i_CopyFrom.m_Flags;
	m_EffectID = i_CopyFrom.m_EffectID;

	delete m_pShaderData;
	m_pShaderData = NULL;
	if (i_CopyFrom.m_pShaderData)
		m_pShaderData = i_CopyFrom.m_pShaderData->Clone();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool matMaterialLayer::operator == (const matMaterialLayer& i_Material) const
{
	return
	(m_Flags.m_bHasSpecular == i_Material.m_Flags.m_bHasSpecular) &&
	(m_Flags.m_bTransparency == i_Material.m_Flags.m_bTransparency) &&
	(m_Flags.m_bForceTransparent == i_Material.m_Flags.m_bForceTransparent) &&
	(m_EffectID == i_Material.m_EffectID);
}

//--------------------------------------------------------------------
//	SetHasSpecular allows the user to specify if the material should
//	be rendered with a specular highlight.
//--------------------------------------------------------------------
void matMaterialLayer::SetHasSpecular(bool i_Specular)
{
	m_Flags.m_bHasSpecular = i_Specular;
}

//--------------------------------------------------------------------
//	ShaderEffect controls algorithm for rendering with
//		multiple passes
//--------------------------------------------------------------------
void matMaterialLayer::SetShaderEffect(const std::string& i_EffectID, const effShaderData* i_Data /*= NULL*/) const 
{
	if ((i_EffectID == m_EffectID) && (i_Data == NULL))
		return;
	if ((i_EffectID == m_EffectID) && (i_Data == m_pShaderData))
		return;

	m_EffectID = i_EffectID;

	if (m_EffectID.empty())
	{
		DBG_LOG("Shader set to NULL");
		delete m_pShaderData;
		m_pShaderData = NULL;
	}
	else
	{
		// BIG ASSUMPTION: m_pShaderData IS COMPATIBLE WITH m_pShaderEffect
		// CAN THIS BE ENFORCED BETTER?

		if (i_Data != NULL)
		{
			delete m_pShaderData;
			m_pShaderData = i_Data->Clone();
		}
//		else
//			m_pShaderData = m_pShaderEffect->CreateData(this);
	}
}

//--------------------------------------------------------------------
//	RemoveTextures removes all textures
//--------------------------------------------------------------------
void matMaterialLayer::RemoveTextures()
{
	if (m_pShaderData)
		m_pShaderData->RemoveTextures();
	this->test_transparency();
}

//--------------------------------------------------------------------
//	SetForceTransparency forces the material to be considered
//	transparent.  If it is false then it is tested for transparency
//	using the normal methods.
//--------------------------------------------------------------------
void matMaterialLayer::ForceTransparency(bool i_bTransparent)
{
	m_Flags.m_bForceTransparent = i_bTransparent;

	test_transparency();
}

//----------------------------------------------------------------------------
//	SetAdditive turns on additive mode rendering.
//----------------------------------------------------------------------------
void matMaterialLayer::SetAdditive( bool i_bAdditive )
{
	m_Flags.m_bAdditive = i_bAdditive;
}

//----------------------------------------------------------------------------
//	test_transparency inspects all of the texture and color information
//	and decides if the material could be transparent.
//----------------------------------------------------------------------------
void matMaterialLayer::test_transparency()
{
	// it is transparent if it is being forced to transparent
	if ( m_Flags.m_bForceTransparent )
	{
		m_Flags.m_bTransparency = true;
		return;
	}

    if (m_pShaderParams)
	{
		if (m_pShaderParams->HasTransparency())
		{
			m_Flags.m_bTransparency = true;
			return;
		}
	}
	if (m_pShaderData)
	{
		if (m_pShaderData->HasTransparency())
		{
			m_Flags.m_bTransparency = true;
			return;
		}
	}

	//	It is probably not transparent 
	m_Flags.m_bTransparency = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effShaderData* matMaterialLayer::GetEffectData() const
{
	if (m_pShaderData == NULL)
	{
		if (m_EffectID.empty())
			m_pShaderData = matShaderMgr::CreateData(m_EffectID);
	}
	return m_pShaderData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
shared_ptr<effShaderParams> matMaterialLayer::GetShaderParams() const
{
	return m_pShaderParams;
}
void matMaterialLayer::SetShaderParams(shared_ptr<effShaderParams> i_Params)
{
	m_pShaderParams = i_Params;
}

