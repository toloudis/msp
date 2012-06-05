/****************************************************************************\
**	mdlModelingPackageListMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlModelingPackageListMgr.hpp"

#include "Core/it/itString.hpp"

#include <list>


//============================================================================
//============================================================================
namespace
{
	std::list<itString> m_List;
}

//------------------------------------------------------------------------
//	Clear the list
//------------------------------------------------------------------------
void mdlModelingPackageListMgr::Clear()
{
	m_List.clear();
}

//------------------------------------------------------------------------
//	Add a modeling package to the list
//
//	This should be a base name so it covers all versions of the package.
//	For instance, "Max" would cover all versions of 3D Studio Max.
//------------------------------------------------------------------------
void mdlModelingPackageListMgr::Add( itString& i_ModelingPackageName )
{
	m_List.push_back( i_ModelingPackageName );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool mdlModelingPackageListMgr::IsInList( itString& i_ModelingPackageName )
{
	if (m_List.size() > 0)
	{
		//	The list contains entries so see if the package is in the list
		//
		std::list<itString>::iterator it;
		for (it = m_List.begin(); it != m_List.end(); ++it)
		{
			//DBG_LOG((*it) << " - " << i_ModelingPackageName);

			if ((*it).HasSubString(i_ModelingPackageName))
				return true;
		}
		return false;	// item not found, this modeling package not allowed
	}

	//	the list is empty to accept everything
	return true;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool mdlModelingPackageListMgr::IsListEmpty()
{
	return (m_List.size() == 0);
}

 