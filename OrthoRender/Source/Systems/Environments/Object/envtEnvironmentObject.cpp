/*****************************************************************************
**  envtEnvironmentObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtEnvironmentObject.hpp"

#include "Systems/Environments/GUI/envtDialogDataUtil.hpp"
#include "Systems/Environments/Data/envtDocumentChunk.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"
#include "Systems/Environments/GUI/envtTextureList.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtEnvironmentObject::envtEnvironmentObject()
:	m_pParent(NULL)
{
	m_pEnvironment = evmtEnvironmentMgr::CreateEnvironment();

	// look up first file path in envtTextureList for initial dir.
	fsysFileList fileList;
	envtTextureList::BuildFileList(fileList);
	fsLocator texDir;
	fileList.GetFilePath(0, texDir);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );

	m_pDiffuseFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_DiffuseMapName), "Diffuse", "Diffuse Map");
	m_pDiffuseFileChooser->SetInitialDirectory( texDir );
	AddProperty( m_pDiffuseFileChooser );
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
	m_pSpecularFileChooser = new prtyFileChooserUIInfo(&(m_Data.m_SpecularMapName), "Specular", "Specular Map");
	m_pSpecularFileChooser->SetInitialDirectory( texDir );
	AddProperty( m_pSpecularFileChooser );
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
	m_Data.m_DiffuseMapName.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
	m_Data.m_DiffuseFactor.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
	m_Data.m_DiffuseAngle.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
	m_Data.m_SpecularMapName.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
	m_Data.m_SpecularFactor.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
	m_Data.m_SpecularAngle.AddCallback(new prtyCallbackWrapper<envtEnvironmentObject>(this, &envtEnvironmentObject::ValueChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtEnvironmentObject::~envtEnvironmentObject()
{
	for (int i = 0; i < m_Data.m_Objects.size(); i++)
	{
		evmtEnvironmentMgr::RemoveObjectFromEnvironment(m_pEnvironment, m_Data.m_Objects[i]);
	}
	m_Data.m_Objects.clear();
	evmtEnvironmentMgr::DeleteEnvironment(m_pEnvironment);

}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string envtEnvironmentObject::GetPick3dName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
// Get values as a environment data structure
//--------------------------------------------------------------------
const envtData& envtEnvironmentObject::GetData() const
{
	return m_Data;
}

//--------------------------------------------------------------------
// Set from environment data structure
//--------------------------------------------------------------------
void envtEnvironmentObject::SetData(const envtData &i_Data)
{
	int i;
	for (i = 0; i < m_Data.m_Objects.size(); i++)
	{
		evmtEnvironmentMgr::RemoveObjectFromEnvironment(m_pEnvironment, m_Data.m_Objects[i]);
	}

	m_Data = i_Data;

	for (i = 0; i < m_Data.m_Objects.size(); i++)
	{
		evmtEnvironmentMgr::AddObjectToEnvironment(m_pEnvironment, m_Data.m_Objects[i]);
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
	}
}

void envtEnvironmentObject::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_pEnvironment->SetDiffuseAngle(m_Data.m_DiffuseAngle.GetValue());
	m_pEnvironment->SetDiffuseFactor(m_Data.m_DiffuseFactor.GetValue());
	m_pEnvironment->SetSpecularAngle(m_Data.m_SpecularAngle.GetValue());
	m_pEnvironment->SetSpecularFactor(m_Data.m_SpecularFactor.GetValue());

	fsysFileList file_list;
	envtTextureList::BuildFileList(file_list);

	m_pEnvironment->SetDiffuseMap(m_Data.m_DiffuseMapName.GetValue(), file_list);
	m_pEnvironment->SetSpecularMap(m_Data.m_SpecularMapName.GetValue(), file_list);

	if (i_bDirty)
	{
		envtDialogDataUtil::UpdateListDialog();
		envtDocumentChunk::ActiveDataChanged();
	}
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//virtual 
pick3dPickObject* envtEnvironmentObject::GetParentObject() const
{
	return m_pParent;
}
void envtEnvironmentObject::SetParentObject(pick3dPickObject* i_pPO)
{
	m_pParent = i_pPO;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envtEnvironmentObject::Add(const nameString& i_ObjectName)
{
	m_Data.m_Objects.push_back(i_ObjectName);
	evmtEnvironmentMgr::AddObjectToEnvironment(m_pEnvironment, i_ObjectName);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envtEnvironmentObject::Remove(const nameString& i_ObjectName)
{
	envSTLHelpers::RemoveOneValue(m_Data.m_Objects, i_ObjectName);
	evmtEnvironmentMgr::RemoveObjectFromEnvironment(m_pEnvironment, i_ObjectName);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envtEnvironmentObject::RefreshNames()
{
	m_Data.m_Objects.clear();
	for (int i = 0; i < m_pEnvironment->m_Objects.size(); i++)
	{
		m_Data.m_Objects.push_back(m_pEnvironment->m_Objects[i]->m_pNameObj->GetName());
	}
}
