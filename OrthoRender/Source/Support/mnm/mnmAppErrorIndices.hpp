/*****************************************************************************
**  mnmAppErrorIndices.hpp
**
**	mnmAppErrorIndices contains the official application error indices.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_APPERRORINDICES_HPP
#error mnmAppErrorIndices.hpp multiply included
#endif
#define MNM_APPERRORINDICES_HPP

#ifndef ENV_PACKAGEERRORINDICES_HPP
#include "Core/env/envPackageErrorIndices.hpp"
#endif


//============================================================================
//============================================================================
namespace mnmAppErrorIndices
{	
	enum 
	{
		//------------------------------------------------------------------------
		//	Application Error Indices
		//------------------------------------------------------------------------
		e_Cptr = envPackageErrorIndices::e_BeginAppSpecificPackageErrorIndices,
	};
}

