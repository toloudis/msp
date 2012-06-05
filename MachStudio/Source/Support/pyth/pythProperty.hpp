/****************************************************************************\
**	pythProperty.hpp
**
**		Property related python commands
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_PROPERTY_HPP
#error pythProperty.hpp multiply included
#endif
#define PYTH_PROPERTY_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <string>

class prtyObject;

//============================================================================
//============================================================================
namespace pythProperty
{
	//--------------------------------------------------------------------
	// Class for providing a way to map certain strings to global
	// property objects in the application. Used when nameMgr 
	// cannot find an object by name.
	//--------------------------------------------------------------------
	class NameResolver
	{
	public:
		virtual ~NameResolver() {}
		virtual prtyObject* ResolveName(std::string &i_PropertyObjectName) = 0;
	};

	//--------------------------------------------------------------------
	// Add/Remove a name resolver.
	//--------------------------------------------------------------------
	void AddNameResolver(const shared_ptr<NameResolver> &i_Resolver);
	void RemoveNameResolver(const shared_ptr<NameResolver> &i_Resolver);

	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
