/****************************************************************************\
**  fsFileUtil.hpp
**
**      fsFileX.hpp defines the exception classes used by the fs package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef FS_FILEX_HPP
#error fsFileX.hpp multiply included
#endif
#define FS_FILEX_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "Core/env/envExceptionX.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//	fsDirectoryDoesntExistX is thrown when the user tries to do something
//	with a directory that doesn't exist.
//============================================================================
class fsDirectoryDoesntExistX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		fsDirectoryDoesntExistX(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~fsDirectoryDoesntExistX();

		//------------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------------
		//virtual envError::Code Index() const;

		//------------------------------------------------------------------------
		//	GetLocator() returns the offending directory
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

//============================================================================
//	fsDirectoryExistsX is thrown when the user tries to create a directory
//	that already exists.
//============================================================================
class fsDirectoryExistsX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		fsDirectoryExistsX(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~fsDirectoryExistsX();

		//------------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------------
		//virtual envError::Code Index() const;

		//------------------------------------------------------------------------
		//	GetLocator() returns the offending directory
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

//============================================================================
//	fsDirectoryNotEmptyX is thrown when the user tries to delete a directory
//	which contains files or other directories.
//============================================================================
class fsDirectoryNotEmptyX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		fsDirectoryNotEmptyX(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~fsDirectoryNotEmptyX();

		//------------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------------
		//virtual envError::Code Index() const;

		//------------------------------------------------------------------------
		//	GetLocator() returns the offending directory
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

//============================================================================
//	fsFileDoesntExistX is thrown when the user tries to do something with
//	a file that doesn't exist.
//============================================================================
class fsFileDoesntExistX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		fsFileDoesntExistX(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~fsFileDoesntExistX();

		//------------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------------
		//virtual envError::Code Index() const;

		//------------------------------------------------------------------------
		//	GetLocator() returns the offending filename
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

//============================================================================
//	fsFileExistsX is thrown when the user tries to create a file that already
//	exists.
//============================================================================
class fsFileExistsX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		fsFileExistsX(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~fsFileExistsX();

		//------------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------------
		//virtual envError::Code Index() const;

		//------------------------------------------------------------------------
		//	GetLocator() returns the offending file
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

//============================================================================
//	fsFileInUseX is thrown when the user tries to operate on a file that is
//	already open (possibly by another process).
//============================================================================
class fsFileInUseX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		fsFileInUseX(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~fsFileInUseX();

		//------------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------------
		//virtual envError::Code Index() const;

		//------------------------------------------------------------------------
		//	GetLocator() returns the offending file
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

//============================================================================
//	fsInvalidLocatorX is thrown when a locator contains characters that the
//	OS will not allow in a filename, or is flawed in some other way.
//	(too long?)
//============================================================================
class fsInvalidLocatorX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		fsInvalidLocatorX(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~fsInvalidLocatorX();

		//------------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------------
		//virtual envError::Code Index() const;

		//------------------------------------------------------------------------
		//	GetLocator() returns the offending locator
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

//============================================================================
//	fsReadOnlyX is thrown when the user tries to write to a file marked as
//	read only.
//============================================================================
class fsReadOnlyX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		fsReadOnlyX(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~fsReadOnlyX();

		//------------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------------
		//virtual envError::Code Index() const;

		//------------------------------------------------------------------------
		//	GetLocator() returns the offending filename
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

//============================================================================
//	fsDiskFullX is thrown when the user tries to write to a file 
//	but there is not disk space left or the disk is corrupt
//============================================================================
class fsDiskFullX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		fsDiskFullX(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~fsDiskFullX();

		//------------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------------
		//virtual envError::Code Index() const;

		//------------------------------------------------------------------------
		//	GetLocator() returns the offending filename
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

//============================================================================
//	fsUnknownX is thrown when an error is not covered by the above exceptions.
//
//============================================================================
class fsUnknownX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		fsUnknownX(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~fsUnknownX();

		//------------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------------
		//virtual envError::Code Index() const;

		//------------------------------------------------------------------------
		//	GetLocator() returns the offending locator
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

//============================================================================
//	fsXMLErrorX is thrown when an error reading an XML file occurs
//
//============================================================================
class fsXMLErrorX : public envExceptionX
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		fsXMLErrorX(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual ~fsXMLErrorX();

		//------------------------------------------------------------------------
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------------
		//virtual envError::Code Index() const;

		//------------------------------------------------------------------------
		//	GetLocator() returns the offending locator
		//------------------------------------------------------------------------
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

