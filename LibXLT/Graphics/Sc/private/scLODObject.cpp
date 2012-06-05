/*****************************************************************************
**	scLODObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scLODObject.hpp"

#include "Graphics/g3d/g3dSceneNode.hpp"


//--------------------------------------------------------------------
//	The object will initially
//	be placed at the origin with unit scale and no rotation.
//--------------------------------------------------------------------
scLODObject::scLODObject()
:	m_CurrentObjectIndex(0)
{
	m_Objects.resize(0);

	scObject::GetBase()->SetRenderable( true );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
scLODObject::~scLODObject()
{
	//	from ~scOjbect()
	//if( m_pBase->GetParent() )
	//{
	//	m_pBase->GetParent()->RemoveChild(m_pBase);
	//}
	//delete m_pBase;
	//envSTLHelpers::DeleteContainer(m_ControlAnims);
}

//--------------------------------------------------------------------
//	Add the object at the specified index.  if the index is -1 then
//	the object will be appended on the end.
//--------------------------------------------------------------------
void scLODObject::AddObject( scObject* i_pObject, float i_fDistance, bool i_bDefault, int i_Index )
{
	DBG_ASSERT( i_pObject != 0 , "Cannot add a NULL object" );
	if (i_pObject == 0)
		return;

	//	add the object to the list
	//
	int index = i_Index;
	if ( index == -1 )
	{
		index = m_Objects.size();
		m_Objects.resize( index + 1 );
	}
	else
	{
		DBG_ASSERT( index < m_Objects.size(), "Index out of range" );
		if (index >= m_Objects.size())
			return;
	}

	m_Objects[index].m_pObject = i_pObject;

	//	add the object to the base scene node
	//
	g3dSceneNode* pNode = i_pObject->GetBase();

	m_Objects[index].m_pBase		= pNode;
	m_Objects[index].m_bDefault		= i_bDefault;
	m_Objects[index].m_fDistance	= i_fDistance;

	scObject::GetBase()->AddChild( pNode );

	pNode->SetRenderable( false );

	if ( m_Objects.size() == 1 )			// TODO: temporary?
		pNode->SetRenderable( true );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void scLODObject::RemoveObject( scObject* i_pObject )
{
	DBG_ASSERT( i_pObject != 0 , "Cannot remove a NULL object" );
	if (i_pObject == 0)
		return;

	int index = 0;
	std::vector<level_of_detail_object>::iterator it = m_Objects.begin();
	for (; it != m_Objects.end(); ++it, ++index)
	{
		if ( m_Objects[index].m_pObject == i_pObject )
		{
			//	remove the object to the base scene node
			//
			scObject::GetBase()->RemoveChild( m_Objects[index].m_pBase );

			delete m_Objects[index].m_pObject;
			m_Objects[index].m_pObject = 0;
			break;
		}
	}
}

//--------------------------------------------------------------------
//	replace the object at the passed in index.  this function will
//	DELETE the old object.
//--------------------------------------------------------------------
//virtual 
void scLODObject::ReplaceObject( int i_Index, scObject* i_pObject, float i_fDistance, bool i_bDefault )
{
	DBG_ASSERT( i_Index < m_Objects.size(), "Index out of range" );
	if (i_Index >= m_Objects.size())
		return;

	if ( m_Objects[i_Index].m_pObject != 0 )
	{
		RemoveObject( m_Objects[i_Index].m_pObject );
		delete m_Objects[i_Index].m_pObject;
		m_Objects[i_Index].m_pObject = 0;
	}

	AddObject( i_pObject, i_fDistance, i_bDefault, i_Index );
}

//--------------------------------------------------------------------
//	check the origin point against the object's point to see if 
//	it should change it's LOD.
//--------------------------------------------------------------------
void scLODObject::UpdateLOD( const maPoint3d& i_OriginPoint )
{
	DBG_ASSERT( m_Objects[m_CurrentObjectIndex].m_pObject != 0, "Invalid object" );
	if (m_Objects[m_CurrentObjectIndex].m_pObject == 0)
		return;

	float dist;
	dist = (m_Objects[m_CurrentObjectIndex].m_pObject->GetPosition() - i_OriginPoint).Length();

	//DBG_LOG3( "Cam (%6.3f, %6.3f, $6.3f)", i_OriginPoint.GetX(), i_OriginPoint.GetY(), i_OriginPoint.GetZ() );
	//DBG_LOG3( "Obj (%6.3f, %6.3f, $6.3f)", m_Objects[m_CurrentObjectIndex].m_pObject->GetPosition().GetX(), m_Objects[m_CurrentObjectIndex].m_pObject->GetPosition().GetY(), m_Objects[m_CurrentObjectIndex].m_pObject->GetPosition().GetZ() );
	//DBG_LOG1( "     Distance = %6.3f", dist );

	// based on the distance choose the correct model
	//
	int i, newindex;
	for ( i = m_Objects.size() - 1 ; i >= 0 ; --i )
	{
		if ( m_Objects[i].m_fDistance <= dist )
		{
			newindex = i;
			break;
		}
	}

	if ( newindex != m_CurrentObjectIndex )
	{
		DBG_LOG( "static LOD change from " << m_CurrentObjectIndex << " to " << newindex );

		m_Objects[m_CurrentObjectIndex].m_pBase->SetRenderable( false );
		m_Objects[newindex].m_pBase->SetRenderable( true );

		m_CurrentObjectIndex = newindex;
	}
}

