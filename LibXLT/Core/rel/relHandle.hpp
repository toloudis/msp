/****************************************************************************\
**	relHandle.hpp
**
**		relHandle defines a way to identify relationships within an object.
**	It needs to be lightweight because it will be used in many objects.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef REL_HANDLE_HPP
#error relHandle.hpp multiply included
#endif
#define REL_HANDLE_HPP


//============================================================================
// Gonna start with the handle being just a pointer. Don't want a std::string
// here because it would be too much memory repeating the same 
// string over and over. By using a char* instead of an int, then we can
// give better debugging messages. This could be implemented as a byte that
// indexes into a dictionary of strings if we really want to make it 
// smaller later.
//============================================================================
typedef const char* relHandle;
