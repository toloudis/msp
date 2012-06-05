/****************************************************************************\
**	relRelationship.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Core/rel/relRelationship.hpp"


//============================================================================
//============================================================================
namespace
{
}

//--------------------------------------------------------------------
// constructor takes reference to object owning the relationship
//--------------------------------------------------------------------
relRelationship::relRelationship(relHandle i_Handle,
								 relObject& i_ContainerObject)
:	m_Handle(i_Handle),
	m_ContainerObject(i_ContainerObject)
{

}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
relRelationship::~relRelationship()
{

}
