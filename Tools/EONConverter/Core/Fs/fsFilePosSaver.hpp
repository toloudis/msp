/****************************************************************************\
**  fsFilePosSaver.hpp
**
**      fsFilePosSaver is a handy class which saves and restores the
**	position of a stream class.  To use it, create one, then move the file
**	position however you want.  When the destructor is called, it will
**	move the file position back to the original position.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef FS_FILEPOSSAVER_HPP
#error fsFilePosSaver.hpp multiply included
#endif
#define FS_FILEPOSSAVER_HPP

template <class T>
class fsFilePosSaver
{
	public:

		//====================================================================
		//	Create the fsFilePosSaver with by giving it the file whose
		//	position you want saved.
		//====================================================================
		fsFilePosSaver(T& io_File);

		//====================================================================
		//	When the destructor is called, the position of the file when
		//	the constructor was called will be restored.
		//====================================================================
		~fsFilePosSaver();

		//====================================================================
		//	GetPos returns the position saved (counted from the beginning of
		//	the file).
		//====================================================================
		int GetPos() const;

	private:

		T& m_File;
		int m_Pos;
};

template <class T>
inline
fsFilePosSaver<T>::fsFilePosSaver(T& io_File)
:	m_File(io_File),
	m_Pos(io_File.GetFilePos())
{	
}

template <class T>
inline 
fsFilePosSaver<T>::~fsFilePosSaver()
{
	m_File.SetFilePos(m_Pos);
}

template <class T>
inline int 
fsFilePosSaver<T>::GetPos() const
{ 
	return m_Pos;
}
