/*****************************************************************************
**	propDriverAnimationFull.hpp
**
**	Derived driver class for full object animation
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_DRIVERANIMATIONFULL_HPP
#error propDriverAnimationFull.hpp multiply included
#endif
#define PROP_DRIVERANIMATIONFULL_HPP

#ifndef TMLN_DRIVERANIMATIONFULL_HPP
#include "Drivers/Animation/tmlnDriverAnimationFull.hpp"
#endif

//============================================================================
//============================================================================
class propDriverAnimationFull : public tmlnDriverAnimationFull
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	propDriverAnimationFull(tmlnChannelAnimationFull &i_Channel, 
							chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//	Find the full locator for the given animation filename.
	//--------------------------------------------------------------------
	virtual fsLocator GetAnimationLocator( const itString &i_AnimFilename ) const;

};
