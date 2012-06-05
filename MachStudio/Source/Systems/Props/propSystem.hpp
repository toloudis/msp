/*****************************************************************************
**  propSystem.hpp
**
**      System for adding dynamic objects to the world.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_SYSTEM_HPP
#error propSystem.hpp multiply included
#endif
#define PROP_SYSTEM_HPP


//============================================================================
//============================================================================
class itString;


//============================================================================
//============================================================================
namespace propSystem
{
		//--------------------------------------------------------------------
		// Init -- initialize system with directory to use to look for
		//		geometry files
		//
		//	SoundDir should be the post-fix dir path under the app path.
		//--------------------------------------------------------------------
		void Init(const itString& i_PropDataDirName);

		//--------------------------------------------------------------------
		// CleanUp -- cleanup system
		//--------------------------------------------------------------------
		void CleanUp();
};
