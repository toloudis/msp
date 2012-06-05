/****************************************************************************\
**	pythLog.hpp
**
**		Implements a module to capture output from Python interpretor.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef PYTH_LOG_HPP
#error pythLog.hpp multiply included
#endif
#define PYTH_LOG_HPP


//============================================================================
//============================================================================
namespace pythLog
{
	typedef void (*LogFunction)(const char*);

	//--------------------------------------------------------------------
	// The pythPackage must be initialized before this is called
	//--------------------------------------------------------------------
	void Initialize(LogFunction i_pLogOutputFunc,
					LogFunction i_pLogErrorFunc);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize();
};
