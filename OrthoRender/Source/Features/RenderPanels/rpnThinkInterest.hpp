/*****************************************************************************
**  rpnThinkInterest.hpp
**
**      the Think interest for render panels
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_THINKINTEREST_HPP
#error rpnThinkInterest.hpp multiply included
#endif
#define RPN_THINKINTEREST_HPP

#ifndef MNM_THINKINTEREST_HPP
#include "Support/mnm/mnmThinkInterest.hpp"
#endif


//============================================================================
//============================================================================
class rpnThinkInterest : public mnmThinkInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		rpnThinkInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~rpnThinkInterest();

		//--------------------------------------------------------------------
		//	Think
		//--------------------------------------------------------------------
		virtual void Think( );
};
