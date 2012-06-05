// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//
// Smart pointer typedef declarations
//
//#include "ssauto.h"
//
//extern "C" const GUID __declspec(selectany) LIBID_SourceSafeTypeLib =
//    {0x783cd4e0,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) IID_IVSSItem =
//    {0x783cd4e1,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) IID_IVSSVersions =
//    {0x783cd4e7,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) IID_IVSSVersion =
//    {0x783cd4e8,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) IID_IVSSItems =
//    {0x783cd4e5,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) IID_IVSSCheckouts =
//    {0x8903a770,0xf55f,0x11cf,{0x92,0x27,0x00,0xaa,0x00,0xa1,0xeb,0x95}};
//extern "C" const GUID __declspec(selectany) IID_IVSSCheckout =
//    {0x783cd4e6,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) IID_IVSSDatabase =
//    {0x783cd4e2,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) CLSID_VSSItem =
//    {0x783cd4e3,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) CLSID_VSSVersion =
//    {0x783cd4ec,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) CLSID_VSSDatabase =
//    {0x783cd4e4,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) IID_IVSSEvents =
//    {0x783cd4e9,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) IID_IVSS =
//    {0x783cd4eb,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//extern "C" const GUID __declspec(selectany) IID_IVSSEventHandler =
//    {0x783cd4ea,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
//
//
//_COM_SMARTPTR_TYPEDEF(IVSSItem, IID_IVSSItem);
//_COM_SMARTPTR_TYPEDEF(IVSSVersions, IID_IVSSVersions);
//_COM_SMARTPTR_TYPEDEF(IVSSVersion, IID_IVSSVersion);
//_COM_SMARTPTR_TYPEDEF(IVSSItems, IID_IVSSItems);
//_COM_SMARTPTR_TYPEDEF(IVSSCheckouts, IID_IVSSCheckouts);
//_COM_SMARTPTR_TYPEDEF(IVSSCheckout, IID_IVSSCheckout);
//_COM_SMARTPTR_TYPEDEF(IVSSDatabase, IID_IVSSDatabase);
//_COM_SMARTPTR_TYPEDEF(IVSSEvents,IID_IVSSEvents);
//_COM_SMARTPTR_TYPEDEF(IVSS, IID_IVSS);
//_COM_SMARTPTR_TYPEDEF(IVSSEventHandler, IID_IVSSEventHandler);
//
//inline void v_TestHr(HRESULT x) {if FAILED(x) _com_issue_error(x);};
//
//inline void v_SetApplicationRegistryKey(LPCTSTR pstr_company, LPCTSTR pstr_application)
//{
//	HKEY h_Key;
//	DWORD dw_Disposition;
//
//	CString str_RegistryKey;
//	str_RegistryKey.Format(_T("Software\\%s\\%s"),
//							pstr_company,pstr_application);
//
//	if (ERROR_SUCCESS==RegCreateKeyEx(HKEY_CURRENT_USER,
//			str_RegistryKey, 
//			0,
//			NULL,
//			REG_OPTION_NON_VOLATILE,
//			KEY_ALL_ACCESS,
//			NULL,
//			&h_Key,
//			&dw_Disposition))
//	{
//		gstr_baseRegistryKey = str_RegistryKey;
//		RegCloseKey(h_Key);
//	}
//}
//inline BOOL b_GetProfileString(LPCTSTR pstr_Subkey, LPCTSTR pstr_DefaultValue, LPTSTR pstr_Value, 
//							   DWORD dw_BufferSize)
//{
//	ZeroMemory(pstr_Value,dw_BufferSize);
//	if (pstr_DefaultValue!=NULL)
//	{
//		_tcsncpy(pstr_Value, pstr_DefaultValue,dw_BufferSize);
//	}
//
//	DWORD dw_Size = _MAX_PATH;
//	TCHAR str_ReadValue[_MAX_PATH];
//
//	HKEY h_Key;
//	if (ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER,
//		gstr_baseRegistryKey,
//		0,
//		KEY_READ,
//		&h_Key))
//	{
//		if(ERROR_SUCCESS==RegQueryValueEx(h_Key, 
//			pstr_Subkey,
//			NULL,
//			NULL,
//			(LPBYTE)str_ReadValue,
//			&dw_Size))
//		{
//			_tcsncpy(pstr_Value,str_ReadValue, min(dw_BufferSize,dw_Size));
//			return TRUE;
//		}
//	}
//
//	return FALSE;
//}
//
//inline void v_WriteProfileString(LPCTSTR pstr_SubKey, LPCTSTR pstr_Value)
//{
//	HKEY h_Key;
//
//	if (ERROR_SUCCESS==RegCreateKeyEx(HKEY_CURRENT_USER,
//			gstr_baseRegistryKey, 
//			0,
//			NULL,
//			REG_OPTION_NON_VOLATILE,
//			KEY_ALL_ACCESS,
//			NULL,
//			&h_Key,
//			NULL))
//	{
//		RegSetValueEx(h_Key,pstr_SubKey,NULL,REG_SZ,(LPBYTE)pstr_Value,_tcslen(pstr_Value)*sizeof(TCHAR));
//	}
//}
//
//inline BOOL b_DisplayAnyError()
//{
//	IErrorInfoPtr errorInfo;
//	if (GetErrorInfo(0, &errorInfo) == S_OK)
//	{
//		CComBSTR bstr_errorInfo;
//		errorInfo->GetDescription(&bstr_errorInfo);
//		MessageBox(NULL, bstr_errorInfo, _T("ResourceSync"), MB_OK|MB_ICONINFORMATION);
//		return TRUE;
//	}
//
//	return FALSE;
//}
