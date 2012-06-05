/*****************************************************************************
**	actnPostFXMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/actn/actnPostFXMgr.hpp"

#include "FCSupport/fcmd/fcmdConstants.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"
#include "Features/Import/ImportData.hpp"
#include "Features/Import/ImportUtil.hpp"
#include "Graphics/g3d/g3dPostProcessing.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/mat/matShaderMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Support/pfx/pfxPostEffectObject.hpp"
#include "Support/pfx/pfxPostEffectMgr.hpp"


///-----------------------------------------------------------------------
/// constructors
///-----------------------------------------------------------------------
actnPostFXMgr::actnPostFXMgr()
{
}

///-----------------------------------------------------------------------
/// destructors
///-----------------------------------------------------------------------
actnPostFXMgr::~actnPostFXMgr()
{
}

///---------------------------------------------------------------------------
///	the current instance of the mode mgr singleton
///---------------------------------------------------------------------------
actnPostFXMgr* actnPostFXMgr::Instance = NULL;

///---------------------------------------------------------------------------
//	Deinitialize/Initialize
///---------------------------------------------------------------------------
void actnPostFXMgr::Initialize()
{
}
void actnPostFXMgr::DeInitialize()
{
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a PostFX element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void actnPostFXMgr::ProcessPostFX(const fsLocator& i_PostFXScene)
{

	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(i_PostFXScene, files);
	
	itString icon( fcmdConstants::c_Icon );
	itString extension, icon_check;
	fsLocator shaderfile, texturefile;
	for ( int i = 0; i < files.size(); ++i )
	{
		icon_check = files[i].GetLastName();
		//don't process the icon file of the asset
		if ( icon_check == icon )
			continue;
		files[i].GetLastName().GetExtension(extension);
		if (extension == itString("fx"))
		{
			shaderfile = files[i];
		}
		else if (extension == itString("bmp"))
		{
			texturefile = files[i];
		}
	}

	DBG_TRACE("PostFX folder: " << i_PostFXScene);

	itString shader_file;
	fsFileUtil::LocatorToUnicodeString(shaderfile, shader_file);
	std::string shader = itStringUtil::GetStdString(shaderfile.GetLastName());

	pfxPostEffectObject* obj = dynamic_cast<pfxPostEffectObject*>(pfxPostEffectMgr::GetDataObject(pfxPostEffectMgr::e_ViewportPfx));
	pfxData& data= obj->GetData();
		
	if(shader == fcuiConstants::c_FILE_NONE_POSTFX)
	{
		data.m_bActive.SetValue(false);
	}
	else
	{
		data.m_bActive.SetValue(true);
		data.m_Name.SetValue(shaderfile);
		obj->ForceLoadTexture(texturefile);
	}

}

///---------------------------------------------------------------------------
/// Set the checked item for the PostFXs
///---------------------------------------------------------------------------
void actnPostFXMgr::SetCheckedItem(const fsLocator& i_PostFXScene)
{
	m_CheckedLocator = i_PostFXScene;
}

///---------------------------------------------------------------------------
/// Get the checked item for the PostFXs
///---------------------------------------------------------------------------
const fsLocator& actnPostFXMgr::GetCheckedItem()
{
	return m_CheckedLocator;
}
