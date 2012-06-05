/*****************************************************************************
**	scLODObject.hpp
**
**		scLODObject is an abstract base for Level of Detail 
**	scene object.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef SC_LODOBJECT_HPP
#error scLODObject.hpp multiply included
#endif
#define SC_LODOBJECT_HPP

#ifndef SC_OBJECT_HPP
#include "Graphics/sc/scObject.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class scLODObject : public scObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		scLODObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~scLODObject();

		//--------------------------------------------------------------------
		//	Add the object at the specified index.  if the index is -1 then
		//	the object will be appended on the end.
		//--------------------------------------------------------------------
		virtual void AddObject( scObject* i_pObject, float i_fDistance, bool i_bDefault, int i_Index = -1 );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void RemoveObject( scObject* i_pObject );

		//--------------------------------------------------------------------
		//	replace the object at the passed in index.  this function will
		//	DELETE the old object.
		//--------------------------------------------------------------------
		virtual void ReplaceObject( int i_Index, scObject* i_pObject, float i_fDistance, bool i_bDefault );

		//--------------------------------------------------------------------
		//	GetBase returns the scene node representing the root of the
		//	object.  This is also the node which affects visibility of the
		//	object, and positioning of the object.
		//--------------------------------------------------------------------
		virtual inline const g3dSceneNode* GetBase() const;
		virtual inline g3dSceneNode* GetBase();

		//--------------------------------------------------------------------
		//	check the origin point against the object's point to see if 
		//	it should change it's LOD.
		//--------------------------------------------------------------------
		void UpdateLOD( const maPoint3d& i_OriginPoint );

	private:
		struct level_of_detail_object
		{
			scObject* m_pObject;
			g3dSceneNode*		m_pBase;
			bool				m_bDefault;
			float				m_fDistance;
		};

		int	m_CurrentObjectIndex;
		std::vector<level_of_detail_object>	m_Objects;
};

//--------------------------------------------------------------------
//	GetBase returns the scene node representing the root of the
//	object.  This is also the node which affects visibility of the
//	object, and positioning of the object.
//--------------------------------------------------------------------
inline const g3dSceneNode* scLODObject::GetBase() const
{
	return scObject::GetBase();
}

inline g3dSceneNode* scLODObject::GetBase()
{
	return scObject::GetBase();
}
