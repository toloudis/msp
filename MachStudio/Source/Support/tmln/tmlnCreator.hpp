/*****************************************************************************
**	tmlnCreator.hpp
**
**		tmlnCreator handles the creation of channels and drivers for
**	different types of objects.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CREATOR_HPP
#error tmlnCreator.hpp multiply included
#endif
#define TMLN_CREATOR_HPP

#ifndef SEL3D_OBJECT_HPP
#include "Tool/sel3d/sel3dObject.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//	forward references
//============================================================================
class tmlnChannel;
class tmlnDriver;
class tmlnDriverInfo;
class tmlnScriptObject;

//============================================================================
//	Forward Declarations
//============================================================================
class tmlnDriverCreator;

//============================================================================
//============================================================================
class tmlnDriverNameList
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	struct DriverName
	{
		tmlnDriverCreator* m_Creator;
		std::string m_Name;
		std::string m_Category;
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddDriver(	const char* i_Name, 
					const char *i_Category,
					tmlnDriverCreator* i_Creator);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool Empty(); // returns true if empty list

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool GetDriverName( const std::string& i_DriverName, tmlnDriverNameList::DriverName& o_DriverName ) const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::vector<DriverName> m_DriverNames;
};


//============================================================================
//============================================================================
class tmlnDriverCreator
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~tmlnDriverCreator() {}

	//--------------------------------------------------------------------
	// Gather the possible types of drivers that can be created for
	//	for this object.  Driver names are added through the
	//	DriverNameList's API
	//--------------------------------------------------------------------
	virtual void GatherPossibleDrivers(	const tmlnScriptObject* i_pObject,
										tmlnDriverNameList& io_Drivers) = 0;

	//--------------------------------------------------------------------
	// Create driver for this object based on the name used in
	//	GatherPossibleDrivers and attach it to the channels
	//	on this script object.
	//--------------------------------------------------------------------
	virtual tmlnDriver* CreateDriverByName( const char* i_DriverName,
											tmlnScriptObject* i_pObject ) = 0;

	//--------------------------------------------------------------------
	// Create static key driver for the given channel
	//--------------------------------------------------------------------
	virtual tmlnDriver* CreateKeyForChannel( tmlnScriptObject* io_pObject,
											 tmlnChannel* i_pChannel) = 0;

	//--------------------------------------------------------------------
	// Create driver for this object based on the the info structure
	//--------------------------------------------------------------------
	virtual tmlnDriver* CreateDriverFromInfo(	tmlnScriptObject* io_pObject,
												const tmlnDriverInfo& i_Info) = 0;

	
	//--------------------------------------------------------------------
	//   Check if there are drivers in the place where the new driver(s)
	//	are supposed to go.  If there are then ask the user if the 
	//	current drivers should be overwritten or not.
	//
	//	Returns false if the user does not want the current drivers
	//	overwritten.
	//--------------------------------------------------------------------
	bool CheckIfOverwriteDrivers(	tmlnScriptObject* io_pObject,
									std::vector<tmlnDriver*> i_ActiveDrivers );
};

//============================================================================
//============================================================================
namespace tmlnCreator
{
	//--------------------------------------------------------------------
	//	Deinitialize/Initialize
	//--------------------------------------------------------------------
	void Initialize();
	void DeInitialize();

	//--------------------------------------------------------------------
	// Adds a method for creating drivers for a given object type
	//--------------------------------------------------------------------
	void AddDriverCreator(tmlnDriverCreator* i_pCreator);

	//--------------------------------------------------------------------
	// Gather the possible types of drivers that can be created for
	//	for this object.  Driver names are accessed through the
	//	altered DriverNameList which is appended to.
	//--------------------------------------------------------------------
	void GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
								tmlnDriverNameList& io_Drivers,
								bool i_bSortList = false);

	//--------------------------------------------------------------------
	// GetDriverInfo - get driver info out of script object and into
	//		vector of tmlnDriverInfo
	//--------------------------------------------------------------------
	void GetDriverInfo(	const tmlnScriptObject* i_pObject,
						std::vector<tmlnDriverInfo*> &o_Drivers );

	//--------------------------------------------------------------------
	//	CreateDriverFromInfo - create a driver for script object based on
	//	tmlnDriverInfo and then adds it to the object.  The function 
	//	returns a pointer to the driver if it was successfully created,
	//	otherwise NULL is returned.
	//--------------------------------------------------------------------
	tmlnDriver* CreateDriverFromInfo(	tmlnScriptObject* io_pObject,
										const tmlnDriverInfo* i_pDriverInfo );

	//-----------------------------------------------------------------------
	// CreateDriverByName 
	//-----------------------------------------------------------------------

	tmlnDriver* CreateDriverByName( const char* i_DriverName,
									tmlnScriptObject* io_pObject);

		//--------------------------------------------------------------------
	// Similar to CreateDriverFromInfo, this also passes in an index
	// in order to put driver back into old index when restoring from undo.
	//--------------------------------------------------------------------
	tmlnDriver* RestoreDriverFromInfo(	tmlnScriptObject* io_pObject,
										const tmlnDriverInfo* i_pDriverInfo,
										int i_DriverIndex);

	//--------------------------------------------------------------------
	//   SetDriverInfo - create drivers for script object based on
	//		vector of tmlnDriverInfo
	//--------------------------------------------------------------------
	void SetDriverInfo( tmlnScriptObject* io_pObject,
					    const std::vector<tmlnDriverInfo*> &i_Drivers );

	//--------------------------------------------------------------------
	//   CloneDriver - clone a driver and attach it to the object
	//--------------------------------------------------------------------
	tmlnDriver* CloneDriver(tmlnScriptObject* io_pObject,
							const tmlnDriver* i_pDriver);

	//--------------------------------------------------------------------
	// Create static key driver for the given channel
	//--------------------------------------------------------------------
	tmlnDriver* CreateKeyForChannel( tmlnScriptObject* io_pObject,
									 tmlnChannel* i_pChannel);

	//--------------------------------------------------------------------
	//   Check if there are drivers in the place where the new driver(s)
	//	are supposed to go.  If there are then ask the user if the 
	//	current drivers should be overwritten or not.
	//
	//	Returns false if the user does not want the current drivers
	//	overwritten.
	//--------------------------------------------------------------------
	bool CheckIfOverwrite(	tmlnScriptObject* io_pObject,
									std::vector<tmlnDriver*> i_ActiveDrivers );
};
