/*****************************************************************************
**	dirltChannelLightPos.hpp
**
**	 Channel for altering position of dir lights
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef DIRLT_CHANNELLIGHTPOS_HPP
#error dirltChannelLightPos.hpp multiply included
#endif
#define DIRLT_CHANNELLIGHTPOS_HPP


#ifndef TMLN_CHANNELPOSITION_HPP
#include "tmlnChannelPosition.hpp"
#endif

//============================================================================
//============================================================================
class dirltDirLightObject;


//============================================================================
//============================================================================
class dirltChannelLightPos : public tmlnChannelPosition
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dirltChannelLightPos(const char* i_Name, dirltDirLightObject* i_pLight);

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
	dirltDirLightObject* m_pLight;
};
