/****************************************************************************\
**  gfFileX.hpp
**
**      gfFileX.hpp defines the exception classes used by the gf package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GF_FILEX_HPP
#error gfFileX.hpp multiply included
#endif
#define GF_FILEX_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "Core/env/envExceptionX.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

class gfInvalidFileBinX : public envExceptionX
{
	public:
		//========================================================================
		//	gfInvalidFileBinX is thrown when the header of a FileBin is invalid
		//========================================================================
		gfInvalidFileBinX(const fsLocator& i_Locator);

		//========================================================================
		//========================================================================
		virtual ~gfInvalidFileBinX();

		//========================================================================
		//	Index() returns the error code/string index.
		//========================================================================
		//virtual envError::Code Index() const;

		//========================================================================
		//	GetLocator() returns the offending locator
		//========================================================================
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

class gfInvalidTokenFileTxtX : public envExceptionX
{
	public:
		//========================================================================
		//	gfInvalidTokenFileTxtX is thrown when a quoted token is malformed
		//========================================================================
		gfInvalidTokenFileTxtX(const fsLocator& i_Locator);

		//========================================================================
		//========================================================================
		virtual ~gfInvalidTokenFileTxtX();

		//========================================================================
		//	Index() returns the error code/string index.
		//========================================================================
		//virtual envError::Code Index() const;

		//========================================================================
		//	GetLocator() returns the offending locator
		//========================================================================
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};

class gfInvalidPakFileX : public envExceptionX
{
	public:
		//========================================================================
		//	gfInvalidPakFileX is thrown when the format of a PakFile is invalid
		//========================================================================
		gfInvalidPakFileX(const fsLocator& i_Locator);

		//========================================================================
		//========================================================================
		virtual ~gfInvalidPakFileX();

		//========================================================================
		//	Index() returns the error code/string index.
		//========================================================================
		//virtual envError::Code Index() const;

		//========================================================================
		//	GetLocator() returns the offending locator
		//========================================================================
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;

};

class gfFileNotInPakX : public envExceptionX
{
	public:
		//========================================================================
		//	gfFileNotInPakX is thrown when the requested file is not in a Pakfile
		//========================================================================
		gfFileNotInPakX(const fsLocator& i_Locator);

		//========================================================================
		//========================================================================
		virtual ~gfFileNotInPakX();

		//========================================================================
		//	Index() returns the error code/string index.
		//========================================================================
		//virtual envError::Code Index() const;

		//========================================================================
		//	GetLocator() returns the offending locator
		//========================================================================
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;

};

class gfFileCorruptX : public envExceptionX
{
	public:
		//========================================================================
		//	gfFileCorruptX is thrown when the data in a FileInPak
		//  has been corrupted.
		//========================================================================
		gfFileCorruptX();

		//========================================================================
		//========================================================================
		virtual ~gfFileCorruptX();

		//========================================================================
		//	Index() returns the error code/string index.
		//========================================================================
		//virtual envError::Code Index() const;

};

class gfNoMemToInflateX : public envExceptionX
{
	public:
		//========================================================================
		//	gfNoMemToInflateX is thrown when there is not enough memory to 
		//  complete inflation of compressed data.
		//========================================================================
		gfNoMemToInflateX();

		//========================================================================
		//========================================================================
		virtual ~gfNoMemToInflateX();

		//========================================================================
		//	Index() returns the error code/string index.
		//========================================================================
		//virtual envError::Code Index() const;

};

class gfReadErrorX : public envExceptionX
{
	public:
		//========================================================================
		//	gfReadErrorX is thrown when there is not enough memory to 
		//  complete inflation of compressed data.
		//========================================================================
		gfReadErrorX();

		//========================================================================
		//========================================================================
		virtual ~gfReadErrorX();

		//========================================================================
		//	Index() returns the error code/string index.
		//========================================================================
		//virtual envError::Code Index() const;

};

class gfInvalidTextSetFileX : public envExceptionX
{
	public:
		//========================================================================
		//	gfInvalidTextSetFileX is thrown when the header of a TextSetFile is invalid
		//========================================================================
		gfInvalidTextSetFileX(const fsLocator& i_Locator);

		//========================================================================
		//========================================================================
		virtual ~gfInvalidTextSetFileX();

		//========================================================================
		//	Index() returns the error code/string index.
		//========================================================================
		//virtual envError::Code Index() const;

		//========================================================================
		//	GetLocator() returns the offending locator
		//========================================================================
		const fsLocator& GetLocator() const;

	private:
		fsLocator m_Locator;
};