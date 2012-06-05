/*****************************************************************************
**  mtrStaticCubeSaver.hpp
**
**      The mtrStaticCubeSaver writes static cube filenames into 
**	.scm file.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MT_STATICCUBESAVER_HPP
#error mtrStaticCubeSaver.hpp multiply included
#endif
#define MT_STATICCUBESAVER_HPP

#include <string>

class fsLocator;

namespace mtrStaticCubeSaver
{
		
	//========================================================================
	// Write - writes filenames to text file
	//========================================================================
	void	Write(  const fsLocator &i_Locator,
					const std::string i_Filenames[6]);

	//========================================================================
	// Read - reads six strings from text file.
	//========================================================================
	bool	Read(  const fsLocator &i_Locator,
					std::string o_Filenames[6]);

}
