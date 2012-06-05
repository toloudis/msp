/*****************************************************************************
**  keyfPackage.hpp
**
**      Package for handling "one-step" Keyframing  
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef KEYF_PACKAGE_HPP
#error keyfPackage.hpp multiply included
#endif
#define KEYF_PACKAGE_HPP

namespace keyfPackage
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init();

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp();
};
