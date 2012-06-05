/*****************************************************************************
**	cmraDriverCreator.hpp
**
**		Creates timeline drivers.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_DRIVERCREATOR_HPP
#error cmraDriverCreator.hpp multiply included
#endif
#define CMRA_DRIVERCREATOR_HPP

#ifndef TMLN_CREATOR_HPP
#include "Support/tmln/tmlnCreator.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

class cmraDriverMayaScript;
class cmraScriptObject;

//============================================================================
//============================================================================
class cmraDriverCreator : public tmlnDriverCreator
{
public:

	//--------------------------------------------------------------------
	// Create parsers for driver types created by this creator
	//--------------------------------------------------------------------
	static void CreateParsers();

	//--------------------------------------------------------------------
	// Create animation driver for this object, just a convenience
	// function.
	//--------------------------------------------------------------------
	static cmraDriverMayaScript* CreateAnimationDriver( cmraScriptObject* i_pObject );

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
	// Create static key driver for the given channel
	//--------------------------------------------------------------------
	virtual tmlnDriver* CreateKeyForChannel( tmlnScriptObject* io_pObject,
											 tmlnChannel* i_pChannel);

	//--------------------------------------------------------------------
	// Create driver for this object based on the the info structure
	//--------------------------------------------------------------------
	virtual tmlnDriver* CreateDriverFromInfo( tmlnScriptObject* io_pObject,
						const tmlnDriverInfo& i_Info);
};
