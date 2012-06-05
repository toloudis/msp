/*****************************************************************************
**	SupportLayer.cpp
**
**		SupportLayer contains the initialization functions
**	for the all packages within the MachStudio Support Layer.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/SupportLayer.hpp"

#include "Support/brsh/brshPaintBrushMgr.hpp"
#include "Support/cmps/cmpsPackage.hpp"
#include "Support/dyn/dynPackage.hpp"
#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/fgmt/fgmtPackage.hpp"
#include "Support/ltst/ltstIsolateMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/mnm/mnmPackage.hpp"
#include "Support/mtrl/mtrlPackage.hpp"
#include "Support/pfx/pfxPackage.hpp"
#include "Support/pnt/pntPackage.hpp"
#include "Support/pyth/pythPackage.hpp"
#include "Support/rlyr/rlyrPackage.hpp"
#include "Support/rmp/rmpPackage.hpp"
#include "Support/rprf/rprfPrefsUtil.hpp"
#include "Support/spln/splnPackage.hpp"
#include "Support/tmln/tmlnPackage.hpp"
#include "Support/vis/visPackage.hpp"
#include "Support/xtra/xtraPackage.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
}


//----------------------------------------------------------------------------
//	Init
//----------------------------------------------------------------------------
void SupportLayer::Init(g2dSystem* i_pSystem)
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages in the layer
		mnmPackage::Init();
		rmpPackage::Init();
		cmpsPackage::Init();
		pntPackage::Init();
		splnPackage::Init();
		tmlnPackage::Init();
		mtrlPackage::Init();
		fgmtPackage::Init(i_pSystem);
		dynPackage::Init();
		visPackage::Init();
		pythPackage::Initialize();
		ltstLightSetMgr::Initialize();
		ltstIsolateMgr::Initialize();
		lyerLayerMgr::Initialize();
		modeModeMgr::Initialize();
		evmtEnvironmentMgr::Initialize();
		rlyrPackage::Init();
		brshPaintBrushMgr::Initialize();
		rprfPrefsUtil::Initialize();
		xtraPackage::Init();
		pfxPackage::Init();
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp
//----------------------------------------------------------------------------
void SupportLayer::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		pfxPackage::CleanUp();
		evmtEnvironmentMgr::DeInitialize();
		modeModeMgr::DeInitialize();
		lyerLayerMgr::DeInitialize();
		ltstLightSetMgr::DeInitialize();
		ltstIsolateMgr::DeInitialize();
		pythPackage::DeInitialize();
		brshPaintBrushMgr::DeInitialize();
		rprfPrefsUtil::DeInitialize();
		xtraPackage::CleanUp();
		visPackage::CleanUp();
		dynPackage::CleanUp();
		fgmtPackage::CleanUp();
		mtrlPackage::CleanUp();
		tmlnPackage::CleanUp();
		splnPackage::CleanUp();
		pntPackage::CleanUp();
		cmpsPackage::CleanUp();
		rlyrPackage::CleanUp();
		rmpPackage::CleanUp();
		mnmPackage::CleanUp();
	}
}
