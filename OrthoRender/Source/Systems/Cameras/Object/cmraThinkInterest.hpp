/*****************************************************************************
**  cmraThinkInterest.hpp
**
**      the Think interest for system Cameras.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef CMRA_THINKINTEREST_HPP
#error cmraThinkInterest.hpp multiply included
#endif
#define CMRA_THINKINTEREST_HPP

#include "Support/mnm/mnmThinkInterest.hpp"


//============================================================================
//============================================================================
class cmraThinkInterest : public mnmThinkInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmraThinkInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmraThinkInterest();

		//--------------------------------------------------------------------
		//	Think
		//--------------------------------------------------------------------
		virtual void Think( );
};
