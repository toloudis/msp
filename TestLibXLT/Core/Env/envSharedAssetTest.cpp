/*****************************************************************************
**  envSharedAssetTest.cpp
**
**      envSharedAssetTest is a test of envSharedAssetMgr.hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "envSharedAssetTest.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSharedAssetMgr.hpp"


//====================================================================
// Anonymous Namespace for local variables and functions
//====================================================================
namespace 
{
	class NamedAsset
	{
	public:
		NamedAsset(const std::string& i_Name)
			: m_Name(i_Name)
		{
			DBG_LOG1("Creating asset: %s", m_Name.c_str());
		}
		~NamedAsset()
		{
			DBG_LOG1("Deleting asset: %s", m_Name.c_str());
		}

		std::string m_Name;
	};

	class NamedAssetMgr : public envSharedAssetMgr<NamedAsset, std::string>
	{
		virtual NamedAsset* LoadAsset(const std::string& i_Name)
		{
			return new NamedAsset(i_Name);
		}
	};

}

//========================================================================
//	RunTest - executes the envSTLHelpers tests
//========================================================================
void envSharedAssetTest::RunTest()
{
	DBG_LOG0("Begin envSharedAssetTest::RunTest()");

	NamedAssetMgr asset_mgr;
	NamedAsset *pAsset1 = asset_mgr.Load("Resource #1");
	NamedAsset *pAsset2 = asset_mgr.Load("Resource #2");
	NamedAsset *pAsset3 = asset_mgr.Load("Resource #1");

	asset_mgr.Release(pAsset3);
	asset_mgr.Release(pAsset2);
	asset_mgr.Release(pAsset1);

	DBG_LOG0("\nEnd envSharedAssetTest::RunTest()");
}
