/*****************************************************************************
**  pytFeature.hpp
**
**      Feature PythonObject
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef PYT_FEATURE_HPP
#error pytFeature.hpp multiply included
#endif
#define PYT_FEATURE_HPP


namespace pytFeature
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
