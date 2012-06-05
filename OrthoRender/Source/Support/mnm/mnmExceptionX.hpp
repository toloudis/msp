/****************************************************************************\
**  mnmExceptionX.hpp
**
**      mnmExceptionX.hpp defines the game-specific exception classes used
**	by Ultimate Ride.
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_EXCEPTIONX_HPP
#error mnmExceptionX.hpp multiply included
#endif
#define MNM_EXCEPTIONX_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "Core/env/envExceptionX.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

class mnmInvalidTileFileX : public envExceptionX
{
	public:

		//========================================================================
		//	mnmInvalidTileFileX is thrown when a prop file (.prd) is either
		//	corrupt or is an unknown version.
		//========================================================================
		mnmInvalidTileFileX(const fsLocator& i_Locator);

		//========================================================================
		//========================================================================
		virtual ~mnmInvalidTileFileX();

		//========================================================================
		//	Index() returns the error code/string index.
		//========================================================================
		virtual envError::Code Index() const;

		//========================================================================
		//	GetLocator() returns the offending locator
		//========================================================================
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};
