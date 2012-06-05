/****************************************************************************\
**	pythLog.hpp
**
**		Implements a module to capture output from Python interpretor.
**
**	StudioGPU
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

	//--------------------------------------------------------------------
	// Redirect logging to this new function temporarilly
	//--------------------------------------------------------------------
	void SetTemporaryLogFunctions(LogFunction i_pLogOutputFunc,
								   LogFunction i_pLogErrorFunc);

	//--------------------------------------------------------------------
	// Restore logging to the main logging functions given in Init()
	//--------------------------------------------------------------------
	void RestoreLogFunctions();
};
