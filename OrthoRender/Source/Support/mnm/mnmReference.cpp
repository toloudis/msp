/*****************************************************************************
**	mnmReference.cpp
**
**	Default reference implementation, returns matrix based on object's
**	transformation.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/mnm/mnmReference.hpp"

#include "Support/mnm/mnmObject.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmReference::mnmReference(const mnmObject &i_Object)
: m_Object(i_Object)
{

}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmReference::~mnmReference()
{

}

//--------------------------------------------------------------------
// GetMatrix for this named refence
//--------------------------------------------------------------------
//virtual
maMatrix4x4 mnmReference::GetMatrix() const
{
	// need to do full transformation eventually
	maMatrix4x4 matx;
	matx.MakeTranslate(m_Object.GetPosition());
	return matx;
}

