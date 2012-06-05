/*****************************************************************************
**  cptrModeRenderQuick.hpp
**
**      The Quick Render mode.  This render uses different capture settings
**	than the regular render mode.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_MODERENDERQUICK_HPP
#error cptrModeRenderQuick.hpp multiply included
#endif
#define CPTR_MODERENDERQUICK_HPP

#ifndef CPTR_MODERENDER_HPP
#include "Features/Capture/cptrModeRender.hpp"
#endif


//============================================================================
//============================================================================


//============================================================================
//============================================================================
class cptrModeRenderQuick : public cptrModeRender
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrModeRenderQuick(modeModeID i_ModeIDCapture);

		//--------------------------------------------------------------------
		// pure-virtual, children must be created
		//--------------------------------------------------------------------
		virtual ~cptrModeRenderQuick();

		//--------------------------------------------------------------------
		//	Initialize will be called before the first call of Think after
		//	the object is first created or DeInitialized.  During the
		//	lifetime of a mode, Initialize and DeInitialize may be called
		//	several times.  Children of appMode should remember to call
		//	appMode::Initialize() at the beginning of their Initialize
		//	function.
		//--------------------------------------------------------------------
		void Initialize();

		//--------------------------------------------------------------------
		//	The object should clean up things that are not needed while the
		//	mode is not running in the DeInitialize function.
		//--------------------------------------------------------------------
		void DeInitialize();

		//--------------------------------------------------------------------
		//	PostCapture
		//--------------------------------------------------------------------
		virtual void BeginStatePostCapture();

	protected:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual void ResetConfigFileName();

	private:
		modeModeID	m_ModeRenderID;

};


