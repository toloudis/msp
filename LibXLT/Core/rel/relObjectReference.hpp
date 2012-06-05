/*****************************************************************************\
**	relObjectReference.hpp
**
**		Provides a safe way to refer to an object for undo operations.
**	Derivations of this class should provide a way to access an object
**	by a reference that is consistent through deletion and restoration of
**	the object itself.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef REL_OBJECTREFERENCE_HPP
#error relObjectReference.hpp multiply included
#endif
#define REL_OBJECTREFERENCE_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 


//============================================================================
//	Forward References
//============================================================================
class relObject;


//============================================================================
// A prtyReference is a logical reference to a property that may point to
//	different actual prtyProperty classes at different times. This may happen
//	during undo and redo when an object is deleted and restored.
//============================================================================
class relObjectReference
{
	public:
		//--------------------------------------------------------------------
		// virtual destructor
		//--------------------------------------------------------------------
		virtual ~relObjectReference() {}

		//--------------------------------------------------------------------
		//	GetObject - return a pointer to an object that is usable
		//	for a short period of time. 
		//--------------------------------------------------------------------
		virtual relObject* GetObject() const = 0;
};
