/****************************************************************************\
**  g2dResetHandler.hpp
**
**      g2dResetHandler.hpp defines an object which will be notified when
**	the D3D device is going to be reset.  This is used for things that need
**	to destroy and reallocate their device dependent resources.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_RESETHANDLER_HPP
#error g2dResetHandler.hpp multiply included
#endif
#define G2D_RESETHANDLER_HPP


//============================================================================
//============================================================================
class g2dResetHandler
{
	public:
		//------------------------------------------------------------------------
		//	Deallocate is called when all device dependent resources should be
		//	released.
		//------------------------------------------------------------------------
		virtual void Deallocate() = 0;

		//------------------------------------------------------------------------
		//	Allocate is called when the device has been Reset and resources can
		//	be reloaded again.
		//------------------------------------------------------------------------
		virtual void Reallocate() = 0;

		//----------------------------------------------------------------------------
		//	functions to deal with the list of g2dResetHandlers.  The
		//	g2dResetHandlers must be allocated on the heap as they will be deleted
		//	later (by the DestroyResetHandlers functions).
		//----------------------------------------------------------------------------
		static void AddResetHandler(g2dResetHandler* i_Handler);
		static void DestroyResetHandlers();
		static void ResetHandlerDeallocate();
		static void ResetHandlerReallocate();
};
