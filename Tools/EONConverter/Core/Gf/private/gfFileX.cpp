/*****************************************************************************
**  gfFileX.cpp
**
**      gfFileX.cpp defines the exception classes used by the gf package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/gf/gfFileX.hpp"

//#include "Core/env/envPackageErrorIndices.hpp"
//#include "Core/gf/gfErrorCodes.hpp"

//============================================================================
//	gfInvalidFileBinX is thrown when the header of a File is invalid
//============================================================================
gfInvalidFileBinX::gfInvalidFileBinX(const fsLocator& i_Locator) : 
		m_Locator(i_Locator)
{
}

//========================================================================
//========================================================================
gfInvalidFileBinX::~gfInvalidFileBinX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code gfInvalidFileBinX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Gf, gfErrorCodes::e_InvalidFileBin);
//}

//============================================================================
//	GetLocator returns the locator of the Invalid FileBin
//============================================================================
const fsLocator& gfInvalidFileBinX::GetLocator() const
{
	return m_Locator;
}


//============================================================================
//	gfInvalidTokenFileTxtX is thrown when the header of a File is invalid
//============================================================================
gfInvalidTokenFileTxtX::gfInvalidTokenFileTxtX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//========================================================================
//========================================================================
gfInvalidTokenFileTxtX::~gfInvalidTokenFileTxtX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code gfInvalidTokenFileTxtX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Gf, gfErrorCodes::e_InvalidFileBin);
//}

//============================================================================
//	GetLocator returns the offending locator
//============================================================================
const fsLocator& gfInvalidTokenFileTxtX::GetLocator() const
{
	return m_Locator;
}

//============================================================================
//	gfInvalidPakFileX is thrown when the header of a File is invalid
//============================================================================
gfInvalidPakFileX::gfInvalidPakFileX(const fsLocator& i_Locator) :
	m_Locator(i_Locator)
{
}

//======================================================================== 
//========================================================================
gfInvalidPakFileX::~gfInvalidPakFileX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code gfInvalidPakFileX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Gf, gfErrorCodes::e_InvalidPakFile);
//}

//============================================================================
//	GetLocator returns the locator of the Invalid TextSetFile
//============================================================================
const fsLocator& gfInvalidPakFileX::GetLocator() const
{
	return m_Locator;
}

//========================================================================
//	gfFileNotInPakX is thrown when the requested file is not in a Pakfile
//========================================================================
gfFileNotInPakX::gfFileNotInPakX(const fsLocator& i_Locator) :
	m_Locator(i_Locator)
{
}

//======================================================================== 
//========================================================================
gfFileNotInPakX::~gfFileNotInPakX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code gfFileNotInPakX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Gf, gfErrorCodes::e_FileNotInPak);
//}

//============================================================================
//	GetLocator returns the locator of the Invalid TextSetFile
//============================================================================
const fsLocator& gfFileNotInPakX::GetLocator() const
{
	return m_Locator;
}

//========================================================================
//	gfFileCorruptX is thrown when the compressed data in a FileInPak
//  has been corrupted.
//========================================================================
gfFileCorruptX::gfFileCorruptX()
{
}
//========================================================================
//========================================================================
gfFileCorruptX::~gfFileCorruptX()
{
}

//========================================================================
//	Index() returns the error code/string index.
//========================================================================
//envError::Code gfFileCorruptX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Gf, gfErrorCodes::e_FileCorrupt);
//}

//========================================================================
//	gfReadErrorX is thrown when there is not enough memory to 
//  complete inflation of compressed data.
//========================================================================
gfReadErrorX::gfReadErrorX()
{
}

//========================================================================
//========================================================================
gfReadErrorX::~gfReadErrorX()
{
}

//========================================================================
//	Index() returns the error code/string index.
//========================================================================
//envError::Code gfReadErrorX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Gf, gfErrorCodes::e_ReadError);
//}


//========================================================================
//	gfNoMemToInflateX is thrown when there is not enough memory to 
//  complete inflation of compressed data.
//========================================================================
gfNoMemToInflateX::gfNoMemToInflateX()
{
}

//========================================================================
//========================================================================
gfNoMemToInflateX::~gfNoMemToInflateX()
{
}

//========================================================================
//	Index() returns the error code/string index.
//========================================================================
//envError::Code gfNoMemToInflateX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Gf, gfErrorCodes::e_NoMemToInflate);
//}

//============================================================================
//	gfInvalidTextSetFileX is thrown when the header of a File is invalid
//============================================================================
gfInvalidTextSetFileX::gfInvalidTextSetFileX(const fsLocator& i_Locator) : 
		m_Locator(i_Locator)
{
}

//======================================================================== 
//========================================================================
gfInvalidTextSetFileX::~gfInvalidTextSetFileX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code gfInvalidTextSetFileX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Gf, gfErrorCodes::e_InvalidTextSetFile);
//}

//============================================================================
//	GetLocator returns the locator of the Invalid TextSetFile
//============================================================================
const fsLocator& gfInvalidTextSetFileX::GetLocator() const
{
	return m_Locator;
}
