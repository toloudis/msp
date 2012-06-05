/*****************************************************************************
**  chnlPackage.hpp
**
**      Package for handling Channel Editor GUI
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CHNL_PACKAGE_HPP
#error chnlPackage.hpp multiply included
#endif
#define CHNL_PACKAGE_HPP


namespace chnlPackage
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
