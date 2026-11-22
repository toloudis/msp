/*****************************************************************************
**  optxLoader.hpp
**
**      optxLoader loads OptiX from the display driver (nvoptix.dll) and
**	fills the OptiX function table, through which every OptiX call goes.
**	Load must succeed before any OptiX function is called.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXLOADER_HPP
#error optxLoader.hpp multiply included
#endif
#define GOX_OPTXLOADER_HPP

#include <string>

namespace gox
{
	class optxLoader
	{
		public:
			//--------------------------------------------------------------------
			//	Load - loads OptiX; returns false (o_Error says why) if the
			//	driver has no OptiX, or one older than this SDK. Loading again
			//	once loaded does nothing.
			//--------------------------------------------------------------------
			static bool Load(std::string& o_Error);

			//--------------------------------------------------------------------
			//	Unload - releases the driver's library. No OptiX object may be
			//	alive.
			//--------------------------------------------------------------------
			static void Unload();

			//--------------------------------------------------------------------
			//	IsLoaded
			//--------------------------------------------------------------------
			static bool IsLoaded();
	};
}
