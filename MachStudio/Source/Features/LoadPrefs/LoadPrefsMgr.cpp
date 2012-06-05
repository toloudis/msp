/*****************************************************************************
**	LoadPrefsMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006-7 - All Rights Reserved
\****************************************************************************/
#include "Features/LoadPrefs/LoadPrefsMgr.hpp"

#include "Features/LoadPrefs/LoadPrefsDialogUtil.hpp"

#include "Drivers/Animation/tmlnDriverAnimationFull.hpp"
#include "Drivers/Sound/tmlnDriverSound.hpp"
#include "Support/pyth/pythProperty.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlReader.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/smdl/private/smdlSubdivNetwork.hpp"

//============================================================================
//============================================================================
namespace LoadPrefsMgr
{
	//========================================================================
	//
	//	Prefs Mgr functions
	//
	//========================================================================

	//===========================================================================
	//	general namespace
	//===========================================================================
	namespace
	{
		//==========================================================================
		//	Load Prefs base object
		//==========================================================================
		class LoadPrefsObject : public prtyObject
		{
			public:
				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				LoadPrefsObject();


			private:

				//--------------------------------------------------------------------
				// Callbacks for when properties change, updates member data
				//--------------------------------------------------------------------
				void UpdateSkipTextures(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateCompressTextures(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateAllowMissingTextures(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateTextureReduce(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateMaxSubdivLevel(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateOptimizeMeshes(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateBasisVectors(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateGeometryVideoMemory(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateAutoGenerateLowRes(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateSkipHighRes(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateSkipLowRes(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateSkipAnimations(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateDelayAnimations(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateSkipSounds(prtyProperty *i_pProperty, bool i_bDirty);

			public:
				LoadPrefsData m_Data;
		};

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		LoadPrefsObject::LoadPrefsObject()
		{
			prtyPropertyUIInfo* pPUII;
			//prtyComboBoxUIInfo* pCBUII;
			prtyNumericUpDownUIInfo* pNUDUII;					

			// Textures
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bNeverLoadTextures), "Textures", "Skip loading all textures. Saves video and system memory");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bAllowMissingTextures), "Textures", "Allow geometry to be loaded even if textures are missing");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bAlwaysLoadAOTextures), "Textures", "Always load AO texures, overriding the Skip All setting for AO only.");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bAutoDXTCompress), "Textures", "Auto Compress Textures");
			AddProperty( pPUII );

			//	Texture Reduce
			pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_TextureReduce), "Textures", "Amount to reduce size of textures when loading. Only numbers between 0 and 31 are valid. Each level divides side length by 2 and memory use by 4.");
			pNUDUII->SetDecimalPlaces(0);
			pNUDUII->SetMinimum(0);
			pNUDUII->SetMaximum(8);
			AddProperty( pNUDUII );

			// Depth Maps
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bNoDepthMaps), "Depth Maps", "Don't allocate depth map textures. Saves video memory when not using shadows.");
			AddProperty( pPUII );

			//	Depth Map Reduce
			pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_DepthMapReduce), "Depth Maps", "Amount to reduce size of depth maps. Each level reduces a side by 2 and memory use by 4.");
			pNUDUII->SetDecimalPlaces(0);
			pNUDUII->SetMinimum(0);
			pNUDUII->SetMaximum(8);
			AddProperty( pNUDUII );

			//	Surfaces
			pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MaxSubdivLevel), "Surfaces", "Maximum level of subdivision. Higher levels take up more system memory.");
			pNUDUII->SetDecimalPlaces(0);
			pNUDUII->SetMinimum(0);
			pNUDUII->SetMaximum(3);
			AddProperty( pNUDUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bOptimizeMeshes), "Surfaces", "Optimize static meshes. Longer load time, but faster render.");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bComputeBasisVectors), "Surfaces", "Necessary only for shadows-on lighting. Longer load time, slower animation.");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bGeometryInVideoMemory), "Surfaces", "Load geometry into video memory. Alternative is system memory.");
			AddProperty( pPUII );
			
			//	Resolutions
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bAutoGenLowRes), "Resolutions", "Auto generate low resolution model for characters. Affects load-time and isn't necessary when doing a batch capture.");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bSkipHighRes), "Resolutions", "Don't load high resolution meshes from file. Improves memory use if the high resolution isn't needed.");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bSkipLowRes), "Resolutions", "Don't load low resolution meshes from file. Isn't necessary when doing a batch capture.");
			AddProperty( pPUII );

			//	Animations
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bNeverLoadAnimation), "Animation", "Don't load any full animations.");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bDelayLoadingAnimation), "Animation", "Wait until an animation is needed before loading it. Might be useful in batch rendering a small portion of a large scene.");
			AddProperty( pPUII );

			// Sounds
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bNeverLoadSounds), "Sounds", "Don't load any sounds. ");
			AddProperty( pPUII );

			// Add Callbacks
			m_Data.m_bNeverLoadTextures.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateSkipTextures));
			m_Data.m_bAutoDXTCompress.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateCompressTextures));
			m_Data.m_bAlwaysLoadAOTextures.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateSkipTextures));
			m_Data.m_bAllowMissingTextures.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateAllowMissingTextures));
			m_Data.m_TextureReduce.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateTextureReduce));
			m_Data.m_MaxSubdivLevel.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateMaxSubdivLevel));
			m_Data.m_bOptimizeMeshes.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateOptimizeMeshes));
			m_Data.m_bComputeBasisVectors.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateBasisVectors));
			m_Data.m_bGeometryInVideoMemory.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateGeometryVideoMemory));
			m_Data.m_bAutoGenLowRes.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateAutoGenerateLowRes));
			m_Data.m_bSkipHighRes.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateSkipHighRes));
			m_Data.m_bSkipLowRes.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateSkipLowRes));
			m_Data.m_bNeverLoadAnimation.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateSkipAnimations));
			m_Data.m_bDelayLoadingAnimation.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateDelayAnimations));
			m_Data.m_bNeverLoadSounds.AddCallback(new prtyCallbackWrapper<LoadPrefsObject>(this, &LoadPrefsObject::UpdateSkipSounds));
	
			// Note: some callbacks are not needed because the system code will look at
			// the preferences data itself. (Like projected lights will look at the depth
			// map settings.) So, we only need callbacks for Terawatt code values.

		}

		void LoadPrefsObject::UpdateSkipTextures(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			matTextureMgr::SetSkipAllTextures(m_Data.m_bNeverLoadTextures.GetValue());
			effOcclusionData::SetSkipTextureOverride(m_Data.m_bAlwaysLoadAOTextures.GetValue());
		}
		void LoadPrefsObject::UpdateCompressTextures(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			matTextureMgr::SetCompressTextures(m_Data.m_bAutoDXTCompress.GetValue());
		}
		void LoadPrefsObject::UpdateAllowMissingTextures(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			matTextureMgr::SetAllowNullTextures(m_Data.m_bAllowMissingTextures.GetValue());
		}
		void LoadPrefsObject::UpdateTextureReduce(prtyProperty *i_pProperty, bool i_bDirty)
		{	
		}
		void LoadPrefsObject::UpdateMaxSubdivLevel(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			smdlSubdivCharacter::SetMaxSubdivLevel(m_Data.m_MaxSubdivLevel.GetValue());
		}
		void LoadPrefsObject::UpdateOptimizeMeshes(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			mdlFragCreate::EnableFragmentOptimize(m_Data.m_bOptimizeMeshes.GetValue());
		}
		void LoadPrefsObject::UpdateBasisVectors(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			g3dFragmentCreate::SetComputeBasisVectors(m_Data.m_bComputeBasisVectors.GetValue());
			smdlSubdivNetwork::SetComputeBasisVectors(m_Data.m_bComputeBasisVectors.GetValue());
		}
		void LoadPrefsObject::UpdateGeometryVideoMemory(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			g3dFragmentCreate::StoreGeometryInVideoMemory(m_Data.m_bGeometryInVideoMemory.GetValue());
		}
		void LoadPrefsObject::UpdateAutoGenerateLowRes(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			smdlSubdivCharacter::SetAutoGenerateLowRes(m_Data.m_bAutoGenLowRes.GetValue());
		}
		void LoadPrefsObject::UpdateSkipHighRes(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			mdlReader::SetSkipHighRes(m_Data.m_bSkipHighRes.GetValue());
		}
		void LoadPrefsObject::UpdateSkipLowRes(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			mdlReader::SetSkipLowRes(m_Data.m_bSkipLowRes.GetValue());
		}
		void LoadPrefsObject::UpdateSkipAnimations(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			tmlnDriverAnimationFull::SetSkipAnimations(m_Data.m_bNeverLoadAnimation.GetValue());
		}
		void LoadPrefsObject::UpdateDelayAnimations(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			tmlnDriverAnimationFull::SetDelayAnimations(m_Data.m_bDelayLoadingAnimation.GetValue());
		}
		void LoadPrefsObject::UpdateSkipSounds(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			tmlnDriverSound::SetSkipSounds(m_Data.m_bNeverLoadSounds.GetValue());
		}

		//==========================================================================
		//==========================================================================
		class LoadPrefsTextureAdjuster : public matTextureAdjuster
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
				o_WidthReduce = o_HeightReduce = LoadPrefsMgr::Data().m_TextureReduce.GetValue();
			}
		};

		//==========================================================================
		//	property object
		//==========================================================================
		LoadPrefsObject* l_pPrefObject = NULL;

		//====================================================================
		//====================================================================
		class LoadPrefsNameResolver : public pythProperty::NameResolver
		{
			//------------------------------------------------------------
			// Resolve strings into our render pref property objects
			// in order to be accessible from python.
			//------------------------------------------------------------
			virtual prtyObject* ResolveName(std::string &i_PropertyObjectName)
			{
				if (i_PropertyObjectName == std::string("LoadPrefs"))
				{
					return l_pPrefObject;
				}
				return NULL;
			}
		};

		shared_ptr<LoadPrefsNameResolver> l_NameResolver(new LoadPrefsNameResolver);
	}


	//========================================================================
	//	LoadPrefsMgr functions
	//========================================================================

	//------------------------------------------------------------------------
	//  Init
	//------------------------------------------------------------------------
	void  Init()
	{
		//	create the prefs objects if not already created
		//
		if (!l_pPrefObject)
		{
			l_pPrefObject = new LoadPrefsObject();

			// Set the texture adjuster to this pointer so that we can control 
			// the reduction of loaded textures
			matTextureMgr::SetTextureAdjuster( new LoadPrefsTextureAdjuster() );

			// Add name resolver so python can access the render preferences
			pythProperty::AddNameResolver( l_NameResolver ); 

			// Change a default value now after calbacks are setup
			l_pPrefObject->m_Data.m_bAllowMissingTextures = true;

			// create commands to display dialog
			AddToMenu();
		}
	}

	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp()
	{
		if (l_pPrefObject)
		{
			// Remove name resolver 
			pythProperty::RemoveNameResolver( l_NameResolver ); 

			delete l_pPrefObject;
			l_pPrefObject = NULL;
		}

	}

	//------------------------------------------------------------------------
	//  AddToMenu() - add Prefs actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu()
	{
		//
		//	commands
		//
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: View Preferences
		pCmd = new cmaCommandSimple("Load Preferences", 
									"Edit", 
									"View Load Preferences",
									
									&LoadPrefsDialogUtil::Show );
		menu_id = guiMenuMgr::AddMenuItem( "Edit", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	}

	//------------------------------------------------------------------------
	// Access to data structure with properties
	//------------------------------------------------------------------------
	LoadPrefsData& Data()
	{
		DBG_ASSERT(l_pPrefObject, "Load Prefs object not created yet.");
		return l_pPrefObject->m_Data;
	}

	//------------------------------------------------------------------------
	// Access to property object
	//------------------------------------------------------------------------
	prtyObject* GetDataObject()
	{
		return l_pPrefObject;
	}

}
