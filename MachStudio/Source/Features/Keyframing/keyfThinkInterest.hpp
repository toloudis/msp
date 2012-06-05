/*****************************************************************************
**  keyfThinkInterest.hpp
**
**      the Think interest for auto-keyframing.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef KEYF_THINKINTEREST_HPP
#error keyfThinkInterest.hpp multiply included
#endif
#define KEYF_THINKINTEREST_HPP

#ifndef MNM_THINKINTEREST_HPP
#include "Support/mnm/mnmThinkInterest.hpp"
#endif


//============================================================================
//============================================================================
class keyfThinkInterest : public mnmThinkInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		keyfThinkInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~keyfThinkInterest();

		//--------------------------------------------------------------------
		//	Think
		//--------------------------------------------------------------------
		virtual void Think( );
};
