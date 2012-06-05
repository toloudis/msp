//****************************************************************************
//  inTPCInfo.cpp
//
//      see .hpp
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#include "InputTPC/in/inTPCInfo.hpp"

#include "Core/dbg/DbgMsg.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"

// Windows header files
#include <windows.h>
#include <comdef.h>
#include <wchar.h>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace inTPCInfo
{
	namespace
	{
		bool l_bTPCAvailable = false;

		// The key to the registry settings of the installed speech recognizers
		const WCHAR* gc_wszSpeechKey = L"Software\\Microsoft\\Speech\\Recognizers";

		// CLSID of the Text Services Framework's ThreadManager object
		const CLSID CLSID_TF_ThreadMgr = 
				{ 0x529A9E6B,0x6587,0x4F23,{ 0xAB,0x9E,0x9C,0x7D,0x68,0x3E,0x3C,0x50 } };

		// A helper structure, used in the GetComponentInfo function
		typedef struct 
		{
			WCHAR wchName[256];
			WCHAR wchVersion[256];
		} SInfo; 

		////----------------------------------------------------------------------------                       
		//// GetComponentInfo - gathers the component's name and version 
		////					  info and formats them into output strings
		////----------------------------------------------------------------------------
		//bool GetComponentInfo(CLSID i_clsid, SInfo& i_Info)
		//{    
		//	i_Info.wchName[0] = i_Info.wchVersion[0] = 0;

		//	// Format Registry Key string
		//	WCHAR wszKey[45] = L"CLSID\\";  // the key buffer should be large enough for a string 
		//									// like "CLSID\{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}"
		//	// Convert CLSID to String
		//	UINT uPos = lstrlenW(wszKey);
		//	if (0 == StringFromGUID2(i_clsid, &wszKey[uPos], countof(wszKey) - uPos))
		//		return false;
		//	wszKey[countof(wszKey)-1] = 0;

		//	// Open key to find path of application
		//	HKEY hKeyRoot;
		//	if (RegOpenKeyExW(HKEY_CLASSES_ROOT, wszKey, 0, KEY_READ, &hKeyRoot) != ERROR_SUCCESS) 
		//		return false;

		//	// Query value of key to get the name of the component
		//	ULONG cSize = sizeof(i_Info.wchName);  // size of the buffer in bytes
		//	if (RegQueryValueExW(hKeyRoot, NULL, NULL, NULL, (BYTE*)i_Info.wchName, &cSize) != ERROR_SUCCESS)
		//	{
		//		RegCloseKey(hKeyRoot);
		//		return false;
		//	}
		//	i_Info.wchName[countof(i_Info.wchName) - 1] = 0;

		//	// Open the version i_Info subkey 
		//	UINT iVersionMaxLen = countof(i_Info.wchVersion);
		//	HKEY hKey = NULL;
		//	if (RegOpenKeyExW(hKeyRoot, L"Version", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
		//	{
		//		const WCHAR* pcwsVersion = L"version ";
		//		UINT iLen = lstrlenW(pcwsVersion);
		//		// Query value of key to get version string
		//		if (iLen < iVersionMaxLen)
		//		{
		//			// copy the "version " string including terminating 0
		//			lstrcpynW(i_Info.wchVersion, pcwsVersion, iLen + 1);
		//			// get the version string
		//			cSize = (iVersionMaxLen - iLen) * sizeof(WCHAR); // the size is in bytes
		//			if (RegQueryValueExW(hKey, NULL, NULL, NULL, 
		//								 (BYTE*)&i_Info.wchVersion[iLen], 
		//								 &cSize) == ERROR_SUCCESS)
		//			{
		//				i_Info.wchVersion[iVersionMaxLen-1] = 0;
		//			}
		//		}
		//		RegCloseKey(hKey);
		//	}

		//	// Open InprocServer32 subkey to get the path to the component
		//	if (RegOpenKeyExW(hKeyRoot, L"InprocServer32", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
		//	{
		//		// Query value of key to get the path string
		//		WCHAR wchPath[MAX_PATH];
		//		cSize = sizeof(wchPath);    
		//		if (RegQueryValueExW(hKey, NULL, NULL, NULL, (BYTE*)wchPath, &cSize) == ERROR_SUCCESS)
		//		{
		//			// Get the build number from the file version i_Info
		//			DWORD dwHandle = 0;
		//			cSize = GetFileVersionInfoSizeW(wchPath, &dwHandle); // returns the size in bytes
		//			WCHAR* pwchFileVerInfo = NULL;
		//			if (cSize)
		//			{
		//				pwchFileVerInfo = (WCHAR*)new BYTE[cSize];
		//			}
		//			if (NULL != pwchFileVerInfo)
		//			{
		//				// Retrieve version information for the file
		//				if (GetFileVersionInfoW(wchPath, 0, cSize, pwchFileVerInfo))
		//				{
		//					// Get the default language id and code page number
		//					UINT *pdwLang;
		//					UINT cch = 0;
		//					if (VerQueryValueW(pwchFileVerInfo, L"\\VarFileInfo\\Translation",
		//									   (void**)&pdwLang, &cch) == TRUE)
		//					{
		//						// Read the file description for the language and code page.
		//						const int MAX_SUBBLOCK = 40;
		//						WCHAR wchSubBlock[MAX_SUBBLOCK];  // large enough for the string
		//						_snwprintf(wchSubBlock, MAX_SUBBLOCK - 1,
		//								   L"\\StringFileInfo\\%04x%04x\\FileVersion",
		//								   LOWORD(*pdwLang), HIWORD(*pdwLang));
		//						wchSubBlock[MAX_SUBBLOCK-1] = 0;

		//						WCHAR* pwchBuildVer = NULL;
		//						if ((VerQueryValueW(pwchFileVerInfo, wchSubBlock, 
		//											(void**)&pwchBuildVer, &cch) == TRUE)
		//							&& (NULL != pwchBuildVer))
		//						{
		//							// Format the version string
		//							UINT iLen = (UINT)lstrlenW(i_Info.wchVersion);
		//							if (0 < iLen)
		//							{
		//								if (iLen < iVersionMaxLen)
		//								{
		//									const WCHAR* pcwsBuild = L", build ";
		//									lstrcpynW(i_Info.wchVersion + iLen, pcwsBuild, 
		//											  iVersionMaxLen - iLen);
		//									iLen += lstrlenW(pcwsBuild);
		//									if (iLen < iVersionMaxLen)
		//									{
		//										lstrcpynW(i_Info.wchVersion + iLen, pwchBuildVer, 
		//												  iVersionMaxLen - iLen);
		//									}
		//								}
		//							}
		//							else
		//							{
		//								lstrcpynW(i_Info.wchVersion, pwchBuildVer, iVersionMaxLen);
		//							}
		//							i_Info.wchVersion[iVersionMaxLen-1] = 0;
		//						}
		//					}
		//				}
		//				delete [] pwchFileVerInfo;
		//			}

		//		}
		//		RegCloseKey(hKey);
		//	}
		//    
		//	RegCloseKey(hKeyRoot);

		//	return true;
		//}
	} //end anon namespace


	//------------------------------------------------------------------------
	// Logs the initial properties of the user's PC in regards to Tablet
	//	capabilities
	//------------------------------------------------------------------------
	void LogTabletInfo()
	{
		bool bReturn = false; // the value to return 
		// Gather and show the information we're interested in

		// Check out if Microsoft Tablet PC Platform components of the 
		// Microsoft Windows XP Professional Operating System are enabled
		int tablet_ready = GetSystemMetrics(SM_TABLETPC);
		if(tablet_ready)
		{
			DBG_LOG("Tablet PC: Availavable");
			l_bTPCAvailable = true;
		}
		else
		{
			DBG_LOG("Tablet PC: Not Availavable");
			l_bTPCAvailable = false;
		}

		// Get the version of the Text Services Framework components
		//SInfo info;
		//if (GetComponentInfo(CLSID_TF_ThreadMgr, info))
		//{
		//	//set Text Service Version
		//	DBG_LOG("Text Service Framework " << info.wchVersion);
		//}

		// Find out the name and the version of the default handwriting recognizer
		// Create the enumerator for the installed recognizers
		IInkRecognizers* pIInkRecognizers = NULL;
		HRESULT hr = CoCreateInstance(CLSID_InkRecognizers,
									  NULL, 
									  CLSCTX_INPROC_SERVER, 
									  IID_IInkRecognizers, 
									  (void **)&pIInkRecognizers);
		if (SUCCEEDED(hr)) 
		{
			IInkRecognizer* pIInkRecognizer = NULL;
			// The first parameter is the language id, passing 0 means that the language 
			// id will be retrieved using the user default-locale identifier
			hr = pIInkRecognizers->GetDefaultRecognizer(0, &pIInkRecognizer);
			if (SUCCEEDED(hr))
			{
				// Get the recognizer's friendly name
				BSTR bstr;
				if (SUCCEEDED(pIInkRecognizer->get_Name(&bstr)))
				{
					DBG_LOG("Default Handwriting Recognizer: " << bstr);
					SysFreeString(bstr);
				}
				else
				{
					DBG_LOG("Default Handwriting Recognizer: N/A");
				}
				// Get the recognizer's vendor info
				if (SUCCEEDED(pIInkRecognizer->get_Vendor(&bstr)))
				{
					DBG_LOG("Handwriting Recognizer Vendor: " << bstr);
					SysFreeString(bstr);
				}
				else
				{
					 DBG_LOG("Handwriting Recognizer Vendor: N/A");
				}
				// Release it
				pIInkRecognizer->Release();
				pIInkRecognizer = NULL;
			}
			// Release the collection object
			pIInkRecognizers->Release();
			pIInkRecognizers = NULL;
		}

		// Find out the name and the version of the default speech recognizer

		// Open key to find path of application
		HKEY hkeySpeech;
		if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, gc_wszSpeechKey, 0, KEY_READ, 
						&hkeySpeech) == ERROR_SUCCESS) 
		{
			// Query value of key to get the name of the component
			WCHAR wchValue[265];
			ULONG cSize = sizeof(wchValue);
			if (RegQueryValueExW(hkeySpeech, L"DefaultDefaultTokenId", NULL, NULL, 
								(BYTE*)wchValue, &cSize) == ERROR_SUCCESS)
			{
				int ndx = lstrlenW(L"HKEY_LOCAL_MACHINE\\");
				int len = lstrlenW(wchValue);
				if (ndx < len 
					&& RegOpenKeyExW(HKEY_LOCAL_MACHINE, &wchValue[ndx], 0, 
									 KEY_READ, &hkeySpeech) == ERROR_SUCCESS)
				{
					cSize = sizeof(wchValue);
					if (RegQueryValueExW(hkeySpeech, NULL, NULL, NULL, 
										 (BYTE*)wchValue, &cSize) == ERROR_SUCCESS)
					{
						DBG_LOG("Default Speech Recognizer: " << wchValue);
					}
				}
			}
		}

		// Done with the COM
		CoUninitialize();
	}

	//------------------------------------------------------------------------
	// Return whether or not the current system is capable of running the 
	//	TabletPC SDK
	//------------------------------------------------------------------------
	bool IsTabletSystem()
	{
		/// @todo If Tablet PC support is not available on a Windows machine, installing most recent Wacom drivers will fix it.
		int tablet_ready = GetSystemMetrics(SM_TABLETPC);
		if(tablet_ready)
		{
			DBG_LOG("Tablet PC: Availavable");
			l_bTPCAvailable = true;
		}
		else
		{
			DBG_WARNING("Tablet PC: Not Availavable, Paint Operations will not be accesible on this system");
			prtyTextureFileChooserUIInfo::DisableType( prtyTextureFileChooserUIInfo::e_Paint );
			l_bTPCAvailable = false;
		}
		return l_bTPCAvailable;
	}
} //end namespace inTPCInfoPAC