/*****************************************************************************
**  envtSwlEnvironment.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtSwlEnvironment.hpp"

#include "Systems/Environments/GUI/envtDialogDataUtil.hpp"
#include "Systems/Environments/Data/envtDocumentChunk.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"
#include "Systems/Environments/GUI/envtTextureList.hpp"

#include "Support/evmt/evmtEnvironment.hpp"
#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/rmp/rmpDialogMgr.hpp"
#include "Support/rmp/rmpDialogUtil.hpp"
#include "Support/rmp/rmpObject.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBEditUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Tool/gpx/gpxGlobalAmbient.hpp"
#include <sstream>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtSwlEnvironment::envtSwlEnvironment()
:	m_pParent(NULL), 
	m_bEnabledRamp(false),
	m_pGpxEnvironment(new gpxGlobalAmbient())
{
	m_pEnvironment = evmtEnvironmentMgr::GetSwlEnvironment();
	DBG_ASSERT(m_pEnvironment, "Swl Environment not created early enough.");
	m_pEnvironment->SetName(nameString(evmtEnvironmentMgr::GetSwlEnvironmentName()));
	m_Data.m_Name = nameString(evmtEnvironmentMgr::GetSwlEnvironmentName());

	m_Data.m_DiffuseColor.SetPropertyName("IBL Color");
	m_Data.m_DiffuseMapName.SetPropertyName("IBL Map");
	m_Data.m_DiffuseFactor.SetPropertyName("IBL Scale");
	m_Data.m_DiffuseAngle.SetPropertyName("IBL Map Angle");

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	pPUII->SetReadOnly(true);
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_SwlData.m_bEnable), "Asset", "Enable");
	AddProperty( pPUII );
	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_SwlData.m_bEnableBG), "Diffuse", "Enable Background");
	AddProperty( pPUII );

	m_pDiffuseFileChooser = new prtyTextureFileChooserUIInfo(&(m_Data.m_DiffuseMapName), "Diffuse", "Diffuse Map");
	m_pDiffuseFileChooser->SetDirectoryCategory("Environments");
	m_pDiffuseFileChooser->AddItem( m_pDiffuseFileChooser->e_Ramp );
	AddProperty( m_pDiffuseFileChooser );
	
	pPUII  = new prtyColorRGBEditUIInfo(&(m_Data.m_DiffuseColor), "Diffuse", "Tint color for diffuse");
	AddProperty( pPUII );
	prtyRangedFloatUIInfo* pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_DiffuseFactor), "Diffuse", "Brightness multiplier");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_DiffuseAngle), "Diffuse", "Rotation angle");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(360.0f);
	pRFUII->SetNumTicks(360);
	AddProperty( pRFUII );
	
	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_SwlData.m_bEnable.AddCallback(new prtyCallbackWrapper<envtSwlEnvironment>(this, &envtSwlEnvironment::EnableChanged));
	m_Data.m_DiffuseColor.AddCallback(new prtyCallbackWrapper<envtSwlEnvironment>(this, &envtSwlEnvironment::ValueChanged));
	m_Data.m_DiffuseMapName.AddCallback(new prtyCallbackWrapper<envtSwlEnvironment>(this, &envtSwlEnvironment::DiffuseMapValueChanged));
	m_Data.m_DiffuseFactor.AddCallback(new prtyCallbackWrapper<envtSwlEnvironment>(this, &envtSwlEnvironment::ValueChanged));
	m_Data.m_DiffuseAngle.AddCallback(new prtyCallbackWrapper<envtSwlEnvironment>(this, &envtSwlEnvironment::ValueChanged));
	m_Data.m_SwlData.m_bEnableBG.AddCallback(new prtyCallbackWrapper<envtSwlEnvironment>(this, &envtSwlEnvironment::ValueChanged));
	
	shared_ptr<prtyPropertyCallback> rampCallbackPtr(new prtyCallbackWrapper<envtSwlEnvironment>(this, &envtSwlEnvironment::RampChangedFromData));
	m_Data.m_RampData.RegisterCallback(rampCallbackPtr);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtSwlEnvironment::~envtSwlEnvironment()
{
//	RemoveAll();
	m_bEnabledRamp = false;
	rmpDialogMgr::SetRampChangedCallback(NULL);
	evmtEnvironmentMgr::DeleteEnvironment(m_pEnvironment);
	delete m_pGpxEnvironment;
	m_pGpxEnvironment = NULL;
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual set
std::string envtSwlEnvironment::GetDisplayName() const
{
	return evmtEnvironmentMgr::GetSwlEnvironmentName();
}

//--------------------------------------------------------------------
// Get values as a environment data structure
//--------------------------------------------------------------------
const envtData& envtSwlEnvironment::GetData() const
{
	// Since we don't have properties that handle things like
	// checked lists and tree controls, the environment manager
	// still maintains the "true" object and environment connectíons.
	// We have to refresh that info from the manager whenever
	// someone asks for the data.
	std::vector<nameString> objects;
	m_pEnvironment->GetData(objects);

	m_Data.m_Objects = objects;

	return m_Data;
}

//--------------------------------------------------------------------
// Set from environment data structure
//--------------------------------------------------------------------
void envtSwlEnvironment::SetData(const envtData &i_Data)
{
	m_bEnabledRamp = false;
	nameString set_name = nameString(evmtEnvironmentMgr::GetSwlEnvironmentName());

	//evmtEnvironmentMgr::ClearEnvironment(set_name);
	m_pEnvironment->RemoveAllObjects();

	m_Data = i_Data;

	m_Data.m_Name = set_name;
	m_Data.m_DiffuseColor.SetPropertyName("IBL Color");
	m_Data.m_DiffuseMapName.SetPropertyName("IBL Map");
	m_Data.m_DiffuseFactor.SetPropertyName("IBL Scale");
	m_Data.m_DiffuseAngle.SetPropertyName("IBL Map Angle");
	// don't set name into underlying evmt - this env is not nameable.
//	m_pEnvironment->SetName(m_Data.m_Name.GetValue());

	if( m_Data.m_DiffuseMapName.GetFullValue().m_CurrentCallback == 
		prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Ramp) )
	{
		m_pEnvironment->RampChanged(m_Data.m_RampData);
	}
	else if( m_pEnvironment->GetDiffuseMapName().GetNumNames() == 0 )
	{
		m_pEnvironment->SetDiffuseTexture(NULL);
	}

	evmtEnvironmentMgr::AddAllObjectToEnvironment(set_name, false);
	/*const int num_objects = m_Data.m_Objects.size();
	for (int i = 0; i < num_objects; ++i)
	{
		evmtEnvironmentMgr::AddObjectToEnvironment(set_name, m_Data.m_Objects[i]);
	}*/
}

//--------------------------------------------------------------------
// Enable property access
//--------------------------------------------------------------------
prtyBoolean& envtSwlEnvironment::PropertyEnable()
{
	return m_Data.m_SwlData.m_bEnable;
}

const prtyBoolean& envtSwlEnvironment::GetPropertyEnable() const
{
	return m_Data.m_SwlData.m_bEnable;
}

//--------------------------------------------------------------------
// DiffuseFactor property access
//--------------------------------------------------------------------
prtyFloat&	envtSwlEnvironment::PropertyDiffuseFactor()
{
	return m_Data.m_DiffuseFactor;
}
const prtyFloat&	envtSwlEnvironment::GetPropertyDiffuseFactor() const
{
	return m_Data.m_DiffuseFactor;
}

//--------------------------------------------------------------------
// DiffuseAngle property access
//--------------------------------------------------------------------
prtyFloat&	envtSwlEnvironment::PropertyDiffuseAngle()
{
	return m_Data.m_DiffuseAngle;
}
const prtyFloat&	envtSwlEnvironment::GetPropertyDiffuseAngle() const
{
	return m_Data.m_DiffuseAngle;
}

//--------------------------------------------------------------------
// SpecularFactor property access
//--------------------------------------------------------------------
prtyFloat&	envtSwlEnvironment::PropertySpecularFactor()
{
	return m_Data.m_SpecularFactor;
}
const prtyFloat&	envtSwlEnvironment::GetPropertySpecularFactor() const
{
	return m_Data.m_SpecularFactor;
}

//--------------------------------------------------------------------
// SpecularAngle property access
//--------------------------------------------------------------------
prtyFloat&	envtSwlEnvironment::PropertySpecularAngle()
{
	return m_Data.m_SpecularAngle;
}
const prtyFloat&	envtSwlEnvironment::GetPropertySpecularAngle() const
{
	return m_Data.m_SpecularAngle;
}

void envtSwlEnvironment::EnableChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	evmtEnvironmentMgr::SetSoftwareLighting(m_Data.m_SwlData.m_bEnable.GetValue());

	if (m_Data.m_SwlData.m_bEnable.GetValue())
	{
		m_pEnvironment->SetName(evmtEnvironmentMgr::GetSwlEnvironmentName());
		m_pEnvironment->SetDiffuseAngle(m_Data.m_DiffuseAngle.GetValue());
		m_pEnvironment->SetDiffuseFactor(m_Data.m_DiffuseFactor.GetValue());
		m_pEnvironment->SetDiffuseColor(m_Data.m_DiffuseColor.GetValue());
		m_pEnvironment->SetSpecularAngle(m_Data.m_SpecularAngle.GetValue());
		m_pEnvironment->SetSpecularFactor(m_Data.m_SpecularFactor.GetValue());
		m_pEnvironment->SetSpecularColor(m_Data.m_SpecularColor.GetValue());
		m_pEnvironment->SetDiffuseMap(m_Data.m_DiffuseMapName.GetValue());
		m_pEnvironment->SetSpecularMap(m_Data.m_SpecularMapName.GetValue());
	}
	else
	{
		evmtEnvironmentMgr::UpdateAllEnvironments();
	}

	m_pEnvironment->UpdateToSceneGlobal(m_pGpxEnvironment,
										m_Data.m_SwlData);
}

void envtSwlEnvironment::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_pEnvironment->SetName(evmtEnvironmentMgr::GetSwlEnvironmentName());
	m_pEnvironment->SetDiffuseAngle(m_Data.m_DiffuseAngle.GetValue());
	m_pEnvironment->SetDiffuseFactor(m_Data.m_DiffuseFactor.GetValue());
	m_pEnvironment->SetDiffuseColor(m_Data.m_DiffuseColor.GetValue());
	m_pEnvironment->SetSpecularAngle(m_Data.m_SpecularAngle.GetValue());
	m_pEnvironment->SetSpecularFactor(m_Data.m_SpecularFactor.GetValue());
	m_pEnvironment->SetSpecularColor(m_Data.m_SpecularColor.GetValue());

	m_pEnvironment->SetSpecularMap(m_Data.m_SpecularMapName.GetValue());

	m_pEnvironment->UpdateToSceneGlobal(m_pGpxEnvironment,
										m_Data.m_SwlData);

	if (i_bDirty)
	{
		envtDialogDataUtil::UpdateListDialog();
		envtDocumentChunk::ActiveDataChanged();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void envtSwlEnvironment::DiffuseMapValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	std::string callback( m_Data.m_DiffuseMapName.GetFullValue().m_CurrentCallback );

	//if the button on one of the texture options was clicked we need to pass
	//the necessary info to the manager of that operation
	if( m_Data.m_DiffuseMapName.GetFullValue().m_bButtonPressed )
	{
		//reset our texture locator
		prtyTextureFileData val;
		val.m_TextureLocator = m_Data.m_DiffuseMapName.GetValue();

		//decide which callback was executed
		if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Paint) )
		{
			//do paint manager operations
			//brshPaintBrushMgr::SetMatTexture(pParam);
			//m_Data.m_bEnabledRamp.SetValue(false);
			m_bEnabledRamp = false;
		}
		else if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Ramp) )
		{
			m_bEnabledRamp = true;
			m_Data.m_RampData = GetRampData();
			NotifyRampUI();
			//m_Data.m_bEnabledRamp.SetValue(true);
			m_pEnvironment->RampChanged(m_Data.m_RampData);
		}
		else if( callback == prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Reset) )
		{
			callback = prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Texture);
			//m_Data.m_bEnabledRamp.SetValue(false);
			m_bEnabledRamp = false;
			m_Data.m_RampData = rmpData();
			UpdateRampData();
			m_pEnvironment->SetDiffuseTexture(NULL);
		}

		val.m_CurrentCallback = callback;
		m_Data.m_DiffuseMapName.SetValueWithoutNotify(val);
	}

	else
	{
		//m_Data.m_bEnabledRamp.SetValue(false);
		m_bEnabledRamp = false;
		m_pEnvironment->SetDiffuseMap(m_Data.m_DiffuseMapName.GetValue());
	}

	//process other value changes
	ValueChanged(i_pProperty, i_bDirty);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void envtSwlEnvironment::NotifyRampUI()
{
	rmpDialogMgr::SetRampChangedCallback(NULL);
	rmpDialogMgr::SetData(m_Data.m_RampData);
	rmpDialogMgr::SetRampChangedCallback(std::bind1st(std::mem_fun(&envtSwlEnvironment::RampChangedFromUI), this));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void envtSwlEnvironment::RampChangedFromUI(bool i_bDirty)
{
	if (m_bEnabledRamp)
	{
		m_Data.m_RampData = rmpDialogMgr::Data();
		UpdateRampData();
		m_pEnvironment->RampChanged(m_Data.m_RampData);

		if (i_bDirty)
		{
			envtDialogDataUtil::UpdateListDialog();
			envtDocumentChunk::ActiveDataChanged();
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void envtSwlEnvironment::RampChangedFromData(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (m_bEnabledRamp)
	{
		UpdateRampData();
		m_pEnvironment->RampChanged(m_Data.m_RampData);
		
		if (i_bDirty)
		{
			envtDialogDataUtil::UpdateListDialog();
			envtDocumentChunk::ActiveDataChanged();
		}
	}
}

//----------------------------------------------------------------------------
// Update the ramp object stored in the texture control with the new value.
//----------------------------------------------------------------------------
void envtSwlEnvironment::UpdateRampData()
{
	prtyTextureFileData val = m_Data.m_DiffuseMapName.GetFullValue();
	shared_ptr<rmpObject> newRamp(new rmpObject);
	newRamp.get()->m_Data = m_Data.m_RampData;
	val.m_RampObject = (shared_ptr<prtyObject>)newRamp;
	m_Data.m_DiffuseMapName.SetValueWithoutNotify(val);
}

//----------------------------------------------------------------------------
// return the ramp data of the texture control
//----------------------------------------------------------------------------
rmpData envtSwlEnvironment::GetRampData()
{
	rmpData data = m_Data.m_RampData;
	rmpObject* pRampObject = dynamic_cast<rmpObject*>(m_Data.m_DiffuseMapName.GetFullValue().m_RampObject.get());
	if(pRampObject)
	{
		data = pRampObject->m_Data;
	}
	return data;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envtSwlEnvironment::ReportMemory(gfFileTxt& i_File)
{
    std::ostringstream stm;
	
	if (m_pEnvironment->m_DiffuseMap)
	{
		stm << "  Diffuse : " << m_pEnvironment->m_DiffuseMap->GetSize() << "KB\r\n";
	}

	if (m_pEnvironment->m_SpecularMap)
	{
		stm << "  Specular: " << m_pEnvironment->m_SpecularMap->GetSize() << "KB\r\n";
	}

	i_File.WriteLine(stm.str());
}



//--------------------------------------------------------------------
// GetEnvironment()
//--------------------------------------------------------------------
evmtEnvironment* envtSwlEnvironment::GetEnvironment()
{
	return m_pEnvironment;
};

//--------------------------------------------------------------------
// GetEnabledRamp()
//--------------------------------------------------------------------
bool envtSwlEnvironment::GetEnabledRamp()
{
	return m_bEnabledRamp;
}