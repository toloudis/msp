/*****************************************************************************/
/*  MXAPI.CS   MS-WINDOWS Win32 (95/98/ME/NT/2K/XP)                          */
/*                                                                           */
/*  (C) TDi GmbH                                                             */
/*                                                                           */
/*  Defines to acces the Matrix API in C#                                    */
/*****************************************************************************/

#include <string>


#ifdef _MANAGED
//============================================================================
//============================================================================
using namespace System;
using namespace System::Runtime::InteropServices;   /* Required namespace for the DllImport method */
#else
//============================================================================
// Windows API functions and constants
//============================================================================
#define DLLIMPORT __declspec(dllimport) 
#endif	// _MANAGED


//============================================================================
//============================================================================
namespace MXAPI
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	struct DNGINFO
	{
		public: short LPT_Nr;
		public: short LPT_Adr;
		public: short DNG_Cnt;
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
#if defined(WIN32)
	const char * lc_MatrixDLL = "Matrix32.dll";
#else
	const char * lc_MatrixDLL = "Matrix64.dll";
#endif

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	class Matrix
	{
		public:
#ifndef _MANAGED
		//============================================================================
		//	Non-managed version
		//============================================================================
#if defined(WIN32)
			// This c#-class will import the Matrix API classes
			//
			DLLIMPORT static short Init_MatrixAPI();
			DLLIMPORT static short Release_MatrixAPI();
			DLLIMPORT static int GetVersionAPI();
			DLLIMPORT static int GetVersionDRV();
			DLLIMPORT static int GetVersionDRV_USB();
			DLLIMPORT static void SetW95Access(short Mode);
			DLLIMPORT static short GetPortAdr(short Port);
			DLLIMPORT static short PausePrinterActivity();
			DLLIMPORT static short ResumePrinterActivity();
			DLLIMPORT static short Dongle_Find();
			DLLIMPORT static short Dongle_FindEx(DNGINFO* DngInfo);
			DLLIMPORT static int Dongle_Version(short DngNr, short Port);
			DLLIMPORT static int Dongle_Model(short DngNr, short Port);
			DLLIMPORT static short Dongle_MemSize(short DngNr, short Port);
			DLLIMPORT static short Dongle_Count(short Port);
			DLLIMPORT static short Dongle_ReadData(int UserCode, int& Data, short Count, short DngNr, short Port);
			DLLIMPORT static short Dongle_ReadDataEx(int UserCode, int& Data, short Fpos, short Count, short DngNr, short Port);
			DLLIMPORT static int Dongle_ReadSerNr(int UserCode, short DngNr, short Port);
			DLLIMPORT static short Dongle_WriteData(int UserCode, int& Data, short Count, short DngNr, short Port);
			DLLIMPORT static short Dongle_WriteDataEx(int UserCode, int& Data, short Fpos, short Count, short DngNr, short Port);
			DLLIMPORT static short Dongle_WriteKey(int UserCode, int& KeyData, short DngNr, short Port);
			DLLIMPORT static short Dongle_GetKeyFlag(int UserCode, short DngNr, short Port);
			DLLIMPORT static short Dongle_Exit();
			DLLIMPORT static short SetConfig_MatrixNet(short nAccess, char* nFile);
			DLLIMPORT static int GetConfig_MatrixNet(short Category);
			DLLIMPORT static short LogIn_MatrixNet(int UserCode, short AppSlot, short DngNr);
			DLLIMPORT static short LogOut_MatrixNet(int UserCode, short AppSlot, short DngNr);
			DLLIMPORT static short Dongle_EncryptData(int UserCode, int& DataBlock, short DngNr, short Port);
			DLLIMPORT static short Dongle_DecryptData(int UserCode, int& DataBlock, short DngNr, short Port);
#else
			// This c#-class will import the Matrix API classes
			//		
			DLLIMPORT static short Init_MatrixAPI();
			DLLIMPORT static short Release_MatrixAPI();
			DLLIMPORT static int GetVersionAPI();
			DLLIMPORT static int GetVersionDRV();
			DLLIMPORT static int GetVersionDRV_USB();
			DLLIMPORT static void SetW95Access(short Mode);
			DLLIMPORT static short GetPortAdr(short Port);
			DLLIMPORT static short PausePrinterActivity();
			DLLIMPORT static short ResumePrinterActivity();
			DLLIMPORT static short Dongle_Find();
			DLLIMPORT static short Dongle_FindEx(DNGINFO* DngInfo);
			DLLIMPORT static int Dongle_Version(short DngNr, short Port);
			DLLIMPORT static int Dongle_Model(short DngNr, short Port);
			DLLIMPORT static short Dongle_MemSize(short DngNr, short Port);
			DLLIMPORT static short Dongle_Count(short Port);
			DLLIMPORT static short Dongle_ReadData(int UserCode, int& Data, short Count, short DngNr, short Port);
			DLLIMPORT static short Dongle_ReadDataEx(int UserCode, int& Data, short Fpos, short Count, short DngNr, short Port);
			DLLIMPORT static int Dongle_ReadSerNr(int UserCode, short DngNr, short Port);
			DLLIMPORT static short Dongle_WriteData(int UserCode, int& Data, short Count, short DngNr, short Port);
			DLLIMPORT static short Dongle_WriteDataEx(int UserCode, int& Data, short Fpos, short Count, short DngNr, short Port);
			DLLIMPORT static short Dongle_WriteKey(int UserCode, int& KeyData, short DngNr, short Port);
			DLLIMPORT static short Dongle_GetKeyFlag(int UserCode, short DngNr, short Port);
			DLLIMPORT static short Dongle_Exit();
			DLLIMPORT static short SetConfig_MatrixNet(short nAccess, char* nFile);
			DLLIMPORT static int GetConfig_MatrixNet(short Category);
			DLLIMPORT static short LogIn_MatrixNet(int UserCode, short AppSlot, short DngNr);
			DLLIMPORT static short LogOut_MatrixNet(int UserCode, short AppSlot, short DngNr);
			DLLIMPORT static short Dongle_EncryptData(int UserCode, int& DataBlock, short DngNr, short Port);
			DLLIMPORT static short Dongle_DecryptData(int UserCode, int& DataBlock, short DngNr, short Port);
#endif
#else // _MANAGED
#if defined(WIN32)
		// This c#-class will import the Matrix API classes
		//		
		[DllImport("Matrix32.dll")]
		static short Init_MatrixAPI();

		[DllImport("Matrix32.dll")]
		static short Release_MatrixAPI();

		[DllImport("Matrix32.dll")]
		static int GetVersionAPI();

		[DllImport("Matrix32.dll")]
		static int GetVersionDRV();

		[DllImport("Matrix32.dll")]
		static int GetVersionDRV_USB();

		[DllImport("Matrix32.dll")]
		static void SetW95Access(short Mode);

		[DllImport("Matrix32.dll")]
		static short GetPortAdr(short Port);

		[DllImport("Matrix32.dll")]
		static short PausePrinterActivity();

		[DllImport("Matrix32.dll")]
		static short ResumePrinterActivity();

		[DllImport("Matrix32.dll")]
		static short Dongle_Find();

		[DllImport("Matrix32.dll")]
		static short Dongle_FindEx(DNGINFO* DngInfo);

		[DllImport("Matrix32.dll")]
		static int Dongle_Version(short DngNr, short Port);

		[DllImport("Matrix32.dll")]
		static int Dongle_Model(short DngNr, short Port);

		[DllImport("Matrix32.dll")]
		static short Dongle_MemSize(short DngNr, short Port);

		[DllImport("Matrix32.dll")]
		static short Dongle_Count(short Port);

		[DllImport("Matrix32.dll")]
		static short Dongle_ReadData(int UserCode, int& Data, short Count, short DngNr, short Port);

		[DllImport("Matrix32.dll")]
		static short Dongle_ReadDataEx(int UserCode, int& Data, short Fpos, short Count, short DngNr, short Port);

		[DllImport("Matrix32.dll")]
		static int Dongle_ReadSerNr(int UserCode, short DngNr, short Port);

		[DllImport("Matrix32.dll")]
		static short Dongle_WriteData(int UserCode, int& Data, short Count, short DngNr, short Port);

		[DllImport("Matrix32.dll")]
		static short Dongle_WriteDataEx(int UserCode, int& Data, short Fpos, short Count, short DngNr, short Port);

		[DllImport("Matrix32.dll")]
		static short Dongle_WriteKey(int UserCode, int& KeyData, short DngNr, short Port);

		[DllImport("Matrix32.dll")]
		static short Dongle_GetKeyFlag(int UserCode, short DngNr, short Port);

		[DllImport("Matrix32.dll")]
		static short Dongle_Exit();

		[DllImport("Matrix32.dll")]
		static short SetConfig_MatrixNet(short nAccess, char* nFile);

		[DllImport("Matrix32.dll")]
		static int GetConfig_MatrixNet(short Category);

		[DllImport("Matrix32.dll")]
		static short LogIn_MatrixNet(int UserCode, short AppSlot, short DngNr);

		[DllImport("Matrix32.dll")]
		static short LogOut_MatrixNet(int UserCode, short AppSlot, short DngNr);

		[DllImport("Matrix32.dll")]
		static short Dongle_EncryptData(int UserCode, int& DataBlock, short DngNr, short Port);

		[DllImport("Matrix32.dll")]
		static short Dongle_DecryptData(int UserCode, int& DataBlock, short DngNr, short Port);
#else
		// This c#-class will import the Matrix API classes
		//		
		[DllImport("Matrix64.dll")]
		static short Init_MatrixAPI();

		[DllImport("Matrix64.dll")]
		static short Release_MatrixAPI();

		[DllImport("Matrix64.dll")]
		static int GetVersionAPI();

		[DllImport("Matrix64.dll")]
		static int GetVersionDRV();

		[DllImport("Matrix64.dll")]
		static int GetVersionDRV_USB();

		[DllImport("Matrix64.dll")]
		static void SetW95Access(short Mode);

		[DllImport("Matrix64.dll")]
		static short GetPortAdr(short Port);

		[DllImport("Matrix64.dll")]
		static short PausePrinterActivity();

		[DllImport("Matrix64.dll")]
		static short ResumePrinterActivity();

		[DllImport("Matrix64.dll")]
		static short Dongle_Find();

		[DllImport("Matrix64.dll")]
		static short Dongle_FindEx(DNGINFO* DngInfo);

		[DllImport("Matrix64.dll")]
		static int Dongle_Version(short DngNr, short Port);

		[DllImport("Matrix64.dll")]
		static int Dongle_Model(short DngNr, short Port);

		[DllImport("Matrix64.dll")]
		static short Dongle_MemSize(short DngNr, short Port);

		[DllImport("Matrix64.dll")]
		static short Dongle_Count(short Port);

		[DllImport("Matrix64.dll")]
		static short Dongle_ReadData(int UserCode, int& Data, short Count, short DngNr, short Port);

		[DllImport("Matrix64.dll")]
		static short Dongle_ReadDataEx(int UserCode, int& Data, short Fpos, short Count, short DngNr, short Port);

		[DllImport("Matrix64.dll")]
		static int Dongle_ReadSerNr(int UserCode, short DngNr, short Port);

		[DllImport("Matrix64.dll")]
		static short Dongle_WriteData(int UserCode, int& Data, short Count, short DngNr, short Port);

		[DllImport("Matrix64.dll")]
		static short Dongle_WriteDataEx(int UserCode, int& Data, short Fpos, short Count, short DngNr, short Port);

		[DllImport("Matrix64.dll")]
		static short Dongle_WriteKey(int UserCode, int& KeyData, short DngNr, short Port);

		[DllImport("Matrix64.dll")]
		static short Dongle_GetKeyFlag(int UserCode, short DngNr, short Port);

		[DllImport("Matrix64.dll")]
		static short Dongle_Exit();

		[DllImport("Matrix64.dll")]
		static short SetConfig_MatrixNet(short nAccess, char* nFile);

		[DllImport("Matrix64.dll")]
		static int GetConfig_MatrixNet(short Category);

		[DllImport("Matrix64.dll")]
		static short LogIn_MatrixNet(int UserCode, short AppSlot, short DngNr);

		[DllImport("Matrix64.dll")]
		static short LogOut_MatrixNet(int UserCode, short AppSlot, short DngNr);

		[DllImport("Matrix64.dll")]
		static short Dongle_EncryptData(int UserCode, int& DataBlock, short DngNr, short Port);

		[DllImport("Matrix64.dll")]
		static short Dongle_DecryptData(int UserCode, int& DataBlock, short DngNr, short Port);
#endif
#endif	// _MANAGED
	};
}

