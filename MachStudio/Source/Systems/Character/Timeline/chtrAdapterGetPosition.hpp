/*****************************************************************************
**	chtrAdapterGetPosition.hpp
**
**	 Adapter for setting character animation
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef CHTR_ADAPTERGETPOSITION_HPP
#error chtrAdapterGetPosition.hpp multiply included
#endif
#define CHTR_ADAPTERGETPOSITION_HPP

#ifndef TMLN_ADAPTERPOSITION_HPP
#include "Support/tmln/tmlnAdapterPosition.hpp"
#endif

class api3dObject;

class chtrAdapterGetPosition : public tmlnAdapterGetPosition
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	chtrAdapterGetPosition( api3dObject* i_pObject );

	//--------------------------------------------------------------------
	//  Get position of object, for localizing sounds
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const;

private:
	api3dObject* m_pCharacter;
};
