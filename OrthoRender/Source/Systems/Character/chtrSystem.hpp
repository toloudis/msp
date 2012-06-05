/*****************************************************************************
**  chtrSystem.hpp
**
**      System for adding dynamic objects to the world.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef CHTR_SYSTEM_HPP
#error chtrSystem.hpp multiply included
#endif
#define CHTR_SYSTEM_HPP

//============================================================================
//============================================================================
class fsLocator;
class itString;


//============================================================================
//============================================================================
namespace chtrSystem
{
		//--------------------------------------------------------------------
		// Init -- initialize system with directory to use to look for
		//		geometry files
		//
		//	SoundDir should be the post-fix dir path under the app path.
		//--------------------------------------------------------------------
		void Init(const fsLocator& i_AppDir, const itString& i_CharacterDataDir);

		//--------------------------------------------------------------------
		// CleanUp -- cleanup system
		//--------------------------------------------------------------------
		void CleanUp();

};
