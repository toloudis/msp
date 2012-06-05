/*****************************************************************************
**	fgmtHighlight.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/fgmtHighlight.hpp"
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/fgmt/fgmtScriptObject.hpp"

#include "Support/mnm/mnmThinkInterest.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"

#include "Core/app/appTime.hpp"
#include "Graphics/an/an2StateAnimation.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"


//============================================================================
//============================================================================
namespace fgmtHighlight
{
	namespace
	{
		matMaterial* l_HighlightMaterial = NULL;

		const bool c_DoFlashAnimation = true;		// Should the flash animation be used?

		bool l_IsHilighted = false;					// Is highlighting fixed on
		bool l_bAnimatingHighlight = false;			// Flash animation of highlight
		float l_EndAnimationTime = 0.0f;			// When to turn off the flash animation
		const float c_FlashAnimDuration = 1.0f;		// How long the flash animation should last
		bool l_bJustFinishedHighlight = false;		// One frame past highlight change scene needs to be re-rendered

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void highlight_fragment(fgmtScriptObject* i_pObject,
								int i_Index,
								bool i_bOn)
		{
			if (i_pObject != NULL &&
				i_Index > -1 && 
				i_Index < i_pObject->GetNumFragments())
			{
				i_pObject->HighlightFragment(i_Index, fgmtHighlight::GetHighlightMaterial(), i_bOn);
			}		
		}


		//--------------------------------------------------------------------
		// Switching between the flash animation and regular highlight
		// should also turn on and off the material animation.
		// (The flash animation does not animate the highlight 
		//	material color also)
		//--------------------------------------------------------------------
		void set_flash_animation(bool i_bOnOff)
		{
			l_bAnimatingHighlight = i_bOnOff;
			if (l_HighlightMaterial)
			{
				const int material_anim_index = 0;
				if (i_bOnOff)
				{
					l_HighlightMaterial->DeactivateMatAnim(material_anim_index);
				}
				else
				{
					l_HighlightMaterial->ActivateMatAnim(material_anim_index);
				}
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void highlight_all_selected( bool i_bIsHilighted )
		{
			const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
			std::list<sel3dObject*>::const_iterator sit;
			for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
			{
				if ( fgmtScriptObject *pSurfaceObj = sel3dCastUtil::CastPickObject<fgmtScriptObject>(*sit) )
				{
					// This case handles selection of a fragment pick object
					if ( fgmtPropertyObject *pPropObj = sel3dCastUtil::CastPickObject<fgmtPropertyObject>(*sit) )
					{
						highlight_fragment(pSurfaceObj, pSurfaceObj->GetIndexForName(pPropObj->GetName()), i_bIsHilighted);
					}
				}
			}
		}

		//============================================================================
		// Animation code for flash of highlight on selection changes
		//============================================================================
		class FlashThinkInterest : public mnmThinkInterest
		{
		public:
			//--------------------------------------------------------------------
			//	Think
			//--------------------------------------------------------------------
			virtual void Think()
			{
				if (l_bAnimatingHighlight)
				{
					float time = appTime::GetTime();
					if (time >= l_EndAnimationTime)
					{
						set_flash_animation(false);
						highlight_all_selected(false);

						// track last frame where highlight was turned off
						// so that the rendering does one more render before turning off again
						l_bJustFinishedHighlight = true;
					}
				}
			}
		};
		FlashThinkInterest l_ThinkInterest;

	}	// end of namespace

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize()
	{
		l_HighlightMaterial = new matMaterial(maFloatRGBA(0,0,0,1),
			maFloatRGBA(0,0,0,0),
			maFloatRGBA(0.2f, 0.2f, 0.7f, 1.0f));

		an2StateAnimation<maFloatRGBA>* color_anim = 
				new an2StateAnimation<maFloatRGBA>(	
						maFloatRGBA(0.2f, 0.2f, 0.7f, 1.0f),
						maFloatRGBA(0.5f, 0.5f, 0.7f, 1.0f),
						1.0f);
		color_anim->SetLooping(true);
		color_anim->SetReversing(true);

		matMatAnim* sel_anim = new matMatAnim(color_anim, matMatParamIndex::e_Emissive, 0.0f);
		sel_anim->SetUseRealTime(true);
		l_HighlightMaterial->AddMatAnim(sel_anim);

		if (c_DoFlashAnimation)
		{
			// register the think interest
			mnmThinkMgr::RegisterThinkInterest( &l_ThinkInterest );
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize()
	{
		if (c_DoFlashAnimation)
		{
			// unregister the think interest
			mnmThinkMgr::UnRegisterThinkInterest( &l_ThinkInterest );
		}

		delete l_HighlightMaterial;
		l_HighlightMaterial = NULL;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	matMaterial* GetHighlightMaterial()
	{
		return l_HighlightMaterial;
	}

	//--------------------------------------------------------------------
	// Change highlight state on the given fragment
	//--------------------------------------------------------------------
	void  HighlightFragmentIndex(fgmtScriptObject* i_pObject, int i_Index, bool i_bOnOff)
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		if (l_IsHilighted || l_bAnimatingHighlight)
			highlight_fragment(i_pObject, i_Index, i_bOnOff);
	}

	//--------------------------------------------------------------------
	//	Toggle hilighting of current fragment.
	//--------------------------------------------------------------------
	void HighlightFragment(bool i_On)
	{
		//if( sel3dMgr::getSelectionLock() )
		//	return;

		// If we were in the middle of a flash animation already, then
		// we don't need to turn on the actual material highlight
		// because it is already on.

		bool bPreviousHighlight = (l_IsHilighted || l_bAnimatingHighlight);
		if (bPreviousHighlight != i_On)
		{
			// stop any render threads
			gpxRenderControl::ConfirmSingleThread();

			highlight_all_selected(i_On);
		}
			
		l_IsHilighted = i_On;

		// Turn off any flash animation immediately
		set_flash_animation(false);
	}
	bool IsHighlightFragment()
	{
		return l_IsHilighted;
	}

	//--------------------------------------------------------------------
	// Start a flash of the material highlighting for a short period 
	// of time. Call this when a material is selected.
	//--------------------------------------------------------------------
	void BeginHighlightFlashAnimation()
	{
		if (c_DoFlashAnimation)
		{
			// Flash animation is only needed when we aren't already 
			// highlighting the selection
			if (!l_IsHilighted)
			{
				if (!l_bAnimatingHighlight)
				{
					// Turn on the highlight temporarily
					set_flash_animation(true);
					highlight_all_selected(true);
				}

				// Even if the flash animation was already on, reset
				// the new end time so that it stays on longer
				l_EndAnimationTime = appTime::GetTime() + c_FlashAnimDuration;
			}
		}
	}

	//--------------------------------------------------------------------
	// Check to see if any surfaces have highlight animations
	//--------------------------------------------------------------------
	bool AreSurfacesHighlighted()
	{
		if (l_IsHilighted || l_bAnimatingHighlight || l_bJustFinishedHighlight) 
		{
			l_bJustFinishedHighlight = false;
			const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
			std::list<sel3dObject*>::const_iterator sit;
			for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
			{
				// If a fragment pick object is in the selection list, then it is highlighted.
				if ( fgmtPropertyObject *pPropObj = sel3dCastUtil::CastPickObject<fgmtPropertyObject>(*sit) )
					return true;
			}
		}
		return false;
	}

}	// end of namespace
