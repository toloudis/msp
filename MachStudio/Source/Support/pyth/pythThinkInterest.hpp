/*****************************************************************************
**  pythThinkInterest.hpp
**
**      the Think interest for python commands that need their own thread.
**			i.e. wxPython forms
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef PYTH_THINKINTEREST_HPP
#error pythThinkInterest.hpp multiply included
#endif
#define PYTH_THINKINTEREST_HPP

#include "Support/mnm/mnmThinkInterest.hpp"


//============================================================================
//============================================================================
class pythThinkInterest : public mnmThinkInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pythThinkInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~pythThinkInterest();

		//--------------------------------------------------------------------
		//	Think
		//--------------------------------------------------------------------
		virtual void Think( );
};
