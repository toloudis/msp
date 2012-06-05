/*****************************************************************************
**	actnLightMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/actn/actnLightMgr.hpp"

#include "Features/Import/ImportData.hpp"
#include "Features/Import/ImportUtil.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "Systems/AmbientOcclusion/Object/aoObjectMgr.hpp"
#include "Systems/Billboard/Data/billScriptData.hpp"
#include "Systems/Billboard/Object/billBillboardObject.hpp"
#include "Systems/Billboard/Object/billObjectMgr.hpp"
#include "Systems/Billboard/Undo/billOperations.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"
#include "Systems/LightSets/Object/lsetObjectMgr.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#include "Systems/Ptlt/Object/ptltObjectMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"


///-----------------------------------------------------------------------
/// constructors
///-----------------------------------------------------------------------
actnLightMgr::actnLightMgr()
{
}

///-----------------------------------------------------------------------
/// destructors
///-----------------------------------------------------------------------
actnLightMgr::~actnLightMgr()
{
}

///---------------------------------------------------------------------------
///	the current instance of the mode mgr singleton
///---------------------------------------------------------------------------
actnLightMgr* actnLightMgr::Instance = NULL;

///---------------------------------------------------------------------------
//	Deinitialize/Initialize
///---------------------------------------------------------------------------
void actnLightMgr::Initialize()
{
}
void actnLightMgr::DeInitialize()
{
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a lighting element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void actnLightMgr::ProcessLights(const fsLocator& i_LightScene)
{
	DBG_TRACE("Lighting folder: " << i_LightScene);

	// Grab the scene file
	//
	ImportData idata;
	shared_ptr<docDocument> pDoc;

	// Store filename that was loaded
	idata.m_LastFile = i_LightScene;
	idata.m_LastFile.Push(itString(L"Lighting.mab"));
	DBG_TRACE("Light scene: " << idata.m_LastFile);

	//	load in the doc file
	pDoc.reset( docSingleTypeMgr::CreateDocument() );
	const bool l_cUPDATE_WITH_CURRENT_DATA = false;
	pDoc->SetInactive( l_cUPDATE_WITH_CURRENT_DATA );

	// Do actual load - need to handle exceptions here
	//
	//	TODO - implement Qt messagebox!
	try
	{
		pDoc->Load( idata.m_LastFile );
	}
	catch (const fsFileDoesntExistX& i_Ex)
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		std::string msg = "Cannot find file\n" + filename;
		DBG_ERROR("ERROR: " << msg.c_str());
//		guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
		return;
	}
	catch (const fsDirectoryDoesntExistX& i_Ex)
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		std::string msg = "Cannot find directory\n" + filename;
		DBG_ERROR("ERROR: " << msg.c_str());
//		guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
		return;
	}

	// Get data chunk information from the document
	ImportUtil::BuildDataList( pDoc.get(), idata );

	//	select only the lights
	//
	for (int i=0; i < idata.m_Chunks.size(); ++i)
	{
		if (   (idata.m_Chunks[i].m_Desc != std::string("Projected Lights"))
			&& (idata.m_Chunks[i].m_Desc != std::string("Point Lights"))
			&& (idata.m_Chunks[i].m_Desc != std::string("AO"))
			&& (idata.m_Chunks[i].m_Desc != std::string("Environment Lights"))
			&& (idata.m_Chunks[i].m_Desc != std::string("LightSets")))
		{
			DBG_TRACE( i << " " << idata.m_Chunks[i].m_Desc.c_str() << " do not import");
			idata.m_Chunks[i].m_bRemovableChunk = true;
		}
		else
		{
			idata.m_Chunks[i].m_Items.clear();

			DBG_TRACE( i << " " << idata.m_Chunks[i].m_Desc.c_str() << " set for IMPORT!  (" << idata.m_Chunks[i].m_Items.size() << ")");
			for (int j=0; j < idata.m_Chunks[i].m_Items.size(); ++j)
			{
				DBG_TRACE("    " << j << " - " << idata.m_Chunks[i].m_Items[j].c_str() );
			}
		}
	}

	// Delete the lights in the scene
	//
	prjltObjectMgr::Clear();
	ptltObjectMgr::Clear();
	envtObjectMgr::Clear();
	aoObjectMgr::Clear();
	lsetObjectMgr::Clear();

	// Import the new lights in the scene
	//
	DBG_TRACE( "[IMPORT] data chunks in list = " << idata.m_Chunks.size() );
	ImportUtil::ImportDoc( docSingleDocumentMgr::GetDocument(), pDoc.get(), idata );

	//	after loading the lights make sure all the projected lights icons are hidden
	fcuiUtils::ProjectedLight_HideIcons();

	//	Add the billboards to environment light
	const int num_objects = billObjectMgr::GetNumObjects();
	for (int i=0; i<num_objects; ++i)
	{
		billScriptObject *pScriptObject = billObjectMgr::GetObject(i);
		billScriptData billScript = pScriptObject->GetScriptData();
		billBillboardObject *pObject = pScriptObject->GetPickObject();
		billData objData = pObject->GetData();
		envtOperations::AddObjectToEnvironment(nameString(fcuiConstants::c_BILLBOARD_OBJECT_NAME), itString(objData.m_Name.GetString().c_str()));
	}

	pDoc.reset();
}

///---------------------------------------------------------------------------
/// Set the checked item for the lights
///---------------------------------------------------------------------------
void actnLightMgr::SetCheckedItem(const fsLocator& i_LightScene)
{
	m_CheckedLocator = i_LightScene;
}

///---------------------------------------------------------------------------
/// Get the checked item for the lights
///---------------------------------------------------------------------------
const fsLocator& actnLightMgr::GetCheckedItem()
{
	return m_CheckedLocator;
}
