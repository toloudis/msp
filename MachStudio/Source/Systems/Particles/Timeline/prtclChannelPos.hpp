/*****************************************************************************
**	prtclChannelPos.hpp
**
**	 Channel for altering position of particle generators
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_CHANNELPOS_HPP
#error prtclChannelPos.hpp multiply included
#endif
#define PRTCL_CHANNELPOS_HPP

#ifndef TMLN_CHANNELPOSITION_HPP
#include "Support/tmln/tmlnChannelPosition.hpp"
#endif


//============================================================================
//============================================================================
class prtyPoint3d;
class prtclObject;


//============================================================================
//============================================================================
class prtclChannelPos : public tmlnChannelPosition
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	prtclChannelPos(const char* i_Name, prtyPoint3d& i_Property);

	//--------------------------------------------------------------------
	//  Set new position for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetPosition(const maPoint3d &i_Pos);

	//--------------------------------------------------------------------
	//  Get position of object, for blending with current value
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const;

private:
	prtyPoint3d& m_Property;
};
