/*****************************************************************************\
**	pick3dMgr.hpp
**
**		the pick system manager.  This controls all the pick interests for
**	the system.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef PICK3D_MGR_HPP
#error pick3dMgr.hpp multiply included
#endif
#define PICK3D_MGR_HPP

#ifndef PICK3D_PICKINTEREST_HPP
#include "Tool/pick3d/pick3dPickInterest.hpp"
#endif
#ifndef PICK3D_PICKITEM_HPP
#include "Tool/pick3d/pick3dPickItem.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class geoPickRay;


//============================================================================
//============================================================================
namespace pick3dMgr
{
//	public:
		//--------------------------------------------------------------------
		//	RegisterPickInterest() - add a pick interest to the system
		//--------------------------------------------------------------------
		void RegisterPickInterest( pick3dPickInterest* i_pInterest );

		//--------------------------------------------------------------------
		//	UnRegisterPickInterest() - remove a pick interest from the system.
		//
		//	Note: this will NOT delete the pick interest.  It is up to the
		//	registerer.
		//--------------------------------------------------------------------
		void UnRegisterPickInterest( pick3dPickInterest* i_pInterest );

		//--------------------------------------------------------------------
		//	Clear() - clear the list
		//--------------------------------------------------------------------
		void Clear();
		
		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode);
}

