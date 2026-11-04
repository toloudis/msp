/****************************************************************************\
**  fsFileErrorUtil.hpp
**
**      fsFileErrorUtil throws the fs exception (fsFileX.hpp) for an error a
**      file operation reported as a std::error_code: from std::filesystem,
**      or an operating system error code in std::system_category(). The
**      error is matched against portable std::errc conditions, so the same
**      code serves every platform.
**
**      Not part of the public fs interface.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef FS_FILEERRORUTIL_HPP
#error fsFileErrorUtil.hpp multiply included
#endif
#define FS_FILEERRORUTIL_HPP

#include <system_error>


class fsLocator;


namespace fsFileErrorUtil
{
	//----------------------------------------------------------------------------
	//	Throw throws the exception for an error on one file or directory.
	//	Something missing is fsDirectoryDoesntExistX when the directory it
	//	should be in is missing too, else fsFileDoesntExistX.
	//----------------------------------------------------------------------------
	[[noreturn]] void Throw(const std::error_code& i_Error, const fsLocator& i_Locator);

	//----------------------------------------------------------------------------
	//	Throw throws the exception for an error copying or moving i_From to
	//	i_To, naming whichever of the two the error is about.
	//----------------------------------------------------------------------------
	[[noreturn]] void Throw(const std::error_code& i_Error, const fsLocator& i_From, const fsLocator& i_To);

	//----------------------------------------------------------------------------
	//	ThrowMissing throws as Throw does when i_Locator doesn't exist.
	//----------------------------------------------------------------------------
	[[noreturn]] void ThrowMissing(const fsLocator& i_Locator);
}
