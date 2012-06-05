/*****************************************************************************
**	mslHeaderFooter.hpp
**
**	 mslHeaderFooter handles the wrapper part of shader generation,
**	decrypting a template from memory.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_HEADERFOOTER_HPP
#error mslHeaderFooter.hpp multiply included
#endif
#define MSL_HEADERFOOTER_HPP

#include <string>

//============================================================================
//============================================================================
namespace mslHeaderFooter 
{	
	//------------------------------------------------------------------------
	//	DecryptHeader
	//------------------------------------------------------------------------
	void DecryptHeader(std::string &o_Header);

	//------------------------------------------------------------------------
	//	DecryptFooter
	//------------------------------------------------------------------------
	void DecryptFooter(std::string &o_Footer);
}

