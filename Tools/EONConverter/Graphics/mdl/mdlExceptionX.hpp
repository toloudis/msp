/****************************************************************************\
**  mdlExceptionX.hpp
**
**      mdlExceptionX.hpp defines the exceptions that can be thrown from the
**	mdl package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MDL_EXCEPTIONX_HPP
#error mdlExceptionX.hpp multiply included
#endif
#define MDL_EXCEPTIONX_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "Core/env/envExceptionX.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class mdlInvalidModelFileX : public envExceptionX
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mdlInvalidModelFileX(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~mdlInvalidModelFileX();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const fsLocator& GetLocator() const;

		//--------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//--------------------------------------------------------------------
		//virtual envError::Code Index() const;

	private:

		fsLocator m_Locator;
};

