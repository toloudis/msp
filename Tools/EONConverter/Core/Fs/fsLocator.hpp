/**********************************************************************
**  fsLocator.hpp
**
**      fsLocator represents the name of a file on a disk.
**	The fsLocator is composed of path names, which are
**	represented to the user as itStrings, in order that
**	foreign language file systems can be supported.
**	The fsLocator also has (optionally) an index, representing a 
**	directory (gfPaths), in which case the locator is generally
**	considered to contain a relative path.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************************/
#ifdef FS_LOCATOR_HPP
#error fsLocator.hpp multiply included
#endif
#define FS_LOCATOR_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class fsLocator
{
	public:
		//------------------------------------------------------------------------
		//	Constructors
		//------------------------------------------------------------------------
		fsLocator();
		fsLocator(int i_Index);
		fsLocator(const fsLocator& i_Locator);
		fsLocator(const fsLocator& i_Locator, int i_Index);
		fsLocator(const itString& i_Name);
		// This version sets the index and pushes the given name
		fsLocator(int i_Index, const itString& i_Name);

		//------------------------------------------------------------------------
		//	Destructor
		//------------------------------------------------------------------------
		~fsLocator();

		//------------------------------------------------------------------------
		//	HasIndex will return true if the locator is considered to be relative
		//	to a stored directory.
		//------------------------------------------------------------------------
		bool HasIndex() const;

		//------------------------------------------------------------------------
		//	GetIndex returns an index of a directory to be used as the base for
		//	the locator (usually gfPath).
		//------------------------------------------------------------------------
		int GetIndex() const;

		//------------------------------------------------------------------------
		//	SetIndex sets the directory index of the locator.  This is the same
		//	as using the constructor with the index parameter.  The index can't
		//	be a negative number.
		//------------------------------------------------------------------------
		void SetIndex(int i_Index);

		//------------------------------------------------------------------------
		//	GetNumNames() returns the number of names in the path
		//------------------------------------------------------------------------
		int GetNumNames() const;

		//------------------------------------------------------------------------
		//	GetName() returns a particular name.
		//------------------------------------------------------------------------
		const itString& GetName(int i_Num) const;
		
		//------------------------------------------------------------------------
		//	GetLastName() returns the last name in the list
		//------------------------------------------------------------------------
		const itString& GetLastName() const;

		//------------------------------------------------------------------------
		//	Boolean operators.  Two fsLocators are equal if all of their names
		//	are equal.
		//------------------------------------------------------------------------
		bool operator == (const fsLocator& i_Locator) const;
		bool operator != (const fsLocator& i_Locator) const { return !(*this == i_Locator); }

		//------------------------------------------------------------------------
		//	operator < performs a lexicographical comparison
		//------------------------------------------------------------------------
		bool operator < (const fsLocator& i_Locator) const;

		//------------------------------------------------------------------------
		//	operator <= performs a lexicographical comparison
		//------------------------------------------------------------------------
		bool operator <= (const fsLocator& i_Locator) const;

		//------------------------------------------------------------------------
		//	operator >= performs a lexicographical comparison
		//------------------------------------------------------------------------
		bool operator >= (const fsLocator& i_Locator) const;

		//------------------------------------------------------------------------
		//	compare two strings case INSENSITIVE
		//------------------------------------------------------------------------
		bool Equals(const fsLocator& i_Locator) const;

		//------------------------------------------------------------------------
		//	Assignment
		//------------------------------------------------------------------------
		fsLocator& operator = (const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//	Push() adds a name or locator to the end of the list
		//------------------------------------------------------------------------
		void Push(const char* i_Name);
		void Push(const itString& i_Name);
		void Push(const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//	PopName() removes a name from the end of the list
		//------------------------------------------------------------------------
		void Pop();

		//------------------------------------------------------------------------
		//	FindFirst - find the first occurrence of the name.  Returns -1 if
		//	name is not found.
		//------------------------------------------------------------------------
		int FindFirst(const itString& i_Name) const;

		//------------------------------------------------------------------------
		//	ReplaceName() changes one of the names in the list
		//------------------------------------------------------------------------
		void ReplaceName(const itString& i_Name, int i_Num);

		//------------------------------------------------------------------------
		//	Clear() gets rid of all names and empties the locator
		//------------------------------------------------------------------------
		void Clear();

		//------------------------------------------------------------------------
		//	RemoveAfter() find the first occurence of the string passed in and
		//	remove every token AFTER it (but not the token itself).
		//
		//	returns true if it found the string in the locator
		//------------------------------------------------------------------------
		bool RemoveAfter(const itString& i_String);

		//------------------------------------------------------------------------
		//	RemoveBefore() find the first occurence of the string passed in and
		//	remove every token BEFORE it (but not the token itself).
		//
		//	NOTE: This is useful for removing the first part of a path to get a
		//	relative path.
		//	If this locator is used as a relative path the user must set the 
		//	Index to be the path index in the paths list.
		//
		//	returns true if it found the string in the locator
		//------------------------------------------------------------------------
		bool RemoveBefore(const itString& i_String);
		bool RemoveBefore(int i_Index);

		//------------------------------------------------------------------------
		//	Remove() find the first occurence of the locator passed in and
		//	remove it.
		//
		//	NOTE: This is useful for removing the first part of a path to get a
		//	relative path from the App Directory.
		//	If this locator is used as a relative path the user must set the 
		//	Index to be the path index in the paths list.
		//
		//	returns true if it found the string in the locator
		//------------------------------------------------------------------------
		bool Remove(const fsLocator& i_SubDir);

		//------------------------------------------------------------------------
		//	Remove() find the first occurence of the itString passed in and
		//	remove it.
		//
		//	returns true if it found the string in the locator
		//------------------------------------------------------------------------
		bool Remove(const itString& i_DirName);

	private:

		std::vector<itString> m_Names;
		int m_Index;
};


//------------------------------------------------------------------------
//	HasIndex will return true if the locator is considered to be relative
//	to a stored directory.
//------------------------------------------------------------------------
inline bool fsLocator::HasIndex() const
{
	return m_Index >= 0;
}

//------------------------------------------------------------------------
//	GetIndex returns an index of a directory to be used as the base for
//	the locator (usually gfPath).
//------------------------------------------------------------------------
inline int fsLocator::GetIndex() const
{
	return m_Index;
}
