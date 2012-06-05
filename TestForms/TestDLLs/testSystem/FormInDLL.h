// FormInDLL.h

#pragma once

#include "plgPlugInDLL.hPP"
#include <windows.h>
#include <iostream>
using namespace std;

//typedef VOID (*MYPROC)(DWORD,UINT); 
typedef UINT (*LPFNDLLFUNC1)(DWORD,UINT);

using namespace System;

namespace FormInDLL
{
	public __gc class Class1
	{
	};
}

#pragma unmanaged
extern "C" __declspec(dllexport) UINT DLLFunc2( DWORD one, UINT two )
{
	int x = 10;
	x++;
	return x;
};
