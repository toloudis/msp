/*****************************************************************************
**	dirltDriverCreator.hpp
**
**	Creates timeline channels for dir lights
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef DIRLT_DRIVERCREATOR_HPP
#error dirltDriverCreator.hpp multiply included
#endif
#define DIRLT_DRIVERCREATOR_HPP


#ifndef TMLN_CREATOR_HPP
#include "tmlnCreator.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "chDefs.hpp"
#endif


class dirltDriverCreator : public tmlnDriverCreator
{
public:

	//--------------------------------------------------------------------
	// Create parsers for driver types created by this creator
	//--------------------------------------------------------------------
	static void CreateParsers();

	//--------------------------------------------------------------------
	// Gather the possible types of drivers that can be created for
	//	for this object.  Driver names are added through the
	//	DriverNameList's API
	//--------------------------------------------------------------------
	virtual void GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
						tmlnDriverNameList& io_Drivers);

	//--------------------------------------------------------------------
	// Create driver for this object based on the name used in
	//	GatherPossibleDrivers and attach it to the channels
	//	on this script object.
	//--------------------------------------------------------------------
	virtual tmlnDriver* CreateDriverByName( const char* i_DriverName,
				tmlnScriptObject* i_pObject );

	//--------------------------------------------------------------------
	// Create driver for this object based on the the info structure
	//--------------------------------------------------------------------
	virtual tmlnDriver* CreateDriverFromInfo( tmlnScriptObject* io_pObject,
						const tmlnDriverInfo& i_Info);
};
