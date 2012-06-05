/*****************************************************************************
**	evmtEnvironment.cpp
**
**	Keeps track of objects to be grouped with common environment settings
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/evmt/evmtEnvironment.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/evmt/private/evmtEnvironmentObject.hpp"
#include "Support/rmp/rmpTextureMgr.hpp"
#include "Support/swl/swlData.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/gpx/gpxGlobalAmbient.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiMessageBox.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
evmtEnvironment::evmtEnvironment(bool i_bIsSwl)
	: m_DiffuseAngle(0), m_SpecularAngle(0),
		m_DiffuseMap(NULL), m_SpecularMap(NULL),
		m_DiffuseFactor(1), m_SpecularFactor(1),
		m_DiffuseColor(0.5f,0.5f,0.5f,1), m_SpecularColor(0,0,0,1),
		m_pRampTexture(NULL), m_bIsSwl(i_bIsSwl)
{
	m_pRampTexture = rmpTextureMgr::CreateRampTexture(512);
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
evmtEnvironment::~evmtEnvironment()
{
	if (m_DiffuseMap != NULL)
	{
		if (m_DiffuseMap == m_pRampTexture)
			m_pRampTexture = NULL;
		matTextureMgr::ReleaseTexture(m_DiffuseMap);
		m_DiffuseMap = NULL;
	}
	if (m_SpecularMap != NULL)
	{
		matTextureMgr::ReleaseTexture(m_SpecularMap);
		m_SpecularMap = NULL;
	}
	if (m_pRampTexture != NULL)
	{
		matTextureMgr::ReleaseTexture(m_pRampTexture);
		m_pRampTexture = NULL;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::SetName(const nameString& i_Name)
{
	m_Name = i_Name;

	// Now we have to go through all of the objects in this environment
	// and have them update 
	// only update data when :
	//	1. not doing software lighting && current environment is not a swl object
	//	2. doing software lighting && current environment is a swl object
	if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
	{
		std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
		for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
		{
			(*obj_it)->UpdateName(m_Name.GetString());
		}
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::SetDiffuseFactor(float i_Factor)
{
	m_DiffuseFactor = i_Factor;

	// Now we have to go through all of the objects in this environment
	// and have them update 
	if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
	{
		std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
		for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
		{
			(*obj_it)->UpdateDiffuse(m_DiffuseMap, m_DiffuseFactor,
				m_DiffuseAngle, m_DiffuseColor);
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::SetSpecularFactor(float i_Factor)
{
	m_SpecularFactor = i_Factor;

	// Now we have to go through all of the objects in this environment
	// and have them update 
	if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
	{
		std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
		for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
		{
			(*obj_it)->UpdateSpecular(m_SpecularMap, m_SpecularFactor,
				m_SpecularAngle, m_SpecularColor);
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float evmtEnvironment::GetDiffuseFactor()
{
	return m_DiffuseFactor;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float evmtEnvironment::GetSpecularFactor()
{
	return m_SpecularFactor;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::SetDiffuseAngle(float i_Angle)
{
	m_DiffuseAngle = i_Angle;

	// Now we have to go through all of the objects in this environment
	// and have them update
	if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
	{
		std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
		for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
		{
			(*obj_it)->UpdateDiffuse(this->m_DiffuseMap, 
				this->m_DiffuseFactor, this->m_DiffuseAngle, this->m_DiffuseColor);
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float evmtEnvironment::GetDiffuseAngle()
{
	return m_DiffuseAngle;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::SetSpecularAngle(float i_Angle)
{
	m_SpecularAngle = i_Angle;

	// Now we have to go through all of the objects in this environment
	// and have them update 
	if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
	{
		std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
		for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
		{
			(*obj_it)->UpdateSpecular(m_SpecularMap, m_SpecularFactor,
				m_SpecularAngle, m_SpecularColor);
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float evmtEnvironment::GetSpecularAngle()
{
	return m_SpecularAngle;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::SetDiffuseMap(const fsLocator& i_FileName)
{
	if (m_DiffuseMapName != i_FileName)
	{
		// stop any render threads for texture manager changes
		gpxRenderControl::ConfirmSingleThread();

		if (m_DiffuseMap != NULL)
		{
			if(m_DiffuseMap == m_pRampTexture)
				m_pRampTexture = NULL;

			matTextureMgr::ReleaseTexture(m_DiffuseMap);
			m_DiffuseMap = NULL;
		}

		if (i_FileName.GetNumNames() > 0)
		{
			m_DiffuseMap = matTextureMgr::LoadTexture(i_FileName);
		}
		m_DiffuseMapName = i_FileName;

		// Now we have to go through all of the objects in this environment
		// and have them update 
		if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
		{
			std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
			for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
			{
				(*obj_it)->UpdateDiffuse(m_DiffuseMap, m_DiffuseFactor,
					m_DiffuseAngle, m_DiffuseColor);
			}
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::SetDiffuseTexture(matTexture* i_pTexture)
{
	// stop any render threads for texture manager changes
	gpxRenderControl::ConfirmSingleThread();

	if (m_DiffuseMap != NULL && m_DiffuseMap != m_pRampTexture)
	{
		matTextureMgr::ReleaseTexture(m_DiffuseMap);
		m_DiffuseMap = NULL;
	}

	m_DiffuseMap = i_pTexture;
	m_DiffuseMapName.Clear();

	// Now we have to go through all of the objects in this environment
	// and have them update 
	if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
	{
		std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
		for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
		{
			(*obj_it)->UpdateDiffuse(m_DiffuseMap, m_DiffuseFactor,
				m_DiffuseAngle, m_DiffuseColor);
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::SetSpecularMap(const fsLocator& i_FileName)
{
	if (m_SpecularMapName != i_FileName)
	{
		// stop any render threads for texture manager changes
		gpxRenderControl::ConfirmSingleThread();

		if (m_SpecularMap != NULL)
		{
			matTextureMgr::ReleaseTexture(m_SpecularMap);
			m_SpecularMap = NULL;
		}

		if (i_FileName.GetNumNames() > 0)
		{
			m_SpecularMap = matTextureMgr::LoadTexture(i_FileName);
		}
		m_SpecularMapName = i_FileName;

		// Now we have to go through all of the objects in this environment
		// and have them update 
		if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
		{
			std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
			for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
			{
				(*obj_it)->UpdateSpecular(m_SpecularMap, m_SpecularFactor,
					m_SpecularAngle, m_SpecularColor);
			}
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
fsLocator evmtEnvironment::GetDiffuseMapName()
{
	return m_DiffuseMapName;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
fsLocator evmtEnvironment::GetSpecularMapName()
{
	return m_SpecularMapName;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::SetDiffuseColor(const maFloatRGBA& i_Color)
{
	m_DiffuseColor = i_Color;

	// Now we have to go through all of the objects in this environment
	// and have them update 
	if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
	{
		std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
		for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
		{
			(*obj_it)->UpdateDiffuse(m_DiffuseMap, m_DiffuseFactor,
				m_DiffuseAngle, m_DiffuseColor);
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::SetSpecularColor(const maFloatRGBA& i_Color)
{
	m_SpecularColor = i_Color;

	// Now we have to go through all of the objects in this environment
	// and have them update 
	if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
	{
		std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
		for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
		{
			(*obj_it)->UpdateSpecular(m_SpecularMap, m_SpecularFactor,
				m_SpecularAngle, m_SpecularColor);
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::GetData(std::vector<nameString>& o_Objects)
{
	o_Objects.clear();
	// Go through all of the objects in this environment
	std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
	for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
	{
		o_Objects.push_back((*obj_it)->GetName());
	}
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::Remove(evmtObject* i_Object)
{
	envSTLHelpers::RemoveOneValue(m_Objects, i_Object);
	i_Object->m_ContainingEnvironment = NULL;

	// update state for this object
	if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
	{
		i_Object->UpdateName("");
		i_Object->UpdateDiffuse(NULL, 0, 0, maFloatRGBA());
		i_Object->UpdateSpecular(NULL, 0, 0, maFloatRGBA());
		//i_Object->UpdateSwlData(false);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::RemoveAllObjects()
{
	// update state for this object
	for(int i = 0; i < m_Objects.size(); i++)
	{
		if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
		{
			m_Objects[i]->UpdateName("");
			m_Objects[i]->UpdateDiffuse(NULL, 0, 0, maFloatRGBA());
			m_Objects[i]->UpdateSpecular(NULL, 0, 0, maFloatRGBA());
			//i_Object->UpdateSwlData(false);
		}
	}

	m_Objects.clear();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::Disconnect()
{
	// Remove this environment from the objects' containing environment list
	std::vector<evmtObject*>::iterator obj_it, obj_end = m_Objects.end();
	for (obj_it = m_Objects.begin(); obj_it != obj_end; ++obj_it)
	{
		(*obj_it)->m_ContainingEnvironment = NULL;

		// update state for this object
		(*obj_it)->UpdateName("");
		(*obj_it)->UpdateDiffuse(NULL, 0, 0, maFloatRGBA());
		(*obj_it)->UpdateSpecular(NULL, 0, 0, maFloatRGBA());
		//(*obj_it)->UpdateSwlData(false);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::Add(evmtObject* i_pObject)
{
	m_Objects.push_back(i_pObject);
	if (!m_bIsSwl)
		i_pObject->m_ContainingEnvironment = this;

	if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
	{
		// update state for this object
		i_pObject->UpdateName(m_Name.GetString());
		i_pObject->UpdateDiffuse(m_DiffuseMap, m_DiffuseFactor,
			m_DiffuseAngle, m_DiffuseColor);
		i_pObject->UpdateSpecular(m_SpecularMap, m_SpecularFactor,
			m_SpecularAngle, m_SpecularColor);
		//i_pObject->UpdateSwlData(m_bSwlEnable);
	}
}

//--------------------------------------------------------------------
// Callback for ramp color change
//--------------------------------------------------------------------
void evmtEnvironment::RampChanged(rmpData i_RampData)
{
	gpxRenderControl::ConfirmSingleThread();

	ConfirmRampTexture(i_RampData.m_TexSize.GetValue());
	rmpTextureMgr::EnableRampTexture(i_RampData, m_pRampTexture);
	if (m_pRampTexture)
		SetDiffuseTexture(m_pRampTexture);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::ConfirmRampTexture(int i_TexSize)
{
	gpxRenderControl::ConfirmSingleThread();

	// check if texture size change
	if (!m_pRampTexture || m_RampTextureSize != i_TexSize)
	{
		m_RampTextureSize = i_TexSize;

		if (m_DiffuseMap == m_pRampTexture)
			SetDiffuseTexture(NULL);

		rmpTextureMgr::ReleaseRampTexture(m_pRampTexture);

		try
		{
			m_pRampTexture = rmpTextureMgr::CreateRampTexture(i_TexSize);
		}
		catch (const g2dOutOfVideoMemoryX& )
		{
			guiMessageBox::Show("Not enough video mem to create ramp texture. Disable ramp texture.", "Error", guiMessageBox::e_OKOnly);
			m_pRampTexture = NULL;
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void evmtEnvironment::Update()
{
	if ( evmtEnvironmentMgr::GetSoftwareLighting() == m_bIsSwl) 
	{
		for (int i = 0; i < m_Objects.size(); i++)
		{
			m_Objects[i]->UpdateName(m_Name.GetString());
			m_Objects[i]->UpdateDiffuse(m_DiffuseMap, m_DiffuseFactor,
				m_DiffuseAngle, m_DiffuseColor);
			m_Objects[i]->UpdateSpecular(m_SpecularMap, m_SpecularFactor,
				m_SpecularAngle, m_SpecularColor);
		}
	}
}


//--------------------------------------------------------------------
// Make current environment setting to global
//--------------------------------------------------------------------
void evmtEnvironment::UpdateToSceneGlobal(gpxGlobalAmbient* i_gpxGlobalAmbient,
										  const swlData& i_swlData)
{
	// Only swl environment can update to global
	if (m_bIsSwl)
	{
		g3dAmbientEnvState envState;
		envState.m_DiffuseMap = m_DiffuseMap;
		envState.m_DiffuseFactor = m_DiffuseFactor;
		envState.m_DiffuseAngle = m_DiffuseAngle;
		envState.m_DiffuseColor = m_DiffuseColor;

		envState.m_SpecularMap = m_SpecularMap;
		envState.m_SpecularFactor = m_SpecularFactor;
		envState.m_SpecularAngle = m_SpecularAngle;
		envState.m_SpecularColor = m_SpecularColor;

		envState.m_bEnableSwlEnv = i_swlData.m_bEnable.GetValue();
		envState.m_bEnableSwlEnvBG = i_swlData.m_bEnableBG.GetValue();

		i_gpxGlobalAmbient->SetGlobalAmbient(envState);
	}
}