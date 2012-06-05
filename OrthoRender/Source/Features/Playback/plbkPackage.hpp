/*****************************************************************************
**  plbkPackage.hpp
**
**      Initializes feature Playback
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef PLBK_PACKAGE_HPP
#error plbkPackage.hpp multiply included
#endif
#define PLBK_PACKAGE_HPP


namespace plbkPackage
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
