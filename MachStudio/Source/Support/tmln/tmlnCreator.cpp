/*****************************************************************************
**	tmlnCreator.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnCreator.hpp"

#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#include <vector>
#include <iomanip>
#include <sstream>


//============================================================================
//============================================================================
namespace
{
	std::vector<tmlnDriverCreator*> l_DriverCreators;

	struct driver_sort
	{
		inline bool operator()(tmlnDriverNameList::DriverName& lhs, tmlnDriverNameList::DriverName& rhs) 
		{ 
			return (lhs.m_Name < rhs.m_Name);

			//	use this one once the categories are displayed.
			//return ((lhs.m_Category <= rhs.m_Category) && (lhs.m_Name < rhs.m_Name)); 
		} 
	};

	//------------------------------------------------------------------------
	//	Sort the driver names
	//------------------------------------------------------------------------
	void sort_drivers(tmlnDriverNameList& io_Drivers)
	{
		// Sort the vector using predicate and std::sort  
		std::sort(io_Drivers.m_DriverNames.begin(), io_Drivers.m_DriverNames.end(), driver_sort()); 
	}
}


//--------------------------------------------------------------------
//	Add driver name to list
//--------------------------------------------------------------------
void tmlnDriverNameList::AddDriver(	const char* i_Name, 
									const char *i_Category,
									tmlnDriverCreator* i_Creator)
{
	DriverName driver_name;
	driver_name.m_Name = i_Name;
	driver_name.m_Category = i_Category;
	driver_name.m_Creator = i_Creator;
	m_DriverNames.push_back(driver_name);
}

//--------------------------------------------------------------------
// returns true if empty list
//--------------------------------------------------------------------
bool tmlnDriverNameList::Empty()
{
	return m_DriverNames.empty();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool tmlnDriverNameList::GetDriverName( const std::string& i_DriverName, tmlnDriverNameList::DriverName& o_DriverName ) const
{
	for (int i = 0; i < m_DriverNames.size(); ++i)
	{
		if ( strcmp(m_DriverNames[i].m_Name.c_str(), i_DriverName.c_str()) == 0 )
		{
			o_DriverName = m_DriverNames[i];
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------
//   Check if there are drivers in the place where the new driver(s)
//	are supposed to go.  If there are then ask the user if the 
//	current drivers should be overwritten or not.
//
//	Returns false if the user does not want the current drivers
//	overwritten.
//--------------------------------------------------------------------
bool tmlnDriverCreator::CheckIfOverwriteDrivers(tmlnScriptObject* io_pObject,
												std::vector<tmlnDriver*> i_ActiveDrivers )
{
	// FIX [rjk] remove this to reinstate the conflicting driver check
	return true;

	if ( i_ActiveDrivers.size() > 0 )
	{
		int num_active_drivers = i_ActiveDrivers.size();
		//char theText[512];
		//sprintf( theText,"Overwrite the current drivers\?\n" );
		//std::ostringstream oss;
		//oss<<"Overwrite the current drivers\?\n";
		std::string theText;
				
		int i;
		for ( i=0; i < num_active_drivers ; i++ )
		{
		//	sprintf( theText,"%s%02d %s (%6.3f to %6.3f) %s\n", theText,
		//								i,
		//								i_ActiveDrivers[i]->GetName().c_str(),
		//								i_ActiveDrivers[i]->GetBeginTime(),
		//								i_ActiveDrivers[i]->GetEndTime(), 
		//								i_ActiveDrivers[i]->GetCategory().c_str() );

			std::ostringstream oss;

			oss.setf(0, std::ios::floatfield);
			oss.setf(std::ios::fixed, std::ios::floatfield);
			std::string begin_time_str, end_time_str;
			tmlnTimeUtil::GetTimeString(i_ActiveDrivers[i]->GetBeginTime(), begin_time_str);
			tmlnTimeUtil::GetTimeString(i_ActiveDrivers[i]->GetEndTime(), end_time_str);
			oss << "Overwrite the current drivers\?\n"<<oss.width(2)<<std::setfill('0')<<i<<" "<<i_ActiveDrivers[i]->GetName()<<" ("<<oss.width(6)<<oss.precision(3)<<begin_time_str<<" to "<<end_time_str<<") "<<i_ActiveDrivers[i]->GetCategory();
			//strcpy((char*)theText.c_str(), oss.str().c_str());
			theText = oss.str();
		}

		//sprintf( theText,"%s\nYES = delete old and add new, NO = don't add new,\n CANCEL = add anyway with conflict\n", theText );
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss << theText << " \nYES = delete old and add new, NO = don't add new,\n CANCEL = add anyway with conflict\n";
		//strcpy((char*)theText.c_str(), oss.str().c_str());
		theText = oss.str();
		int retval = guiMessageBox::Show( theText.c_str(), "Driver Conflict", guiMessageBox::e_YesNoCancel );
		if ( retval == guiMessageBox::e_No )
		{
			return false;
		}

		if ( retval == guiMessageBox::e_Yes )
		{
			//	remove the drivers
			//
			for ( i=0; i < num_active_drivers ; i++ )
			{
				io_pObject->RemoveDriver( i_ActiveDrivers[i] );
			}
			io_pObject->NotifyDriverChanged();
		}
	}

	return true;
}


//--------------------------------------------------------------------
//   Check if there are drivers in the place where the new driver(s)
//	are supposed to go.  If there are then ask the user if the 
//	current drivers should be overwritten or not.
//
//	Returns false if the user does not want the current drivers
//	overwritten.
//--------------------------------------------------------------------

bool tmlnCreator::CheckIfOverwrite(tmlnScriptObject* io_pObject,
												std::vector<tmlnDriver*> i_ActiveDrivers )
{
	// FIX [rjk] remove this to reinstate the conflicting driver check
	return true;


	if ( i_ActiveDrivers.size() > 0 )
	{
		int num_active_drivers = i_ActiveDrivers.size();
		//char theText[512];
		//sprintf( theText,"Overwrite the current drivers\?\n" );
		//std::ostringstream oss;
		//oss<<"Overwrite the current drivers\?\n";
		std::string theText;
				


	
		int i;
		for ( i=0; i < num_active_drivers ; i++ )
		{
		//	sprintf( theText,"%s%02d %s (%6.3f to %6.3f) %s\n", theText,
		//								i,
		//								i_ActiveDrivers[i]->GetName().c_str(),
		//								i_ActiveDrivers[i]->GetBeginTime(),
		//								i_ActiveDrivers[i]->GetEndTime(), 
		//								i_ActiveDrivers[i]->GetCategory().c_str() );

			std::ostringstream oss;

			oss.setf(0, std::ios::floatfield);
			oss.setf(std::ios::fixed, std::ios::floatfield);
			std::string begin_time_str, end_time_str;
			tmlnTimeUtil::GetTimeString(i_ActiveDrivers[i]->GetBeginTime(), begin_time_str);
			tmlnTimeUtil::GetTimeString(i_ActiveDrivers[i]->GetEndTime(), end_time_str);
			oss << "Overwrite the current drivers\?\n"<<oss.width(2)<<std::setfill('0')<<i<<" "<<i_ActiveDrivers[i]->GetName()<<" ("<<oss.width(6)<<oss.precision(3)<<begin_time_str<<" to "<<end_time_str<<") "<<i_ActiveDrivers[i]->GetCategory();
			//strcpy((char*)theText.c_str(), oss.str().c_str());
			theText = oss.str();
		}

		//sprintf( theText,"%s\nYES = delete old and add new, NO = don't add new,\n CANCEL = add anyway with conflict\n", theText );
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss << theText << " \nYES = delete old and add new, NO = don't add new,\n CANCEL = add anyway with conflict\n";
		//strcpy((char*)theText.c_str(), oss.str().c_str());
		theText = oss.str();
		int retval = guiMessageBox::Show( theText.c_str(), "Driver Conflict", guiMessageBox::e_YesNoCancel );
		if ( retval == guiMessageBox::e_No )
		{
			return false;
		}

		if ( retval == guiMessageBox::e_Yes )
		{
			//	remove the drivers
			//
			for ( i=0; i < num_active_drivers ; i++ )
			{
				io_pObject->RemoveDriver( i_ActiveDrivers[i] );
			}
			io_pObject->NotifyDriverChanged();
		}
	}

	return true;
}

//--------------------------------------------------------------------
//	Deinitialize/Initialize
//--------------------------------------------------------------------
void tmlnCreator::Initialize()
{
}
void tmlnCreator::DeInitialize()
{
	envSTLHelpers::DeleteContainer(l_DriverCreators);
}

//--------------------------------------------------------------------
// Adds a method for creating drivers for a given object type
//--------------------------------------------------------------------
void tmlnCreator::AddDriverCreator(tmlnDriverCreator* i_pCreator)
{
	DBG_ASSERT( i_pCreator != 0, "Trying to add a NULL creator" );

	l_DriverCreators.push_back(i_pCreator);
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are accessed through the
//	altered DriverNameList which is appended to.
//--------------------------------------------------------------------
void tmlnCreator::GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
										tmlnDriverNameList& io_Drivers,
										bool i_bSortList /*= false*/)
{
	DBG_ASSERT( i_pObject != 0, "Trying to gather drivers with a NULL object" );

	for (size_t i=0; i<l_DriverCreators.size(); i++)
	{
		l_DriverCreators[i]->GatherPossibleDrivers(i_pObject, io_Drivers);
	}

	if (i_bSortList)
	{
		sort_drivers(io_Drivers);
	}
}

//--------------------------------------------------------------------
// GetDriverInfo - get driver info out of script object and into
//		vector of tmlnDriverInfo
//--------------------------------------------------------------------
void tmlnCreator::GetDriverInfo( const tmlnScriptObject* i_pObject,
								 std::vector<tmlnDriverInfo*> &o_Drivers )
{
	DBG_ASSERT( i_pObject != 0, "Trying to get driver info with a NULL object" );

	envSTLHelpers::DeleteContainer(o_Drivers);

	int num_drivers = i_pObject->GetNumDrivers();
	//DBG_LOG("Num drivers gotten: " << num_drivers);

	for (int i=0; i<num_drivers; i++)
	{
		o_Drivers.push_back(i_pObject->GetDriver(i).GetDriverInfo());
	}
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* tmlnCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
											  tmlnChannel* i_pChannel )
{
	DBG_ASSERT( io_pObject != 0, "Trying to create a driver with a NULL object" );
	DBG_ASSERT( i_pChannel != 0, "Trying to create a key for a NULL channel" );

	for (int i=0; i<l_DriverCreators.size(); i++)
	{
		tmlnDriver* pDriver = l_DriverCreators[i]->CreateKeyForChannel(io_pObject, i_pChannel);
		if (pDriver)
		{
			//DBG_LOG2("Created driver %d (%s)", io_pObject->GetNumDrivers(), i_pDriverInfo->m_Name.c_str() );
			io_pObject->AddDriver(pDriver);
			return pDriver;
		}
	}

	return 0;
}

//--------------------------------------------------------------------
//	CreateDriverFromInfo - create a driver for script object based on
//	tmlnDriverInfo and then adds it to the object.  The function 
//	returns a pointer to the driver if it was successfully created,
//	otherwise NULL is returned.
//--------------------------------------------------------------------
tmlnDriver* tmlnCreator::CreateDriverFromInfo(	tmlnScriptObject* io_pObject,
												const tmlnDriverInfo* i_pDriverInfo )
{
	DBG_ASSERT( io_pObject != 0, "Trying to create a driver with a NULL object" );

	for (int i=0; i<l_DriverCreators.size(); i++)
	{
		tmlnDriver* pDriver = l_DriverCreators[i]->CreateDriverFromInfo(io_pObject, *i_pDriverInfo);
		if (pDriver)
		{
			//DBG_LOG2("Created driver %d (%s)", io_pObject->GetNumDrivers(), i_pDriverInfo->m_Name.c_str() );
			io_pObject->AddDriver(pDriver);
			return pDriver;
		}
	}

	return NULL;
}


tmlnDriver* tmlnCreator::CreateDriverByName(const char* i_DriverName,
											tmlnScriptObject* io_pObject )

											
{
	DBG_ASSERT( io_pObject != 0, " Trying to create a driver with a NULL object" );
	for (int i=0; i<l_DriverCreators.size(); i++)
	{
		tmlnDriver* pDriver = l_DriverCreators[i]->CreateDriverByName( i_DriverName, io_pObject);
		if ( pDriver)
		{
			return pDriver;
		}
	}
	return NULL;
}
	
//--------------------------------------------------------------------
// Similar to CreateDriverFromInfo, this also passes in an index
// in order to put driver back into old index when restoring from undo.
//--------------------------------------------------------------------
tmlnDriver* tmlnCreator::RestoreDriverFromInfo(	tmlnScriptObject* io_pObject,
												const tmlnDriverInfo* i_pDriverInfo,
												int i_DriverIndex)
{
	DBG_ASSERT( io_pObject != 0, "Trying to create a driver with a NULL object" );

	for (int i=0; i<l_DriverCreators.size(); i++)
	{
		tmlnDriver* pDriver = l_DriverCreators[i]->CreateDriverFromInfo(io_pObject, *i_pDriverInfo);
		if (pDriver)
		{
			//DBG_LOG2("Created driver %d (%s)", io_pObject->GetNumDrivers(), i_pDriverInfo->m_Name.c_str() );
			io_pObject->RestoreDriver(pDriver, i_DriverIndex);
			return pDriver;
		}
	}

	return NULL;

}

//--------------------------------------------------------------------
//   SetDriverInfo - create drivers for script object based on
//		vector of tmlnDriverInfo
//--------------------------------------------------------------------
void tmlnCreator::SetDriverInfo( tmlnScriptObject* io_pObject,
								 const std::vector<tmlnDriverInfo*> &i_Drivers )
{
	DBG_ASSERT( io_pObject != 0, "Trying to attach a driver to a NULL object" );

	io_pObject->ClearDrivers();

	int num_drivers = i_Drivers.size();
	//DBG_LOG("Num drivers read: " << num_drivers);

	for (int d=0; d<num_drivers; d++)
	{
		//	get the info + try to create the driver.  If created,
		//	let's jump out of the inner loop
		//
		const tmlnDriverInfo* pInfo = i_Drivers[d];
		CreateDriverFromInfo( io_pObject, pInfo );
	}
}

//--------------------------------------------------------------------
//   CloneDriver - clone a driver and attach it to the object
//--------------------------------------------------------------------
tmlnDriver* tmlnCreator::CloneDriver(	tmlnScriptObject* io_pObject,
										const tmlnDriver* i_pDriver)
{
	DBG_ASSERT( i_pDriver != 0, "Trying to clone a NULL driver" );

	if (i_pDriver != 0)
	{
		const tmlnDriverInfo* pInfo = i_pDriver->GetDriverInfo();

		bool l_PrevCheckForConflict = io_pObject->GetCheckForConflicts();
		io_pObject->SetCheckForConflicts(false);
		tmlnDriver* pDriver = CreateDriverFromInfo( io_pObject, pInfo );
		io_pObject->SetCheckForConflicts(l_PrevCheckForConflict);

		delete pInfo;	// clean up the info pointer the code asked for above
		return pDriver;
	}
	else
	{
		DBG_ERROR("Tried to clone a driver, but the code was given a NULL driver");
	}
	
	return NULL;
}
