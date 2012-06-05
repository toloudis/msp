/*****************************************************************************
**	cptrRenderRmanLiveMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderRmanLiveMgr.hpp"

#include "Features/Capture/cptrRenderRmanLive.hpp"
#include "Features/Capture/cptrRenderRmanLiveData.hpp"

#include "Features/Prefs/PrefsDialogUtil.hpp"

#include "Drivers/Sound/tmlnDriverSound.hpp"
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Keyframing/keyfKeyframeUtil.hpp"
#include "Features/MaterialBrowse/mbrwDialogUtil.hpp"
#include "Features/ObjectManip/mnpModeObjectManip.hpp"
#include "MainApp/mnmApp.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/pyth/pythProperty.hpp"
#include "Support/rman/rmanMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/Env/envThreadGroup.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/private/fsFileNotifyMgr.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyButtonUIInfo.hpp"
#include "Core/prty/prtyUnits.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Graphics/Eff/effOcclusionData.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/Mat/matTextureMgr.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/vtx/vtxVertexAnimBudget.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/cam3d/cam3dManipUtil.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "Tool/icn/icnIconScale.hpp"

#include <string>
#include <vector>

#ifdef USE_WXWIDGETS
#include <wx/tooltip.h>
#endif

#undef CreateDirectory


//============================================================================
//============================================================================
namespace cptrRenderRmanLiveMgr
{
	namespace
	{
		const char* lc_Key_RmanShadingRate				= "RmanShadingRate";
		const char* lc_Key_nRmanAArate				= "nRmanAArate";
		const char* lc_Key_RmanAOsamples				= "RmanAOsamples";
		const char* lc_Key_bRmanCacheTextures				= "bRmanCacheTextures";
		const char* lc_Key_bRmanDisableWarnings				= "bRmanDisableWarnings";
		const char* lc_Key_bRmanAOEnable				= "bRmanAOEnable";
		const char* lc_Key_RmanAOMaxVariation				= "RmanAOMaxVariation";
		const char* lc_Key_bRmanReflEnable				= "bRmanReflEnable";
		const char* lc_Key_RmanReflType				= "RmanReflType";
		const char* lc_Key_bRmanShadowEnable				= "bRmanShadowEnable";
		const char* lc_Key_RmanShadowType				= "RmanShadowType";
		const char* lc_Key_bRmanGIEnable				= "bRmanGIEnable";
		const char* lc_Key_RmanGIsamples				= "RmanGIsamples";
		const char* lc_Key_RmanGIMaxVariation				= "RmanGIMaxVariation";
		const char* lc_Key_RmanFilterType				= "RmanFilterType";
		const char* lc_Key_RmanFilterWidth				= "RmanFilterWidth";
		const char* lc_Key_RmanNumCores				= "RmanNumCores";
		const char* lc_Key_RmanTexMemory				= "RmanTexMemory";
		const char* lc_Key_RmanBucketOrder				= "RmanBucketOrder";
		const char* lc_Key_RmanBucketSize				= "RmanBucketSize";
		const char* lc_Key_RmanRayDepth				= "RmanRayDepth";
		const char* lc_Key_bRmanTonemapEnable				= "bRmanTonemapEnable";



		const char * lc_PrefsFileName			= "RmanLivePrefs.cfg";
		const char * lc_PrefsFileName_Default	= "RmanLivePrefs-Default.cfg";

		std::vector<cptrRenderRmanLiveDataInterest*>	l_PrefsInterestList;

		bool	l_bDataReadIn = false;

		bool	l_RmanRunning = true;

		//====================================================================
		//====================================================================
		class cptrRenderRmanLivePrefsObject : public prtyObject
		{
			public:
				//------------------------------------------------------------
				//------------------------------------------------------------
				cptrRenderRmanLivePrefsObject()
				{
					RegisterProperties();		
				}
				~cptrRenderRmanLivePrefsObject()
				{		
				}

				//------------------------------------------------------------
				//------------------------------------------------------------
				void WritePrefs();
				void ReadPrefs();
				void ApplyPrefs();

				cptrRenderRmanLiveData m_Data;

			private:
				//------------------------------------------------------------
				//------------------------------------------------------------
				void ReadPrefsFromConfigFile(fsLocator& i_ConfigFile);
				void WritePrefsToConfigFile();

				//------------------------------------------------------------
				//------------------------------------------------------------
				virtual void RegisterProperties();

				void ToggleRender(prtyProperty* i_pProperty, bool i_bDirty);

		};


		static cptrRenderRmanLivePrefsObject* l_pcptrRenderRmanLivePrefsObject = 0;

		
		//====================================================================
		//====================================================================
		class PrefsNameResolver : public pythProperty::NameResolver
		{
			//------------------------------------------------------------
			// Resolve the string "Preferences" into our property object
			// in order to be accessible from python.
			//------------------------------------------------------------
			virtual prtyObject* ResolveName(std::string &i_PropertyObjectName)
			{
				if (i_PropertyObjectName == std::string("Preferences"))
				{
					return l_pcptrRenderRmanLivePrefsObject;
				}
				return NULL;
			}
		};

		shared_ptr<PrefsNameResolver> l_NameResolver(new PrefsNameResolver);

		//==========================================================================
		//==========================================================================
		class PrefsTextureAdjuster : public matTextureAdjuster
		{
			//------------------------------------------------------------------------
			//	Implementation of matTextureAdjuster virtual function:
			//	GetReduce returns the amount the texture should be reduced in 
			//	each dimension
			//------------------------------------------------------------------------
			virtual void GetReduce(const fsLocator& i_Locator, 
								   int& o_WidthReduce, 
								   int& o_HeightReduce) const
			{
				// Make sure font for 3d window is not reduced
				if (i_Locator.GetLastName() == itString(mnmConstants::c_ViewerFontName))
					o_WidthReduce = o_HeightReduce = 0;
//				else
//					o_WidthReduce = o_HeightReduce = cptrRenderRmanLiveMgr::Data().m_TextureReduce.GetValue();
			}
		};
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void cptrRenderRmanLivePrefsObject::ReadPrefsFromConfigFile(fsLocator& i_ConfigFile)
	{
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

		guiXMLTextReader::Open(cfgpath.c_str());

		//	read in the preferences
		guiXMLTextReader::gui_Node_Type node_type;
		std::string keyname, strvalue;

		while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
		{
			if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
			{

				if (keyname == lc_Key_RmanShadingRate)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanShadingRate.SetValue(value);};

				if (keyname == lc_Key_nRmanAArate)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_nRmanAArate.SetValue(value);};

				if (keyname == lc_Key_RmanAOsamples)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanAOsamples.SetValue(value);};

				if (keyname == lc_Key_bRmanCacheTextures)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bRmanCacheTextures.SetValue(value);};

				if (keyname == lc_Key_bRmanDisableWarnings)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bRmanDisableWarnings.SetValue(value);};

				if (keyname == lc_Key_bRmanAOEnable)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bRmanAOEnable.SetValue(value);};

				if (keyname == lc_Key_RmanAOMaxVariation)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanAOMaxVariation.SetValue(value);};

				if (keyname == lc_Key_bRmanReflEnable)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bRmanReflEnable.SetValue(value);};

				if (keyname == lc_Key_RmanReflType)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanReflType.SetValue(value);};

				if (keyname == lc_Key_bRmanShadowEnable)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bRmanShadowEnable.SetValue(value);};

				if (keyname == lc_Key_RmanShadowType)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanShadowType.SetValue(value);};

				if (keyname == lc_Key_bRmanGIEnable)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bRmanGIEnable.SetValue(value);};

				if (keyname == lc_Key_RmanGIsamples)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanGIsamples.SetValue(value);};

				if (keyname == lc_Key_RmanGIMaxVariation)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanGIMaxVariation.SetValue(value);};

				if (keyname == lc_Key_RmanFilterType)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanFilterType.SetValue(value);};

				if (keyname == lc_Key_RmanFilterWidth)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanFilterWidth.SetValue(value);};

				if (keyname == lc_Key_RmanNumCores)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanNumCores.SetValue(value);};

				if (keyname == lc_Key_RmanTexMemory)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanTexMemory.SetValue(value);};

				if (keyname == lc_Key_RmanBucketOrder)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanBucketOrder.SetValue(value);};

				if (keyname == lc_Key_RmanBucketSize)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanBucketSize.SetValue(value);};

				if (keyname == lc_Key_RmanRayDepth)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RmanRayDepth.SetValue(value);};

				if (keyname == lc_Key_bRmanTonemapEnable)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bRmanTonemapEnable.SetValue(value);};

			}
		}

		guiXMLTextReader::Close();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void cptrRenderRmanLivePrefsObject::ReadPrefs()
	{
		std::string cfgpath;
		fsLocator cfgdir;
		cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
		cfgdir.Push(lc_PrefsFileName);
		DBG_TRACE("RmanLivePrefs.cfg (read)=" << cfgdir);

		if (fsFileUtil::FileExists(cfgdir))
		{
			ReadPrefsFromConfigFile(cfgdir);
		}
		else
		{
			// no hot key config, so check for default
			cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
			cfgdir.Push( lc_PrefsFileName_Default );
			if (fsFileUtil::FileExists(cfgdir))
			{
				ReadPrefsFromConfigFile(cfgdir);
			}
		}

		//	take the data and apply it as necessary
		//
		ApplyPrefs();

	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void cptrRenderRmanLivePrefsObject::ApplyPrefs()
	{
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//virtual 
	void cptrRenderRmanLivePrefsObject::RegisterProperties()
	{
		prtyPropertyUIInfo* pPUII;
		prtyNumericUpDownUIInfo* pNUDUII;
		prtyRangedFloatUIInfo* pRFUII;
		prtyComboBoxUIInfo* pCBUII;	

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bRmanCacheTextures), "Export Settings", "Rewrite Geometry and Textures");
		AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_RmanFilterType), "Render Settings", "Pixel Filter");
		AddProperty( pCBUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_RmanFilterWidth), "Render Settings", "Filter Width");
		pNUDUII->SetDecimalPlaces(2);
		pNUDUII->SetIncrement(0.01f);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(256);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_nRmanAArate), "Render Settings", "Sampling Rate");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(256);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_RmanShadingRate), "Render Settings", "Shading Rate");
		pRFUII->SetDecimalPlaces(3);
		pRFUII->SetMinimum(0.001f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetRestrictFlag(true);
		AddProperty(pRFUII);

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_RmanBucketOrder), "Render Settings", "Bucket Order");
		AddProperty( pCBUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_RmanBucketSize), "Render Settings", "Bucket Size");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1.0f);
		pNUDUII->SetMaximum(256.0f);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_RmanRayDepth), "Render Settings", "Ray Tracing Depth");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1.0f);
		pNUDUII->SetMaximum(30.0f);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_RmanNumCores), "Performance Settings", "Number of Render Threads");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1.0f);
		pNUDUII->SetMaximum(64.0f);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_RmanTexMemory), "Performance Settings", "Texture Memory");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1.0f);
		pNUDUII->SetMaximum(65536.0f);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bRmanDisableWarnings), "Performance Settings", "Disable Warnings");
		AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bRmanTonemapEnable), "Tonemapping", "Enable");
		AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bRmanReflEnable), "Reflection and Refraction", "Enable");
		AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_RmanReflType), "Reflection and Refraction", "Refl & Refr Type");
		AddProperty( pCBUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bRmanShadowEnable), "Shadows", "Enable");
		AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_RmanShadowType), "Shadows", "Shadow Type");
		AddProperty( pCBUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bRmanAOEnable), "Ambient Occlusion", "Enable");
		AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_RmanAOsamples), "Ambient Occlusion", "Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(1024);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_RmanAOMaxVariation), "Ambient Occlusion", "Max Variation (scaled to range from 0 to 100)");
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetMinimum(0);
		pRFUII->SetMaximum(100);
		AddProperty(pRFUII);

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bRmanGIEnable), "Color Bleeding", "Enable");
		AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_RmanGIsamples), "Color Bleeding", "Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(1024);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_RmanGIMaxVariation), "Color Bleeding", "Max Variation (scaled to range from 0 to 100)");
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetMinimum(0);
		pRFUII->SetMaximum(100);
		pRFUII->SetRestrictFlag(true);
		AddProperty(pRFUII);

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_RmanRenderPass), "Render", "Render Pass");
		AddProperty( pCBUII );	

		prtyButtonUIInfo* pBUII;
		pBUII = new prtyButtonUIInfo(&(m_Data.m_bStartStop), "Render", "Start Render");
		pBUII->SetText(std::string("Start Preview"));
		AddProperty( pBUII );

		//	Register callbacks for items that need to be updated immediately.		
		m_Data.m_bStartStop.AddCallback(new prtyCallbackWrapper<cptrRenderRmanLivePrefsObject>(this, &cptrRenderRmanLivePrefsObject::ToggleRender));
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void cptrRenderRmanLivePrefsObject::WritePrefs()
	{
		try
		{
			// Update any values that are not tracked in UI
			//m_Data.m_MaterialBrowserHome.SetValue( mbrwDialogUtil::GetInitialDirectory() );

			WritePrefsToConfigFile();
		}
		catch (fsInvalidLocatorX& i_Ex)
		{
			fsLocator loc = i_Ex.GetLocator();
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(loc, filename);
			std::string msg = "Cannot write to file\n" + filename + "\nYou might not have permissions.";
			guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void cptrRenderRmanLivePrefsObject::WritePrefsToConfigFile()
	{
		fsLocator cfgdir;
		cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);

		// check if folder exists
		if ( !fsFileUtil::DirectoryExists(cfgdir) )
		{
			fsFileUtil::CreateDirectory(cfgdir);
		}
		
		cfgdir.Push(lc_PrefsFileName);
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);
		DBG_TRACE("RmanLivePrefs.cfg (write)=" << cfgdir);

		//	quick and dirty XML Writer
		guiXMLTextWriter::Open(cfgpath.c_str());
		guiXMLTextWriter::WriteStartElement("Rman_Live_Preferences");

		//	write the actual values
		guiXMLTextWriter::WriteElement(lc_Key_RmanShadingRate, m_Data.m_RmanShadingRate.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_nRmanAArate, m_Data.m_nRmanAArate.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanAOsamples, m_Data.m_RmanAOsamples.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bRmanCacheTextures, m_Data.m_bRmanCacheTextures.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bRmanDisableWarnings, m_Data.m_bRmanDisableWarnings.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bRmanAOEnable, m_Data.m_bRmanAOEnable.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanAOMaxVariation, m_Data.m_RmanAOMaxVariation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bRmanReflEnable, m_Data.m_bRmanReflEnable.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanReflType, m_Data.m_RmanReflType.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bRmanShadowEnable, m_Data.m_bRmanShadowEnable.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanShadowType, m_Data.m_RmanShadowType.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bRmanGIEnable, m_Data.m_bRmanGIEnable.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanGIsamples, m_Data.m_RmanGIsamples.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanGIMaxVariation, m_Data.m_RmanGIMaxVariation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanFilterType, m_Data.m_RmanFilterType.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanFilterWidth, m_Data.m_RmanFilterWidth.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanNumCores, m_Data.m_RmanNumCores.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanTexMemory, m_Data.m_RmanTexMemory.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanBucketOrder, m_Data.m_RmanBucketOrder.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanBucketSize, m_Data.m_RmanBucketSize.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_RmanRayDepth, m_Data.m_RmanRayDepth.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bRmanTonemapEnable, m_Data.m_bRmanTonemapEnable.GetValue());

		//	finish it up
		guiXMLTextWriter::WriteEndElement();
		guiXMLTextWriter::Close();
	}		
	
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadHotKeys()
	{
		cmaCommandMgr::ReadHotKeys();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WriteHotKeys()
	{
		cmaCommandMgr::WriteHotKeys();
	}

	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp()
	{
		if (l_pcptrRenderRmanLivePrefsObject)
		{
			// Remove name resolver 
			pythProperty::RemoveNameResolver( l_NameResolver ); 

			delete l_pcptrRenderRmanLivePrefsObject;
			l_pcptrRenderRmanLivePrefsObject = NULL;
		}

		cptrRenderRmanLive::CleanUp();

		//fsLocator rmanLiveRoot = gfPaths::GetPath(gfPaths::e_UserDataPath);
		//rmanLiveRoot.Push("rmanLiveCache");

		//fsFileUtil::DeleteDirectoryRecursively(rmanLiveRoot);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WritePrefs()
	{
		//
		//	main prefs
		//
		l_pcptrRenderRmanLivePrefsObject->WritePrefs();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadPrefs()
	{
		//
		//	main prefs
		//
		if ( l_pcptrRenderRmanLivePrefsObject == 0 )
		{
			l_pcptrRenderRmanLivePrefsObject = new cptrRenderRmanLivePrefsObject();

			// Add name resolver now that we have a property object
			pythProperty::AddNameResolver( l_NameResolver ); 

			// Set the texture adjuster to this pointer so that we can control 
			// the reduction of loaded textures
			//matTextureMgr::SetTextureAdjuster( new PrefsTextureAdjuster() );
			
			// Leaving the property "allow missing textures" as always on now
			matTextureMgr::SetAllowNullTextures(true);
		}

		l_pcptrRenderRmanLivePrefsObject->ReadPrefs();
	}

	//------------------------------------------------------------------------
	//	apply the prefs
	//------------------------------------------------------------------------
	void ApplyPrefs()
	{
		if (l_pcptrRenderRmanLivePrefsObject != NULL)
			l_pcptrRenderRmanLivePrefsObject->ApplyPrefs();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrRenderRmanLiveData& Data()
	{
		if ( !l_bDataReadIn )
		{
			ReadPrefs();
			l_bDataReadIn = true;
		}

		return l_pcptrRenderRmanLivePrefsObject->m_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrRenderRmanLiveData& GetDataSimple()
	{
		return l_pcptrRenderRmanLivePrefsObject->m_Data;
	}

	//------------------------------------------------------------------------
	//	RegisterInterest() - add an interest
	//------------------------------------------------------------------------
	void RegisterInterest( cptrRenderRmanLiveDataInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

		l_PrefsInterestList.push_back( i_pInterest );
	}

	//------------------------------------------------------------------------
	//	UnRegisterInterest() - remove an interest
	//
	//	Note: this will NOT delete the  interest.  It is up to the
	//	registerer.
	//------------------------------------------------------------------------
	void UnRegisterInterest( cptrRenderRmanLiveDataInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

		envSTLHelpers::RemoveOneValue( l_PrefsInterestList, i_pInterest );
	}

	//------------------------------------------------------------------------
	//	Clear() - clear the interest list
	//------------------------------------------------------------------------
	void ClearInterests()
	{
		l_PrefsInterestList.clear();
	}

	//------------------------------------------------------------------------
	//	This gets called to let the interests know that the data has changed
	//------------------------------------------------------------------------
	void DataUpdated()
	{
		for (int i = 0; i < l_PrefsInterestList.size(); ++i)
		{
			l_PrefsInterestList[i]->RenderRmanLiveDataUpdated(l_pcptrRenderRmanLivePrefsObject->m_Data);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject()
	{
		return l_pcptrRenderRmanLivePrefsObject;
	}

	void cptrRenderRmanLiveMgr::TryRenderManLive()
	{
		if ( !(docSingleDocumentMgr::GetFilename().GetNumNames() > 0) )
		{
			guiMessageBox::Show("Please load a scene first.", "mental ray Live Error");
			return;
		}

		cptrRenderRmanLiveData data = l_pcptrRenderRmanLivePrefsObject->m_Data;

		cptrRenderRmanLive::Start(data);

		//l_RmanRunning = !l_RmanRunning;

		//if ( !l_RmanRunning )
		//{
		//	cptrRenderRmanLive::Start(m_Data);
		//}
		//else
		//{
		//	cptrRenderRmanLive::Stop();
		//}
	}

	void cptrRenderRmanLivePrefsObject::ToggleRender(prtyProperty *i_pProperty, bool i_bDirty)
	{	
		TryRenderManLive();
	}

}
