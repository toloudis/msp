/*****************************************************************************
**	propAdapterGetPosition.hpp
**
**	 Adapter for setting prop animation
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef PROP_ADAPTERGETPOSITION_HPP
#error propAdapterGetPosition.hpp multiply included
#endif
#define PROP_ADAPTERGETPOSITION_HPP

#ifndef TMLN_ADAPTERPOSITION_HPP
#include "Support/tmln/tmlnAdapterPosition.hpp"
#endif

class api3dObjectEntity;

class propAdapterGetPosition : public tmlnAdapterGetPosition
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	propAdapterGetPosition( api3dObjectEntity* i_pObject );

	//--------------------------------------------------------------------
	//  Get position of object, for localizing sounds
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const;

private:
	api3dObjectEntity* m_pProp;
};
