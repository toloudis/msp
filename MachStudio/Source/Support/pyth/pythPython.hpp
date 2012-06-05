/****************************************************************************\
**	pythPython.hpp
**
**		Include of python header
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef PYTH_PYTHON_HPP
#error pythPython.hpp multiply included
#endif
#define PYTH_PYTHON_HPP

#ifndef MNM_CONSTANTS_HPP
#include "Support/mnm/mnmConstants.hpp"
#endif


#define PYTHON_ENABLED

#if defined(PYTHON_ENABLED)
#include <Python.h>
#endif

//#endif

#if defined(PYTHON_ENABLED)
	//------------------------------------------------------------------------
	// Used to wrap a python call with the thread preparation functions
	//------------------------------------------------------------------------
	class pythThreadLock
	{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pythThreadLock()
		{
			m_gstate = PyGILState_Ensure();
		};
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~pythThreadLock()
		{
			PyGILState_Release(m_gstate);
		};

	public:
		PyGILState_STATE m_gstate;
	};
#endif

