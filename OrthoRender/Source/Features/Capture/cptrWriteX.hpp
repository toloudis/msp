/****************************************************************************\
**  cptrWriteUtil.hpp
**
**      cptrWriteX.hpp defines the exception classes used by cptr writes.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_WRITEX_HPP
#error cptrWriteX.hpp multiply included
#endif
#define CPTR_WRITEX_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "Core/env/envExceptionX.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//	cptrWriteBufferOverrunX is thrown when the buffer is *about* to be 
//	overrun.
//============================================================================
class cptrWriteBufferOverrunX : public envExceptionX
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrWriteBufferOverrunX();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cptrWriteBufferOverrunX();

		//--------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//--------------------------------------------------------------------
		virtual envError::Code Index() const;
};


//============================================================================
//	cptrUnknownX is thrown when no other exception fits
//============================================================================
class cptrUnknownX : public envExceptionX
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrUnknownX(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cptrUnknownX();

		//--------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//--------------------------------------------------------------------
		virtual envError::Code Index() const;

		//--------------------------------------------------------------------
		//	GetLocator() returns the offending directory
		//--------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};
