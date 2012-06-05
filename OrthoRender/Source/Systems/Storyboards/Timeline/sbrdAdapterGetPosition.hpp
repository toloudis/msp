/*****************************************************************************
**	sbrdAdapterGetPosition.hpp
**
**	 Adapter for setting position
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SBRD_ADAPTERGETPOSITION_HPP
#error sbrdAdapterGetPosition.hpp multiply included
#endif
#define SBRD_ADAPTERGETPOSITION_HPP

#ifndef TMLN_ADAPTERPOSITION_HPP
#include "Support/tmln/tmlnAdapterPosition.hpp"
#endif


//============================================================================
//============================================================================
class api3dObject;


//============================================================================
//============================================================================
class sbrdAdapterGetPosition : public tmlnAdapterGetPosition
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	sbrdAdapterGetPosition( api3dObject* i_pObject );

	//--------------------------------------------------------------------
	//  Get position of object, for localizing sounds
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const;

private:
	api3dObject* m_pObject;
};
