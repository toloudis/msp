/*****************************************************************************
**  fsFileUtil.cpp
**
**      fsFileX.cpp defines the exception classes used by the fs package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/fs/fsFileX.hpp"

//#include "Core/env/envPackageErrorIndices.hpp"
//#include "Core/fs/fsErrorCodes.hpp"

//============================================================================
//	fsDirectoryDoesntExistX is thrown when the user tries to do something
//	with a directory that doesn't exist.
//============================================================================
fsDirectoryDoesntExistX::fsDirectoryDoesntExistX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//============================================================================
//============================================================================
fsDirectoryDoesntExistX::~fsDirectoryDoesntExistX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code fsDirectoryDoesntExistX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Fs, fsErrorCodes::e_DirectoryDoesntExist);
//}

//============================================================================
//============================================================================
const fsLocator& fsDirectoryDoesntExistX::GetLocator() const
{
	return m_Locator;
}

//****************************************************************************
//============================================================================
//	fsDirectoryExistsX is thrown when the user tries to create a directory
//	that already exists.
//============================================================================
fsDirectoryExistsX::fsDirectoryExistsX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//============================================================================
//============================================================================
fsDirectoryExistsX::~fsDirectoryExistsX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code fsDirectoryExistsX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Fs, fsErrorCodes::e_DirectoryExists);
//}

//============================================================================
//============================================================================
const fsLocator& fsDirectoryExistsX::GetLocator() const
{
	return m_Locator;
}

//****************************************************************************
//============================================================================
//	fsDirectoryNotEmptyX is thrown when the user tries to delete a directory
//	which contains files or other directories.
//============================================================================
fsDirectoryNotEmptyX::fsDirectoryNotEmptyX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//============================================================================
//============================================================================
fsDirectoryNotEmptyX::~fsDirectoryNotEmptyX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code fsDirectoryNotEmptyX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Fs, fsErrorCodes::e_DirectoryNotEmpty);
//}

//============================================================================
//============================================================================
const fsLocator& fsDirectoryNotEmptyX::GetLocator() const
{
	return m_Locator;
}

//****************************************************************************
//============================================================================
//	fsFileDoesntExistX is thrown when the user tries to do something with
//	a file that doesn't exist.
//============================================================================
fsFileDoesntExistX::fsFileDoesntExistX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//============================================================================
//============================================================================
fsFileDoesntExistX::~fsFileDoesntExistX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code fsFileDoesntExistX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Fs, fsErrorCodes::e_FileDoesntExist);
//}


//============================================================================
//============================================================================
const fsLocator& fsFileDoesntExistX::GetLocator() const
{
	return m_Locator;
}

//****************************************************************************
//============================================================================
//	fsFileExistsX is thrown when the user tries to create a file that already
//	exists.
//============================================================================
fsFileExistsX::fsFileExistsX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//============================================================================
//============================================================================
fsFileExistsX::~fsFileExistsX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code fsFileExistsX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Fs, fsErrorCodes::e_FileExists);
//}


//============================================================================
//============================================================================
const fsLocator& fsFileExistsX::GetLocator() const
{
	return m_Locator;
}

//****************************************************************************
//============================================================================
//	fsFileInUseX is thrown when the user tries to operate on a file that is
//	already open (possibly by another process).
//============================================================================
fsFileInUseX::fsFileInUseX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//============================================================================
//============================================================================
fsFileInUseX::~fsFileInUseX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code fsFileInUseX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Fs, fsErrorCodes::e_FileInUse);
//}


//============================================================================
//============================================================================
const fsLocator& fsFileInUseX::GetLocator() const
{
	return m_Locator;
}

//****************************************************************************
//============================================================================
//	fsInvalidLocatorX is thrown when a locator contains characters that the
//	OS will not allow in a filename, or is flawed in some other way.
//	(too long?)
//============================================================================
fsInvalidLocatorX::fsInvalidLocatorX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//============================================================================
//============================================================================
fsInvalidLocatorX::~fsInvalidLocatorX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code fsInvalidLocatorX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Fs, fsErrorCodes::e_InvalidLocator);
//}


//============================================================================
//============================================================================
const fsLocator& fsInvalidLocatorX::GetLocator() const
{
	return m_Locator;
}

//****************************************************************************
//============================================================================
//	fsReadOnlyX is thrown when the user tries to write to a file marked as
//	read only.
//============================================================================
fsReadOnlyX::fsReadOnlyX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//============================================================================
//============================================================================
fsReadOnlyX::~fsReadOnlyX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code fsReadOnlyX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Fs, fsErrorCodes::e_ReadOnly);
//}


//============================================================================
//============================================================================
const fsLocator& fsReadOnlyX::GetLocator() const
{
	return m_Locator;
}

//****************************************************************************
//============================================================================
//	fsDiskFullX is thrown when the user tries to write to a file 
//	but there is not disk space left or the disk is corrupt
//============================================================================
fsDiskFullX::fsDiskFullX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//============================================================================
//============================================================================
fsDiskFullX::~fsDiskFullX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code fsDiskFullX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Fs, fsErrorCodes::e_DiskFull);
//}


//============================================================================
//============================================================================
const fsLocator& fsDiskFullX::GetLocator() const
{
	return m_Locator;
}

//****************************************************************************
//============================================================================
//	fsUnknownX is thrown when an error is not covered by the above exceptions.
//============================================================================
fsUnknownX::fsUnknownX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//============================================================================
//============================================================================
fsUnknownX::~fsUnknownX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code fsUnknownX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Fs, fsErrorCodes::e_Unknown);
//}

//============================================================================
//============================================================================
const fsLocator& fsUnknownX::GetLocator() const
{
	return m_Locator;
}


//****************************************************************************
//============================================================================
//	fsXMLErrorX is thrown when an XML error occurs.
//============================================================================
fsXMLErrorX::fsXMLErrorX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//============================================================================
//============================================================================
fsXMLErrorX::~fsXMLErrorX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
//envError::Code fsXMLErrorX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Fs, fsErrorCodes::e_XMLError);
//}

//============================================================================
//============================================================================
const fsLocator& fsXMLErrorX::GetLocator() const
{
	return m_Locator;
}


