/*****************************************************************************
**  cptrWriteX.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrWriteX.hpp"

#include "Features/Capture/cptrErrorCodes.hpp"

#include "Support/mnm/mnmAppErrorIndices.hpp"


//----------------------------------------------------------------------------
//	cptrWriteBufferOverrunX is thrown when the buffer is *about* to be 
//	overrun.
//----------------------------------------------------------------------------
cptrWriteBufferOverrunX::cptrWriteBufferOverrunX()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrWriteBufferOverrunX::~cptrWriteBufferOverrunX()
{
}

//----------------------------------------------------------------------------
//	Index() returns the error code/string index.
//----------------------------------------------------------------------------
envError::Code cptrWriteBufferOverrunX::Index() const
{
	// FIX: create error indices for the application

	return envError::GetCode(mnmAppErrorIndices::e_Cptr, cptrErrorCodes::e_WriteBufferOverrun);
}



//****************************************************************************
//	cptrUnknownX
//****************************************************************************

//----------------------------------------------------------------------------
//	cptrUnknownX is thrown when the buffer is *about* to be 
//	overrun.
//----------------------------------------------------------------------------
cptrUnknownX::cptrUnknownX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrUnknownX::~cptrUnknownX()
{
}

//----------------------------------------------------------------------------
//	Index() returns the error code/string index.
//----------------------------------------------------------------------------
envError::Code cptrUnknownX::Index() const
{
	return envError::GetCode(mnmAppErrorIndices::e_Cptr, cptrErrorCodes::e_Unknown);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const fsLocator& cptrUnknownX::GetLocator() const
{
	return m_Locator;
}
