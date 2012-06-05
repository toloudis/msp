/*****************************************************************************
**  FBXExport.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/GraphicsLayer.hpp"

#ifdef _DEBUG
#pragma comment(lib,"fbxsdk_md2008d.lib")
#else
#pragma comment(lib,"fbxsdk_md2008.lib")
#endif
#pragma comment(lib,"wininet.lib")

#define KFBX_PLUGIN
#define KFBX_FBXSDK
#define KFBX_NODLL

#include <fbxsdk.h>

int main(int argc, char** argv)
{
	KFbxSdkManager* l_pSdkManager = KFbxSdkManager::Create();

	//GraphicsLayer::Init();

	//GraphicsLayer::CleanUp();

	return 0;
}
