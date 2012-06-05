/*****************************************************************************
**  envtEnvironmentObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtEnvironmentObject.hpp"

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
#include "Core/prty/prtyColorRGBEditUIInfo.hpp"
//#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Graphics/mat/matTexture.hpp"

#include <sstream>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtEnvironmentObject::envtEnvironmentObject()
:	m_pParent(NULL), 
	m_bEnabledRamp(false)
{
	m_pEnvironment = evmtEnvironmentMgr::CreateEnvironment();
#ifdef USE_SWL_UI
	m_pSwlInterest = new envtSwlInterest(m_pEnvironment, m_Data);
	this->RegisterInterest(m_pSwlInterest);
#endif
	// look up first file path in envtTextureList for initial dir.
	//fsysFileList fileList;
	//envtTextureList::BuildFileList(fileList);
	//fsLocator texDir;
	//if (fileList.Size() > 0)
	//	fileList.GetFilePath(0, texDir);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );

	m_pDiffuseFileChooser = new prtyTextureFileChooserUIInfo(&(m_Data.m_DiffuseMapName), "Diffuse", "Diffuse Map");
	//m_pDiffuseFileChooser->SetInitialDirectory( texDir );
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
	m_pSpecularFileChooser = new prtyTextureFileChooserUIInfo(&(m_Data.m_SpecularMapName), "Specular", "Specular Map");
	//m_pSpecularFileChooser->SetInitialDirectory( texDir );
	m_pSpecularFileChooser->SetDirectoryCategory("Environments");
	AddProperty( m_pSpecularFileChooser );
	pPUII  = new prtyColorRGBEditUIInfo(&(m_Data.m_SpecularColor), "Specular", "Tint color for specular");
	AddProperty( pPUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_SpecularFactor), "Specular", "Brightness multiplier");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetNumTicks(100);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_SpecularAngle), "Specular", "Rotation angle");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(360.0f);
	pRFUII->SetNumTicks(360);
	AddProperty( pRFUII );

	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::NameChanged));
	m_Data.m_DiffuseColor.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
	m_Data.m_DiffuseMapName.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::DiffuseMapValueChanged));
	m_Data.m_DiffuseFactor.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
	m_Data.m_DiffuseAngle.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
	m_Data.m_SpecularColor.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
	m_Data.m_SpecularMapName.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
	m_Data.m_SpecularFactor.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
	m_Data.m_SpecularAngle.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));

	shared_ptr<prtyPropertyCallback> rampCallbackPtr(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::RampChangedFromData));
	m_Data.m_RampData.RegisterCallback(rampCallbackPtr);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtEnvironmentObject::~envtEnvironmentObject()
{
//	RemoveAll();
	m_bEnabledRamp = false;
	rmpDialogMgr::SetRampChangedCallback(NULL);
	evmtEnvironmentMgr::DeleteEnvironment(m_pEnvironment);

#ifdef USE_SWL_UI
	delete m_pSwlInterest;
	m_pSwlInterest = NULL;
#endif
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string envtEnvironmentObject::GetDisplayName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
// Get values as a environment data structure
//--------------------------------------------------------------------
const envtData& envtEnvironmentObject::GetData() const
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
void envtEnvironmentObject::SetData(const envtData &i_Data)
{
	m_bEnabledRamp = false;
	evmtEnvironmentMgr::ClearEnvironment(m_Data.m_Name.GetValue());

	m_Data = i_Data;
#ifdef USE_SWL_UI
	this->SetSwlData(m_Data.m_SwlData);
#endif

	nameString set_name = m_Data.m_Name.GetValue();
	const int num_objects = m_Data.m_Objects.size();

	if( m_Data.m_DiffuseMapName.GetFullValue().m_CurrentCallback == 
		prtyTextureFileChooserUIInfo::GetType(prtyTextureFileChooserUIInfo::e_Ramp) )
	{
		//UpdateRampData();
		m_pEnvironment->RampChanged(m_Data.m_RampData);
		m_bEnabledRamp = true;
	}
	else if( m_pEnvironment->GetDiffuseMapName().GetNumNames() == 0 )
	{
		m_pEnvironment->SetDiffuseTexture(NULL);
	}

	for (int i = 0; i < num_objects; ++i)
	{
		evmtEnvironmentMgr::AddObjectToEnvironment(set_name, m_Data.m_Objects[i]);
	}
}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	envtEnvironmentObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	envtEnvironmentObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// DiffuseFactor property access
//--------------------------------------------------------------------
prtyFloat&	envtEnvironmentObject::PropertyDiffuseFactor()
{
	return m_Data.m_DiffuseFactor;
}
const prtyFloat&	envtEnvironmentObject::GetPropertyDiffuseFactor() const
{
	return m_Data.m_DiffuseFactor;
}

//--------------------------------------------------------------------
// DiffuseAngle property access
//--------------------------------------------------------------------
prtyFloat&	envtEnvironmentObject::PropertyDiffuseAngle()
{
	return m_Data.m_DiffuseAngle;
}
const prtyFloat&	envtEnvironmentObject::GetPropertyDiffuseAngle() const
{
	return m_Data.m_DiffuseAngle;
}

//--------------------------------------------------------------------
// SpecularFactor property access
//--------------------------------------------------------------------
prtyFloat&	envtEnvironmentObject::PropertySpecularFactor()
{
	return m_Data.m_SpecularFactor;
}
const prtyFloat&	envtEnvironmentObject::GetPropertySpecularFactor() const
{
	return m_Data.m_SpecularFactor;
}

//--------------------------------------------------------------------
// SpecularAngle property access
//--------------------------------------------------------------------
prtyFloat&	envtEnvironmentObject::PropertySpecularAngle()
{
	return m_Data.m_SpecularAngle;
}
const prtyFloat&	envtEnvironmentObject::GetPropertySpecularAngle() const
{
	return m_Data.m_SpecularAngle;
}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void envtEnvironmentObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue(this->GetName());

	// set name into underlying evmt
	m_pEnvironment->SetName(i_Name);
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void envtEnvironmentObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );
	//m_pEnvironment->m_Name = m_Data.m_Name.GetValue();

	if (i_bDirty)
	{
		envtDialogDataUtil::UpdateListDialog();
		envtDocumentChunk::ActiveDataChanged();

		// If we were selected, then update the object properties
		// dialog so that it tracks the new name correctly
		if (sel3dMgr::GetSelected() == this)
		{
			envtDialogUtil::UpdateDialog(this->GetName());
		}
	}
}

void envtEnvironmentObject::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_pEnvironment->SetName(m_Data.m_Name.GetValue());
	m_pEnvironment->SetDiffuseAngle(m_Data.m_DiffuseAngle.GetValue());
	m_pEnvironment->SetDiffuseFactor(m_Data.m_DiffuseFactor.GetValue());
	m_pEnvironment->SetDiffuseColor(m_Data.m_DiffuseColor.GetValue());
	m_pEnvironment->SetSpecularAngle(m_Data.m_SpecularAngle.GetValue());
	m_pEnvironment->SetSpecularFactor(m_Data.m_SpecularFactor.GetValue());
	m_pEnvironment->SetSpecularColor(m_Data.m_SpecularColor.GetValue());

	//fsysFileList file_list;
	//envtTextureList::BuildFileList(file_list);
	//m_pEnvironment->SetDiffuseMap(m_Data.m_DiffuseMapName.GetValue(), file_list);
	//m_pEnvironment->SetSpecularMap(m_Data.m_SpecularMapName.GetValue(), file_list);
	//m_pEnvironment->SetDiffuseMap(m_Data.m_DiffuseMapName.GetValue());
	m_pEnvironment->SetSpecularMap(m_Data.m_SpecularMapName.GetValue());

	if (i_bDirty)
	{
		envtDialogDataUtil::UpdateListDialog();
		envtDocumentChunk::ActiveDataChanged();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void envtEnvironmentObject::DiffuseMapValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
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
void envtEnvironmentObject::NotifyRampUI()
{
	rmpDialogMgr::SetRampChangedCallback(NULL);
	rmpDialogMgr::SetData(m_Data.m_RampData);
	rmpDialogMgr::SetRampChangedCallback(std::bind1st(std::mem_fun(&envtEnvironmentObject::RampChangedFromUI), this));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void envtEnvironmentObject::RampChangedFromUI(bool i_bDirty)
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
void envtEnvironmentObject::RampChangedFromData(prtyProperty *i_pProperty, bool i_bDirty)
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

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//virtual 
//sel3dObject* envtEnvironmentObject::GetParentObject() const
//{
//	return m_pParent;
//}
//void envtEnvironmentObject::SetParentObject(sel3dObject* i_pPO)
//{
//	m_pParent = i_pPO;
//}

//----------------------------------------------------------------------------
// Update the ramp object stored in the texture control with the new value.
//----------------------------------------------------------------------------
void envtEnvironmentObject::UpdateRampData()
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
rmpData envtEnvironmentObject::GetRampData()
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
void envtEnvironmentObject::ReportMemory(gfFileTxt& i_File)
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
evmtEnvironment* envtEnvironmentObject::GetEnvironment()
{
	return m_pEnvironment;
};

//--------------------------------------------------------------------
// GetEnabledRamp()
//--------------------------------------------------------------------
bool envtEnvironmentObject::GetEnabledRamp()
{
	return m_bEnabledRamp;
}