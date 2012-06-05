/*****************************************************************************
**	docDocumentType.hpp
**
**	 Internal type used by DocumentTypeMgr to represent a type
**	of document that can be created by the application
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef DOC_DOCUMENTTYPE_HPP
#error docDocumentType.hpp multiply included
#endif
#define DOC_DOCUMENTTYPE_HPP

#ifndef ENV_STLHELPERS_HPP
#include "Core/Env/envSTLHelpers.hpp"
#endif 

#include <string>
#include <vector>


//============================================================================
// forward declarations
//============================================================================
class docDocumentInterest;


//============================================================================
//============================================================================
class docDocumentType
{
public:
	std::string m_Name;
	std::string m_Extension;
	std::string m_Filter;
	std::vector<docDocumentInterest*> m_Interests;
	bool m_bAllowMultipleOpen;
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~docDocumentType()
	{
		envSTLHelpers::DeleteContainer(m_Interests);
	}

};
