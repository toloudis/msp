/*****************************************************************************
**	scrtyaMgr.cpp
**
**		see .hpp
**
**	this project includes the following libs:
**		libhasp_cpp_windows_mtd_msc8.lib libhasp_windows_87761.lib
**		One comes from HASP, the other gets built based on our corporate
**		ID given to us by HASP.
**
**	the define USE_SECURITYDONGLE is set in the gold build of this library only.
**	
**	The main idea behind the security system is to not make it easy to debug.
**	When an dongle is looked for and not detected, a flag is set and the error
**	shows up elsewhere in the program making it harder to track down.
**	
**	
**		From Aladdin
**	
**	The Best Practices for protecting your application are listed in the HASP SRM Guide, chapter 6. http://aladdin.com/hs#docs
**	
**	Some tips off-hand would be the following:
**	
**	1) Use hasp_encrypt()/hasp_decrypt() for encrypting application variables to make sure the key is connected, since it uses the AES key stored on the HASP key.
**	2) Using hasp_get_info() doesn't log into the key, but will check for if the key is present, and also pull additional information about the key, license, etc. 
**	
**	(ftp://ftp.aladdin.com/pub/hasp/srm/Documentation/HASP_SRM_Guide.pdf) chapter 5 + 6
**	
**	Vary Behavior when Cracking Attempt is Detected
**	
**	When a cracking attempt is detected (for example, through using a
**	checksum—described later in the chapter), delay the reactive behavior
**	of your software, thus breaking the logical connection between
**	“cause” and “effect”. Delayed reaction confuses a software cracker by
**	obscuring the link between the cracking attempt and the negative
**	reaction of the software to that attempt.
**	Behavior such as impairing program functionality when a cracking
**	attempt is detected can be very effective. Additional behaviors could
**	include causing the program to crash, overwriting data files, or
**	deliberately causing the program to become inaccurate, causing the
**	program to become undependable.
**	
**	Insert Multiple Calls in your Code
**	
**	Inserting many calls, throughout the code, to the HASP SRM
**	protection key in order to check the presence of the key, and binding
**	data from the key with the software functionality, frustrates those
**	attempting to crack your software. Multiple calls increase the
**	difficulty in tracing a protection scheme.
**	You can also add obstacles to a potential software cracker’s progress
**	by encrypting data that has no bearing on the application. Similarly,
**	you can divert attention by generating “noise” through random
**	number generators, time values, intermediate results of calculations,
**	and other mechanisms that do not lead to meaningful results or
**	actions.
**	
**	Encrypt/Decrypt Data with a HASP SRM Protection Key
**	
**	Encryption and decryption processes are performed inside a
**	HASP SRM protection key, well beyond the reach of any debugging
**	utility.
**	Encrypting data with the HASP SRM AES based encryption engine
**	considerably enhances software security. By encrypting data used by
**	your application, the decryption process depends on both the
**	presence of a HASP SRM protection key and its internal intelligence.
**	By implementing a HASP SRM Runtime API scheme in which data is
**	decrypted by a HASP SRM protection key, the association between
**	the protected application and the HASP SRM protection key cannot
**	easily be removed. Cracking the software also necessitates the
**	software cracker decrypting the data.
**	Use a Checksum to Verify Integrity of Executable Files
**	Compare the value in the executable file with a checksum stored in
**	HASP SRM protection key memory. If the two values are not equal,
**	you can assume that someone has attempted to modify the files.
**	Repeat this check in various places in the code, varying it in each place
**	to make it more difficult for a software cracker to detect.
**	
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "SecurityAladdin/scrty/scrtyaMgr.hpp"

#include "Core/scrty/scrtyDongleX.hpp"
#include "Core/app/appTimeUtils.hpp"
#include "Core/dbg/dbgMsg.hpp"

#ifdef USE_SECURITYDONGLE
//#include "SecurityAladdin/scrty/hasp_api.h"
#include "SecurityAladdin/scrty/hasp_api_cpp.h"
#include "SecurityAladdin/scrty/vendor_code.h"
#include "SecurityAladdin/scrty/errorprinter.h"
#endif

#include <time.h>


//==============================================================================
//	library pragmas
//==============================================================================
#ifdef USE_SECURITYDONGLE
#pragma comment(lib,"libhasp_cpp_windows_mtd_msc8_d.lib")
#pragma comment(lib,"libhasp_windows_87761.lib")
#endif


//============================================================================
//============================================================================
namespace
{
	//	access and usage types
	const int lc_ACCESSTYPE_FULL	= 0;
	const int lc_ACCESSTYPE_LIMITED = 1;
	const int lc_USAGETYPE_FULL		= 0;
	const int lc_USAGETYPE_DAYS		= 1;
	const int lc_USAGETYPE_COUNT	= 2;
	const int lc_USAGETYPE_MINUTES	= 3;

	//	dongle memory indexes
	const int lc_MAX_DONGLE_BYTES		= 112;
	const int lc_SIZE_DONGLE_VERSION	= 2;	const int lc_INDEX_DONGLE_VERSION	= 0;
	const int lc_SIZE_USAGE_TYPE		= 1;	const int lc_INDEX_USAGE_TYPE		= lc_INDEX_DONGLE_VERSION	+ lc_SIZE_DONGLE_VERSION;
	const int lc_SIZE_ACCESS_TYPE		= 1;	const int lc_INDEX_ACCESS_TYPE		= lc_INDEX_USAGE_TYPE		+ lc_SIZE_USAGE_TYPE;
	const int lc_SIZE_COMPANY_ID		= 10;	const int lc_INDEX_COMPANY_ID		= lc_INDEX_ACCESS_TYPE		+ lc_SIZE_ACCESS_TYPE;
	const int lc_SIZE_ACTIVATION_DATE	= 8;	const int lc_INDEX_ACTIVATION_DATE	= lc_INDEX_COMPANY_ID		+ lc_SIZE_COMPANY_ID;
	const int lc_SIZE_FEATURES			= 8;	const int lc_INDEX_FEATURES			= lc_INDEX_ACTIVATION_DATE	+ lc_SIZE_ACTIVATION_DATE;
	const int lc_SIZE_USAGE_COUNT		= 10;	const int lc_INDEX_USAGE_COUNT		= lc_INDEX_FEATURES			+ lc_SIZE_FEATURES;
	const int lc_SIZE_USED_MINS			= 4;	const int lc_INDEX_USED_MINS		= lc_INDEX_USAGE_COUNT		+ lc_SIZE_USAGE_COUNT;
	const int lc_SIZE_DAYS_VALID		= 4;	const int lc_INDEX_DAYS_VALID		= lc_INDEX_USED_MINS		+ lc_SIZE_USED_MINS;
	const int lc_SIZE_USES_LEFT			= 4;	const int lc_INDEX_USES_LEFT		= lc_INDEX_DAYS_VALID		+ lc_SIZE_DAYS_VALID;
	const int lc_SIZE_MINUTES_VALID		= 4;	const int lc_INDEX_MINUTES_VALID	= lc_INDEX_USES_LEFT		+ lc_SIZE_USES_LEFT;
	const int lc_SIZE_VALUE				= 4;	const int lc_INDEX_VALUE			= lc_INDEX_MINUTES_VALID	+ lc_SIZE_MINUTES_VALID;
	const int lc_SIZE_INVALID_COUNTER	= 4;	const int lc_INDEX_INVALID_COUNTER	= lc_INDEX_VALUE			+ lc_SIZE_VALUE;
	const int lc_SIZE_STATUS			= 4;	const int lc_INDEX_STATUS			= lc_INDEX_INVALID_COUNTER	+ lc_SIZE_INVALID_COUNTER;
	const int lc_SIZE_LAST_ACCESS_DATE	= 8;	const int lc_INDEX_LAST_ACCESS_DATE	= lc_INDEX_STATUS			+ lc_SIZE_STATUS;
	const int lc_SIZE_VERSION_MAJOR		= 1;	const int lc_INDEX_VERSION_MAJOR	= lc_INDEX_LAST_ACCESS_DATE + lc_SIZE_LAST_ACCESS_DATE;
	const int lc_SIZE_VERSION_MINOR		= 1;	const int lc_INDEX_VERSION_MINOR	= lc_INDEX_VERSION_MAJOR	+ lc_SIZE_VERSION_MAJOR;
	const int lc_SIZE_VERSION_REVISION	= 1;	const int lc_INDEX_VERSION_REVISION	= lc_INDEX_VERSION_MINOR	+ lc_SIZE_VERSION_MINOR;
	const int lc_SIZE_VERSION_INCREMENT	= 1;	const int lc_INDEX_VERSION_INCREMENT= lc_INDEX_VERSION_REVISION + lc_SIZE_VERSION_REVISION;
	const int lc_SIZE_APP_ID			= 1;	const int lc_INDEX_APP_ID			= lc_INDEX_VERSION_INCREMENT+ lc_SIZE_VERSION_INCREMENT;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	struct haspkey_memory
	{
		unsigned short dongle_version;	//	[2]  {01}	version of dongle data
		unsigned char usage_type;		//	[1]  {0}	(1=days valid, 2=usage count, 3=minutes)
		unsigned char access_type;		//	[1]  {0}	(0=full, 1=limited1, 2=limited2)
		unsigned int companyID;			//	[10] {0000000000}	company ID
		unsigned int activation_date;	//	[4]  {0000}			Initially 0 (Julian)
		unsigned int features;			//	[8]  {88888888}		bits
		unsigned int usage_count;		//	[10] {0000000000}	number of times used
		unsigned int used_mins;			//	[4]  {0000}	number of minutes used
		unsigned int days_valid;		//	[4]  {9999}	number of days valid
		unsigned int uses_left;			//	[4]  {9999}	gets decremented
		unsigned int minutes_valid;		//	[4]  {9999}	usage minutes
		unsigned int value; 			//	[4]  {0000}	always even
		unsigned int invalid_counter;	//	[4]  {9999}	(gets set when dongle becomes invalid and counts down uses)
		unsigned int status;			//	[4]  {0892}	(if < 500 invalid dongle, if > 500 valid dongle)
		unsigned int last_access_date;	//	[4]  {0000}	last time software was used
		unsigned char ver_major;		//	[1]  {1}	application version major.minor.revision.increment
		unsigned char ver_minor;		//	[1]  {0}		
		unsigned char ver_revision;		//	[1]  {0}		
		unsigned char ver_increment;	//	[1]  {1}		
		unsigned char app_ID;			//	[1]  {1}	application ID to differentiate dongles from same company
	};

	bool			l_bDongleDataRead = false;
	haspkey_memory	l_MemData;

#ifdef USE_SECURITYDONGLE
    Chasp *l_Hasp;
#endif

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int get_today_julian()
{
	// TODO - if dongle has date/time on it, use that instead.
	time_t t = time(0);
	tm* lt = localtime(&t);
	return appTimeUtils::DateToJulian( lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int convert_chars_to_int( unsigned char* i_CharList, int i_Index, int i_NumDigits )
{
	// TODO - check bounds + valid data
	// TODO - is this affected by UNICODE systems?
	//
	char chardigits[12];
	memcpy(&chardigits[0], &(i_CharList[i_Index]), i_NumDigits);
	chardigits[i_NumDigits] = 0;
	int ctotal = atoi(&chardigits[0]);
	DBG_TRACE("converted to " << ctotal);
	return ctotal;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void convert_int_to_chars( int i_Value, unsigned char* o_CharList, int i_Index, int i_NumDigits )
{
	// TODO - check bounds + valid data
	// TODO - is this affected by UNICODE systems?
	//
	memset(&o_CharList[i_Index], '\0', i_NumDigits);
	_itoa(i_Value, (char*)(&o_CharList[i_Index]), 10);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void update_data( unsigned char* o_CharList )
{
	// TODO - check bounds + valid data
	// TODO - is this affected by UNICODE systems?
	//
	convert_int_to_chars( l_MemData.dongle_version, &o_CharList[0], lc_INDEX_DONGLE_VERSION, lc_SIZE_DONGLE_VERSION );
	convert_int_to_chars( l_MemData.usage_type, &o_CharList[0], lc_INDEX_USAGE_TYPE, lc_SIZE_USAGE_TYPE );
	convert_int_to_chars( l_MemData.access_type, &o_CharList[0], lc_INDEX_ACCESS_TYPE, lc_SIZE_ACCESS_TYPE );
	convert_int_to_chars( l_MemData.companyID, &o_CharList[0], lc_INDEX_COMPANY_ID, lc_SIZE_COMPANY_ID );
	convert_int_to_chars( l_MemData.activation_date, &o_CharList[0], lc_INDEX_ACTIVATION_DATE, lc_SIZE_ACTIVATION_DATE );
	convert_int_to_chars( l_MemData.features, &o_CharList[0], lc_INDEX_FEATURES, lc_SIZE_FEATURES );
	convert_int_to_chars( l_MemData.usage_count, &o_CharList[0], lc_INDEX_USAGE_COUNT, lc_SIZE_USAGE_COUNT );
	convert_int_to_chars( l_MemData.used_mins, &o_CharList[0], lc_INDEX_USED_MINS, lc_SIZE_USED_MINS );
	convert_int_to_chars( l_MemData.days_valid, &o_CharList[0], lc_INDEX_DAYS_VALID, lc_SIZE_DAYS_VALID );
	convert_int_to_chars( l_MemData.uses_left, &o_CharList[0], lc_INDEX_USES_LEFT, lc_SIZE_USES_LEFT );
	convert_int_to_chars( l_MemData.minutes_valid, &o_CharList[0], lc_INDEX_MINUTES_VALID, lc_SIZE_MINUTES_VALID );
	convert_int_to_chars( l_MemData.value, &o_CharList[0], lc_INDEX_VALUE, lc_SIZE_VALUE );
	convert_int_to_chars( l_MemData.invalid_counter, &o_CharList[0], lc_INDEX_INVALID_COUNTER, lc_SIZE_INVALID_COUNTER );
	convert_int_to_chars( l_MemData.status, &o_CharList[0], lc_INDEX_STATUS, lc_SIZE_STATUS );
	convert_int_to_chars( l_MemData.last_access_date, &o_CharList[0], lc_INDEX_LAST_ACCESS_DATE, lc_SIZE_LAST_ACCESS_DATE );
	convert_int_to_chars( l_MemData.ver_major, &o_CharList[0], lc_INDEX_VERSION_MAJOR, lc_SIZE_VERSION_MAJOR );
	convert_int_to_chars( l_MemData.ver_minor, &o_CharList[0], lc_INDEX_VERSION_MINOR, lc_SIZE_VERSION_MINOR );
	convert_int_to_chars( l_MemData.ver_revision, &o_CharList[0], lc_INDEX_VERSION_REVISION, lc_SIZE_VERSION_REVISION );
	convert_int_to_chars( l_MemData.ver_increment, &o_CharList[0], lc_INDEX_VERSION_INCREMENT, lc_SIZE_VERSION_INCREMENT );
	convert_int_to_chars( l_MemData.app_ID, &o_CharList[0], lc_INDEX_APP_ID, lc_SIZE_APP_ID );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void output_memory()
{
#ifdef _DEBUG
	DBG_TRACE( "SECURITY MEMORY DUMP" );
	DBG_TRACE( "--------------------" );
	DBG_TRACE( "dongle_version = " << l_MemData.dongle_version );
	DBG_TRACE( "usage_type = " << (short)l_MemData.usage_type );
	DBG_TRACE( "access_type = " << (short)l_MemData.access_type );
	DBG_TRACE( "companyID = " << l_MemData.companyID );
	DBG_TRACE( "activation_date = " << l_MemData.activation_date );
	DBG_TRACE( "features = " << l_MemData.features );
	DBG_TRACE( "usage_count = " << l_MemData.usage_count );
	DBG_TRACE( "used_mins = " << l_MemData.used_mins );
	DBG_TRACE( "days_valid = " << l_MemData.days_valid );
	DBG_TRACE( "uses_left = " << l_MemData.uses_left );
	DBG_TRACE( "minutes_valid = " << l_MemData.minutes_valid );
	DBG_TRACE( "value = " << l_MemData.value );
	DBG_TRACE( "invalid_counter = " << l_MemData.invalid_counter );
	DBG_TRACE( "status = " << l_MemData.status );
	DBG_TRACE( "last_access_date = " << l_MemData.last_access_date );
	DBG_TRACE( "ver_major = " << (short)l_MemData.ver_major );
	DBG_TRACE( "ver_minor = " << (short)l_MemData.ver_minor );
	DBG_TRACE( "ver_revision = " << (short)l_MemData.ver_revision );
	DBG_TRACE( "ver_increment = " << (short)l_MemData.ver_increment );
	DBG_TRACE( "app_ID = " << (short)l_MemData.app_ID );
#endif
}

};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void scrtyaMgr::Init()
{
#ifdef USE_SECURITYDONGLE
	l_bDongleDataRead = false;
	l_Hasp = new Chasp(ChaspFeature::defaultFeature());

	haspStatus status = l_Hasp->login(vendorCode);
	if (!HASP_SUCCEEDED(status))
	{
		//handle error
		throw scrtyAPIFailedX();
	}

	//	Read in the dongle data
	OpenDongle();

#ifdef _DEBUG
	scrtyaMgr::TestDongle();
#endif
#endif
	return;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void scrtyaMgr::CleanUp()
{
#ifdef USE_SECURITYDONGLE
	if (l_Hasp != NULL)
	{
		haspStatus status = l_Hasp->logout();
		if (!HASP_SUCCEEDED(status))
		{
			//handle error
			throw scrtyAPIFailedX();
		}

		l_Hasp = NULL;
	}

#endif
	return;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool ReadDongle(unsigned char o_Data[])
{
#ifdef USE_SECURITYDONGLE
	//	Read in the Data
	//
	ChaspFile file = l_Hasp->getFile(ChaspFile::fileReadWrite);

	file.setFilePos(0);
	haspStatus status = file.read(o_Data, lc_MAX_DONGLE_BYTES);
	if (!HASP_SUCCEEDED(status))
	{
		throw scrtyReadFailedX();
	}
#endif
	return true;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool WriteDongle(unsigned char i_Data[])
{
#ifdef USE_SECURITYDONGLE
	ChaspFile file = l_Hasp->getFile(ChaspFile::fileReadWrite);

	//	Debug output
	for (int j = 0; j < lc_MAX_DONGLE_BYTES; ++j)
	{
		DBG_TRACE( "write->" << j << " (" << i_Data[j] << " " << atoi((const char*)(&i_Data[j])) << ")" );
	}

	//	Write the data to the dongle
	file.setFilePos(0);
	haspStatus status = file.write(i_Data, lc_MAX_DONGLE_BYTES);
	if (!HASP_SUCCEEDED(status))
	{
		throw scrtyWriteFailedX();
	}
#endif
	return true;
}

//------------------------------------------------------------------------
//	check if the dongle is valid
//------------------------------------------------------------------------
bool scrtyaMgr::DongleValid1()
{
#ifdef USE_SECURITYDONGLE
	try
	{
		//	read in the dongle
		unsigned char data[lc_MAX_DONGLE_BYTES];
		memset(&data,0, sizeof(data));
		ReadDongle(data);
	}
	catch (scrtyReadFailedX)
	{
		return false;	// dongle is not in
	}

	//	Check the various types to see if access has expired
	//
//	if (data[3] == lc_USAGETYPE_DAYS)
//	{
//		int today = get_today_julian()
//		int days = today - data[2];
//		if (   (days < 0)
//			|| (today < data[14]) /* last access */
//			|| (days > data[8])
//			)
//			return false;
//	}
//	else if (data[3] == lc_USAGETYPE_COUNT)
//	{
//		if (data[6] > data[8])
//			return false;
//	}
//	else if (data[3] == lc_USAGETYPE_MINUTES)
//	{
//		return false;
//	}
//	else
//	{
//		return false;
//	}
#endif
	return true;
}

//------------------------------------------------------------------------
//	check if the dongle is valid
//------------------------------------------------------------------------
bool scrtyaMgr::DongleValid2()
{
#ifdef USE_SECURITYDONGLE
	hasp_size_t size;
	haspStatus status = l_Hasp->getFile(ChaspFile::fileReadWrite).getFileSize(size);
	if (!HASP_SUCCEEDED(status))
	{
		return false;
	}
	//Chasp hasp1(ChaspFeature::defaultFeature());
	//haspStatus status = hasp1.login(vendorCode);
	//if (!HASP_SUCCEEDED(status))
	//{
	//	return false;
	//}
	//hasp1.logout();
#endif
	return true;
}

//------------------------------------------------------------------------
//	check if the dongle is valid
//------------------------------------------------------------------------
bool scrtyaMgr::DongleValid3()
{
#ifdef USE_SECURITYDONGLE
	const char* scope = 
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	"<haspscope/>";

	const char* format = 
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	"<haspformat root=\"hasp_info\">"
	"    <hasp>"
	"        <attribute name=\"id\" />"
	"        <attribute name=\"type\" />"
	"        <feature>"
	"            <attribute name=\"id\" />"
	"        </feature>"
	"    </hasp>"
	"</haspformat>";

	std::string info;
	haspStatus status = l_Hasp->getInfo(scope, format, vendorCode, info);
	if (!HASP_SUCCEEDED(status))
	{
		return false;
	}
#endif
	return true;
}

//------------------------------------------------------------------------
//	check if the dongle is valid
//------------------------------------------------------------------------
bool scrtyaMgr::DongleValid4()
{
#ifdef USE_SECURITYDONGLE
	hasp_size_t size;
	haspStatus status = l_Hasp->getFile(ChaspFile::fileReadWrite).getFileSize(size);
	if (!HASP_SUCCEEDED(status))
	{
		return false;
	}
#endif
	return true;
}

//------------------------------------------------------------------------
//	check if the dongle is valid
//------------------------------------------------------------------------
bool scrtyaMgr::DongleValid5()
{
#ifdef USE_SECURITYDONGLE
	//if (l_MemData.dongle_version == 0)
	{
		const char* scope = 
		"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
		"<haspscope/>";

		const char* format = 
		"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
		"<haspformat root=\"hasp_info\">"
		"    <hasp>"
		"        <attribute name=\"id\" />"
		"        <attribute name=\"type\" />"
		"        <feature>"
		"            <attribute name=\"id\" />"
		"        </feature>"
		"    </hasp>"
		"</haspformat>";

		std::string info;
		haspStatus status = l_Hasp->getInfo(scope, format, vendorCode, info);
		if (!HASP_SUCCEEDED(status))
		{
			return false;
		}
	}
	//else
	//{
	//	if (l_MemData.app_ID != 1)
	//	{
	//		return false;
	//	}
	//}
#endif
	return true;
}

//------------------------------------------------------------------------
//
//	On open functions for each data field
//
//------------------------------------------------------------------------
void open01(unsigned char i_Data[])		//	dongle version
{
	l_MemData.dongle_version = convert_chars_to_int( &i_Data[0], lc_INDEX_DONGLE_VERSION, lc_SIZE_DONGLE_VERSION );
}
void open02(unsigned char i_Data[])		//	Usage Type
{
	l_MemData.usage_type = convert_chars_to_int( &i_Data[0], lc_INDEX_USAGE_TYPE, lc_SIZE_USAGE_TYPE );
}
void open03(unsigned char i_Data[])		//	access type
{
	l_MemData.access_type = convert_chars_to_int( &i_Data[0], lc_INDEX_ACCESS_TYPE, lc_SIZE_ACCESS_TYPE );
}
void open04( unsigned char i_Data[] )	//	companyID
{
	l_MemData.companyID = convert_chars_to_int( &i_Data[0], lc_INDEX_COMPANY_ID, lc_SIZE_COMPANY_ID );
}
void open05(unsigned char i_Data[])		//	Activation Date
{
	l_MemData.activation_date = convert_chars_to_int( &i_Data[0], lc_INDEX_ACTIVATION_DATE, lc_SIZE_ACTIVATION_DATE );

	//	set the activation date
	if (l_MemData.activation_date == 0)
	{
		l_MemData.activation_date = get_today_julian();
	}
}
void open06(unsigned char i_Data[])		//	features
{
	l_MemData.features = convert_chars_to_int( &i_Data[0], lc_INDEX_FEATURES, lc_SIZE_FEATURES );
}
void open07(unsigned char i_Data[])		//	usage count
{
	l_MemData.usage_count = convert_chars_to_int( &i_Data[0], lc_INDEX_USAGE_COUNT, lc_SIZE_USAGE_COUNT );
	l_MemData.usage_count += 1;
}
void open08( unsigned char i_Data[] )	//	used minutes
{
	l_MemData.used_mins = convert_chars_to_int( &i_Data[0], lc_INDEX_USED_MINS, lc_SIZE_USED_MINS );
	/*need to implement*/
}
void open09( unsigned char i_Data[] )	//	days valid
{
	l_MemData.days_valid = convert_chars_to_int( &i_Data[0], lc_INDEX_DAYS_VALID, lc_SIZE_DAYS_VALID );

	if (l_MemData.usage_type == lc_USAGETYPE_DAYS)
	{
		int today = get_today_julian();
		if ((today - l_MemData.activation_date) > l_MemData.days_valid)
		{
			l_MemData.value = 0; // NEED FORMULA
			if (l_MemData.value == 0)
			{
				l_MemData.status = ((l_MemData.activation_date / 500)+1) * 1345;	// made up formula
			}
		}
	}
}
void open10( unsigned char i_Data[] )	//	uses left
{
	l_MemData.uses_left = convert_chars_to_int( &i_Data[0], lc_INDEX_USES_LEFT, lc_SIZE_USES_LEFT );

	if (l_MemData.usage_type == lc_USAGETYPE_COUNT)
	{
		if (l_MemData.uses_left > 0)
		{
			l_MemData.uses_left -= 1;
			if (l_MemData.uses_left == 0)
			{
				l_MemData.invalid_counter = 3;	// user gets 3 extra "runs"
			}
		}
	}
}
void open11( unsigned char i_Data[] )	//	minutes valid
{
	l_MemData.minutes_valid = convert_chars_to_int( &i_Data[0], lc_INDEX_MINUTES_VALID, lc_SIZE_MINUTES_VALID );
	/*need to implement*/
}
void open12( unsigned char i_Data[] )	//	value
{
	//l_MemData.value = convert_chars_to_int( &i_Data[0], lc_INDEX_VALUE, lc_SIZE_VALUE );
	l_MemData.value = l_MemData.usage_count + l_MemData.usage_count;
}
void open13( unsigned char i_Data[] )	//	invalid counter
{
	l_MemData.invalid_counter = convert_chars_to_int( &i_Data[0], lc_INDEX_INVALID_COUNTER, lc_SIZE_INVALID_COUNTER );
	if (l_MemData.invalid_counter > 0)
	{
		l_MemData.invalid_counter -= 1;
	}
}
void open14( unsigned char i_Data[] )	//	status
{
	l_MemData.status = convert_chars_to_int( &i_Data[0], lc_INDEX_STATUS, lc_SIZE_STATUS );
	//time_t ltime;
	//time( &ltime );
	//srand((unsigned int)ltime);
	//io_Data[14] = (int)rand();
}
void open15( unsigned char i_Data[] )	//	last access date
{
	l_MemData.last_access_date = convert_chars_to_int( &i_Data[0], lc_INDEX_LAST_ACCESS_DATE, lc_SIZE_LAST_ACCESS_DATE );
}
void open16( unsigned char i_Data[] )	//	version - major
{
	l_MemData.ver_major = convert_chars_to_int( &i_Data[0], lc_INDEX_VERSION_MAJOR, lc_SIZE_VERSION_MAJOR );
}
void open17( unsigned char i_Data[] )	//	version - minor
{
	l_MemData.ver_minor = convert_chars_to_int( &i_Data[0], lc_INDEX_VERSION_MINOR, lc_SIZE_VERSION_MINOR );
}
void open18( unsigned char i_Data[] )	//	version - revision
{
	l_MemData.ver_revision = convert_chars_to_int( &i_Data[0], lc_INDEX_VERSION_REVISION, lc_SIZE_VERSION_REVISION );
}
void open19( unsigned char i_Data[] )	//	version - increment
{
	l_MemData.ver_increment = convert_chars_to_int( &i_Data[0], lc_INDEX_VERSION_INCREMENT, lc_SIZE_VERSION_INCREMENT );
}
void open20( unsigned char i_Data[] )	//	application id
{
	l_MemData.app_ID = convert_chars_to_int( &i_Data[0], lc_INDEX_APP_ID, lc_SIZE_APP_ID );
}

//------------------------------------------------------------------------
//	Call ONCE on launch of program
//------------------------------------------------------------------------
void scrtyaMgr::OpenDongle()
{
	//	read in the dongle
	unsigned char data[lc_MAX_DONGLE_BYTES];
	memset(&data,0, sizeof(data));
	ReadDongle(data);

	//	open and update the numeric data fields
	open01(data);
	open02(data);
	open03(data);
	open04(data);
	open05(data);
	open06(data);
	open07(data);
	open08(data);
	open09(data);
	open10(data);
	open11(data);
	open12(data);
	open13(data);
	open14(data);
	open15(data);
	open16(data);
	open17(data);
	open18(data);
	open19(data);
	open20(data);

	//	debug output
#ifdef _DEBUG
	output_memory();
#endif

	//	update the char data
	update_data( data );

	//	write back to the dongle
	WriteDongle( data );
}

//------------------------------------------------------------------------
//
//	On close functions for each data field
//
//------------------------------------------------------------------------
void close01( unsigned char o_Data[] )	// dongle version
{}
void close02( unsigned char o_Data[] )	//	usage type
{}
void close03( unsigned char o_Data[] )	//	access type
{}
void close04( unsigned char o_Data[] )	//	company id
{}
void close05( unsigned char o_Data[] )	//	activation date
{}
void close06( unsigned char o_Data[] )	//	features
{}
void close07( unsigned char o_Data[] )	//	usage count
{/* need to implement*/}
void close08( unsigned char o_Data[] )	//	used mins
{}
void close09( unsigned char o_Data[] )	//	days valid
{}
void close10( unsigned char o_Data[] )	//	uses left
{}
void close11( unsigned char o_Data[] )	//	minutes valid
{}
void close12( unsigned char o_Data[] )	//	value
{}
void close13( unsigned char o_Data[] )	//	invalid counter (counts down)
{}
void close14( unsigned char o_Data[] )	//	status
{}
void close15( unsigned char o_Data[] )	//	last access date
{
	int today = get_today_julian();
	convert_int_to_chars( today, o_Data, lc_INDEX_LAST_ACCESS_DATE, 4 );
}
void close16( unsigned char o_Data[] )	//	version major
{}
void close17( unsigned char o_Data[] )	//	version minor
{}
void close18( unsigned char o_Data[] )	//	version revision
{}
void close19( unsigned char o_Data[] )	//	version increment
{}
void close20( unsigned char o_Data[] )	//	app ID
{}

//------------------------------------------------------------------------
//	Call ONCE on exit of program
//------------------------------------------------------------------------
void scrtyaMgr::CloseDongle()
{
	//	Read the dongle data
	unsigned char data[lc_MAX_DONGLE_BYTES];
	memset(&data,0, sizeof(data));
	ReadDongle(data);

	//	Update the individual entries
	close01(data);
	close02(data);
	close03(data);
	close04(data);
	close05(data);
	close06(data);
	close07(data);
	close08(data);
	close09(data);
	close10(data);
	close11(data);
	close12(data);
	close13(data);
	close14(data);
	close15(data);
	close16(data);
	close17(data);
	close18(data);
	close19(data);
	close20(data);

	//	update the char data
	update_data( data );

	//	write the dongle
	WriteDongle(data);
}

//------------------------------------------------------------------------
//	for testing only
//------------------------------------------------------------------------
void scrtyaMgr::TestDongle()
{
#ifdef USE_SECURITYDONGLE
	const char* scope = 
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	"<haspscope/>";

	const char* format = 
	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	"<haspformat root=\"hasp_info\">"
	"    <hasp>"
	"        <attribute name=\"id\" />"
	"        <attribute name=\"type\" />"
	"        <feature>"
	"            <attribute name=\"id\" />"
	"        </feature>"
	"    </hasp>"
	"</haspformat>";

	std::string info;
	haspStatus status = Chasp::getInfo(scope, format, vendorCode, info);
	if (!HASP_SUCCEEDED(status))
	{
		//handle error
	}
	DBG_TRACE("dongle info = " << info.c_str());

	//	Get Memory Size
	hasp_size_t size;
	status = l_Hasp->getFile(ChaspFile::fileReadWrite).getFileSize(size);
	if (!HASP_SUCCEEDED(status))
	{
		//handle error
	}
	DBG_TRACE("dongle size = " << size);

	//	read in the dongle
	unsigned char data[lc_MAX_DONGLE_BYTES];
	memset(&data,0, sizeof(data));
	ReadDongle(data);

	//	open and update the numeric data fields
	open01(data);
	open02(data);
	open03(data);
	open04(data);
	open05(data);
	open06(data);
	open07(data);
	open08(data);
	open09(data);
	open10(data);
	open11(data);
	open12(data);
	open13(data);
	open14(data);
	open15(data);
	open16(data);
	open17(data);
	open18(data);
	open19(data);

	//	debug output
#ifdef _DEBUG
	output_memory();
#endif

	//	check port
	//	Note: this is not used for USB
	//
	//short dongle_Addr;
	//dongle_Addr = GetPortAdr(l_DonglePort);
	//if (dongle_Addr == 0)
	//{
	//	throw scrtyDongleDoesntExistX();
	//}

	////
	//short dongle_count;
	//dongle_count = Matrix::Dongle_Count(l_DonglePort);
	//if (dongle_count <= 0)
	//{
	//	throw scrtyDongleDoesntExistX();
	//}

	////
	//short dongle_mem;
	//dongle_mem = Matrix::Dongle_MemSize(dongle_count, l_DonglePort);
	//if (dongle_mem <= 0)
	//{
	//	throw scrtyaemSizeErrorX();
	//}

	////
	//long dongle_version;
	//dongle_version = Matrix::Dongle_Version(dongle_count, l_DonglePort);
	//if (dongle_version <= 0)
	//{
	//	throw scrtyVersionIncorrectX();
	//}

	//
#endif
	return;
}

