/*****************************************************************************
**	scrtymMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "SecurityMatrix/scrty/scrtymMgr.hpp"

//#define USE_SECURITYDONGLE

//	add either the 32-bit or 64-bit version of the library
//	plus the correct header file
//
#ifdef USE_SECURITYDONGLE
#ifdef WIN32
#include "Matrix/API/lib/h/matrix32.h"
#endif
#ifdef WIN64
#include "Matrix/API/lib/h/matrix64.h"
#endif
#endif

#include "Core/scrty/scrtyDongleX.hpp"
#include "Core/app/appTimeUtils.hpp"
#include "Core/dbg/dbgMsg.hpp"
#ifdef USE_SECURITYDONGLE
#include "SecurityMatrix/scrty/Mxapi.hpp"
#endif

#include <time.h>


//==============================================================================
//	library pragmas
//==============================================================================
#ifdef USE_SECURITYDONGLE
#pragma comment(lib,"mxst32_vc80.lib")
#endif


//============================================================================
//============================================================================
#ifdef USE_SECURITYDONGLE
using namespace MXAPI;
#endif


//============================================================================
//============================================================================
namespace //scrtymMgr
{
	static const long  l_UserCode = 48095;
	static const short l_MaxDataFields = 15;
	static const short l_NumDataFields = 15;
	static const short l_DongleNumber = 1;
	static const short l_DonglePort = 'U';	// char 85

	const int lc_ACCESSTYPE_FULL = 0;
	const int lc_ACCESSTYPE_LIMITED = 1;
	const int lc_USAGETYPE_DAYS = 1;
	const int lc_USAGETYPE_COUNT = 2;
	const int lc_USAGETYPE_MINUTES = 3;
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void scrtymMgr::Init()
{
#ifdef USE_SECURITYDONGLE
	//	initialize the security API
	//
	short ret;
	try
	{
		ret = Matrix::Init_MatrixAPI();
	}
	catch (...)
	{
		//	if this happens it means the matrix DLL wasn't found.
		ret = -1;
	}

	if (ret < 0)
	{
		throw scrtyAPIFailedX();
	}

	//	api version
	//
	long API_version;
	API_version = Matrix::GetVersionAPI();
	if (API_version == 0)
		throw scrtyAPIFailedX();

	//	read the actual dongle
	OpenDongle();

	//test
	//int day1 = appTimeUtils::DateToJulian(2007,9,9);
	//int day2 = appTimeUtils::DateToJulian(2007,9,12);
	//int day3 = appTimeUtils::DateToJulian(2007,10,12);
	//int day4 = appTimeUtils::DateToJulian(2009,10,12);
	//DBG_LOG("JULIAN DATE SUBTRACTION");
	//DBG_LOG3("%d - %d = %d", day2, day1, (day2-day1));
	//DBG_LOG3("%d - %d = %d", day3, day2, (day3-day2));
	//DBG_LOG3("%d - %d = %d", day4, day3, (day4-day3));
	//DBG_LOG3("%d - %d = %d", day4, day2, (day4-day2));
#endif
	return;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void scrtymMgr::CleanUp()
{
#ifdef USE_SECURITYDONGLE
	//
	CloseDongle();

	//
	short ret = Matrix::Release_MatrixAPI();
#endif
	return;
}

//------------------------------------------------------------------------
//	for testing only
//------------------------------------------------------------------------
void scrtymMgr::TestDongle()
{
#ifdef USE_SECURITYDONGLE
	//	check port
	//	Note: this is not used for USB
	//
	//short dongle_Addr;
	//dongle_Addr = GetPortAdr(l_DonglePort);
	//if (dongle_Addr == 0)
	//{
	//	throw scrtyDongleDoesntExistX();
	//}

	//
	short dongle_count;
	dongle_count = Matrix::Dongle_Count(l_DonglePort);
	if (dongle_count <= 0)
	{
		throw scrtyDongleDoesntExistX();
	}

	//
	short dongle_mem;
	dongle_mem = Matrix::Dongle_MemSize(dongle_count, l_DonglePort);
	if (dongle_mem <= 0)
	{
		throw scrtyMemSizeErrorX();
	}

	//
	long dongle_version;
	dongle_version = Matrix::Dongle_Version(dongle_count, l_DonglePort);
	if (dongle_version <= 0)
	{
		throw scrtyVersionIncorrectX();
	}

	//
#endif
	return;
}

//------------------------------------------------------------------------
//	check if the dongle is valid
//------------------------------------------------------------------------
bool scrtymMgr::DongleValid()
{
#ifdef USE_SECURITYDONGLE
	int data[l_MaxDataFields];
	
	//	read in the data
	short ret = Matrix::Dongle_ReadData(l_UserCode, data[0], l_NumDataFields, l_DongleNumber, l_DonglePort);
	if (ret < 0)
	{
		return false;
		//throw scrtyReadFailedX();
	}

	if (data[3] == lc_USAGETYPE_DAYS)
	{
		time_t t = time(0);
		tm* lt = localtime(&t);
		int today = appTimeUtils::DateToJulian( lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday );
		int days = today - data[2];
		if (   (days < 0)
			|| (today < data[14]) /* last access */
			|| (days > data[8])
			)
			return false;
	}
	else if (data[3] == lc_USAGETYPE_COUNT)
	{
		if (data[6] > data[8])
			return false;
	}
	else if (data[3] == lc_USAGETYPE_MINUTES)
	{
		return false;
	}
	else
	{
		return false;
	}
#endif
	return true;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool ReadDongle(int o_Data[])
{
#ifdef USE_SECURITYDONGLE
	//	read in the data
	short ret = Matrix::Dongle_ReadData(l_UserCode, o_Data[0], l_NumDataFields, l_DongleNumber, l_DonglePort);
	if (ret < 0)
	{
		throw scrtyReadFailedX();
	}

	//	check the fields
	//DBG_LOG3("Read Dongle data: %d %d %d", o_Data[0], o_Data[1], o_Data[2]);
#endif
	return true;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool WriteDongle(int i_Data[])
{
#ifdef USE_SECURITYDONGLE
	//
	for (int i = 0; i < l_MaxDataFields; ++i)
	{
		DBG_LOG("writing dongle data: " << i << " " << i_Data[i]);
	}

	short ret = Matrix::Dongle_WriteData(l_UserCode, i_Data[0], l_NumDataFields, l_DongleNumber, l_DonglePort);

	//	update the data
	//
	//update03(data);
#endif
	return true;
}

//------------------------------------------------------------------------
//
//	On open functions for each data field
//
//------------------------------------------------------------------------
void open01( int io_Data[] )	// reserved
{}
void open02( int io_Data[] )	//	reserved
{}
void open03( int io_Data[] )	//	activate date
{
	//	set the activation date
	if (io_Data[2] == 0)
	{
		time_t t = time(0);
		tm* lt = localtime(&t);
		io_Data[2] = appTimeUtils::DateToJulian( lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday );
	}
}
void open04( int io_Data[] )	//	usage type
{}
void open05( int io_Data[] )	//	access type
{}
void open06( int io_Data[] )	//	run count
{io_Data[5] = io_Data[5] + 1;}
void open07( int io_Data[] )	//	minutes used
{/*need to implement*/}
void open08( int io_Data[] )	//	days valid
{
	if (io_Data[3] == lc_USAGETYPE_DAYS)
	{
		//	compare 2 julian dates to find days spent
		if (io_Data[7] > 0)
		{
			io_Data[7] = 0; // NEED FORMULA
			if (io_Data[7] == 0)
			{
				io_Data[13] = ((io_Data[2] / 500)+1) * 1345;
			}
		}
	}
}
void open09( int io_Data[] )	//	uses left (count)
{
	if (io_Data[3] == lc_USAGETYPE_COUNT)
	{
		if (io_Data[8] > 0)
		{
			io_Data[8] = io_Data[8] - 1;
			if (io_Data[8] == 0)
			{
				io_Data[12] = 3;	// user gets n extra "runs"
			}
		}
	}
}
void open10( int io_Data[] )	//	minutes valid
{}
void open11( int io_Data[] )	//	value
{io_Data[10] = io_Data[6] * io_Data[6]; }
void open12( int io_Data[] )	//	value 2
{io_Data[11] = io_Data[10] * io_Data[5];}
void open13( int io_Data[] )	//	invalid counter (counts down)
{
	if (io_Data[12] > 0)
	{
		io_Data[12] = io_Data[12] - 1;
	}
}
void open14( int io_Data[] )	//	extra valid/invalid flag
{	
}
void open15( int io_Data[] )
{
	time_t ltime;
	time( &ltime );
	srand((unsigned int)ltime);
	io_Data[14] = (int)rand();
}

//------------------------------------------------------------------------
//	Call ONCE on launch of program
//------------------------------------------------------------------------
void scrtymMgr::OpenDongle()
{
	int data[l_MaxDataFields];
	ReadDongle(data);

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

	//
	//DBG_LOG("Opening Dongle---------------------------");
	//for (int i = 0; i < l_MaxDataFields; ++i)
	//{
	//	DBG_LOG2("Dongle data: %02d %d", i, data[i]);
	//}

	//	write the dongle
	WriteDongle(data);
}

//------------------------------------------------------------------------
//
//	On close functions for each data field
//
//------------------------------------------------------------------------
void close01( int io_Data[] )	// reserved
{}
void close02( int io_Data[] )	//	reserved
{}
void close03( int io_Data[] )	//	activate date
{}
void close04( int io_Data[] )	//	usage type
{}
void close05( int io_Data[] )	//	access type
{}
void close06( int io_Data[] )	//	run count
{}
void close07( int io_Data[] )	//	minutes used
{/* need to implement*/}
void close08( int io_Data[] )	//	days valid
{}
void close09( int io_Data[] )	//	uses left (count)
{}
void close10( int io_Data[] )	//	minutes valid
{}
void close11( int io_Data[] )	//	value
{}
void close12( int io_Data[] )	//	value 2
{}
void close13( int io_Data[] )	//	invalid counter (counts down)
{}
void close14( int io_Data[] )	//	extra valid/invalid flag
{}
void close15( int io_Data[] )
{
	time_t t = time(0);
	tm* lt = localtime(&t);
	int today = appTimeUtils::DateToJulian( lt->tm_year+1900, lt->tm_mon+1, lt->tm_mday );
	io_Data[14] = today;
}
//------------------------------------------------------------------------
//	Call ONCE on exit of program
//------------------------------------------------------------------------
void scrtymMgr::CloseDongle()
{
	int data[l_MaxDataFields];
	ReadDongle(data);

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

	//
	//DBG_LOG("Closing Dongle---------------------------");
	//for (int i = 0; i < l_MaxDataFields; ++i)
	//{
	//	DBG_LOG2("Dongle data: %02d %d", i, data[i]);
	//}

	//	write the dongle
	WriteDongle(data);
}

