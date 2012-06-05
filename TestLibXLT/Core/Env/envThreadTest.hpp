/*****************************************************************************
**  envThreadTest.hpp
**
**      envThreadTest is a test of envThread.hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef ENV_THREADTEST_HPP
#error envThreadTest.hpp multiply included
#endif
#define ENV_THREADTEST_HPP

//====================================================================
//	Forward References
//====================================================================

namespace envThreadTest
{
	//========================================================================
	//	RunTest - executes the boost library tests
	//========================================================================
	void RunTest();
}
