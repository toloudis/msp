/****************************************************************************\
**	mnmTestPickInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "mnmTestPickInterest.hpp"

#include "mnmProp.hpp"

//	library
#include "g3dFragment.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dScene.hpp"
#include "g3dSceneNode.hpp"
#include "geoPickRay.hpp"
#include "matMaterial.hpp"
#include "scObject.hpp"

//	tool
#include "api3dObject.hpp"
#include "api3dShape.hpp"
#include "api3dScene.hpp"


namespace
{
	class mnmTestObject : public mnmProp
	{
		public:
			mnmTestObject::mnmTestObject()
			{
				Set3dObject( api3dShape::CreateSphere(maFloatRGBA(1,0,0,1), 5.0f, 8, 8) );
				api3dScene::AddObject( Get3dObject() );
			};

			virtual mnmTestObject::~mnmTestObject()
			{
				api3dScene::RemoveObject( Get3dObject() );
				delete Get3dObject();
			};
	};

	mnmTestObject* l_pObject = 0;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmTestPickInterest::mnmTestPickInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
mnmTestPickInterest::~mnmTestPickInterest()
{
	delete l_pObject;
}

//--------------------------------------------------------------------
//	OccluderPick() - search through objects that are occluders and
//	modify the tVal so the ray is shortened to only the visible length
//--------------------------------------------------------------------
//virtual
bool mnmTestPickInterest::OccluderPick( geoPickRay& i_Ray, float &o_tVal )
{
	return false;
}

//--------------------------------------------------------------------
//	ObjectPick() - after the OccluderPick() is called (optional) the
//	shortened ray can then be used for actual object picking.  The
//	objects in the list are sorted from the closest to the farthest.
//	So getting the first object in the list will be the nearest.
//--------------------------------------------------------------------
//virtual
bool mnmTestPickInterest::ObjectPick( geoPickRay& i_Ray, pick3dPickList& io_PickList )
{
	if ( l_pObject == 0 )
	{
		l_pObject = new mnmTestObject();
	}

	float tval;
	//if ( l_pObject->RayPick(i_Ray.GetRayStart(), i_Ray.GetRayEnd(), tval))
	{
		io_PickList.AddItem( l_pObject, 1.0f );
		return true;
	}

	return false;
}
