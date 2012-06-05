/*****************************************************************************
**  PivotFuncs.cpp
**
**   Namespace for pivot-point related functions, used to 
**	set pivot points of transforms to (0,0,0) since file
**	format doesn't handle rotational pivot points.  
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <PivotFuncs.hpp>
#include <SceneFuncs.hpp>

#include <maya/MFnMesh.h>
#include <maya/MFnTransform.h>
#include <maya/MMatrix.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>

#include <vector>

namespace
{

void
apply_translation(MFnMesh &mesh, const MVector &i_Translate)
{
	cout << "Applying translate to mesh. " << i_Translate << endl;

	MPointArray verts;
	mesh.getPoints(verts);
	MPoint pt;
	for (int i=0; i<verts.length(); i++)
	{
		pt = verts[i];
		pt += i_Translate;
		verts.set(pt, i);
	}
	mesh.setPoints(verts);
}

void
apply_translation(MFnTransform &transform, const MVector &i_Translate,
				 bool bApplySelf = true)
{
	cout << "Applying translate to " << transform.name() << " " << i_Translate << endl;

	// test how pivot is changing in world space
	MPoint pivot = transform.rotatePivot(MSpace::kWorld);
	cout << "Current world pivot: " << transform.name() << " " << pivot << endl;
	pivot = transform.rotatePivot(MSpace::kObject);
	cout << "Current local pivot: " << transform.name() << " " << pivot << endl;


	MVector new_trans = i_Translate;
	if (bApplySelf)
	{
		MMatrix matx = transform.transformation().asMatrixInverse();
		new_trans = i_Translate * matx;

		MPoint pivot = transform.rotatePivot(MSpace::kObject);
		pivot += new_trans;
		transform.setRotatePivot(pivot, MSpace::kObject, true);
		transform.setScalePivot(pivot, MSpace::kObject, true);
	}

	int numChildren = transform.childCount();
	for (int i = 0; i < numChildren; i++) 
	{
		MStatus status;
		MObject obj = transform.child(i);
		MFnTransform trans_child(obj, &status);
		if (status)
		{
			apply_translation(trans_child, new_trans);
		}
		else
		{			
			MFnMesh mesh(obj, &status);
			if (status == MS::kSuccess) 
			{
				apply_translation(mesh, new_trans);
			}
		}
	}
}

}	// end of namespace

//========================================================================
//	RemoveXformPivots - alter transformations in graph to remove
//		rotational pivot points.  This will compile the transformations
//		into the vertices.
//========================================================================
void PivotFuncs::RemoveXformPivots(MFnTransform &transform)
{
	MPoint pivot = transform.rotatePivot(MSpace::kObject);

	cout << "Remove pivots, xform: " << transform.name() << endl;
	cout << "         pivot:  " << pivot[0] << "," << pivot[1] << "," << pivot[2] << endl;

	if (pivot != MPoint(0,0,0))
	{
		// Push translation into meshes
		apply_translation(transform, pivot * -1, false);

		// Apply translation to transform in order 
		// to get center of transform over pivot
		transform.translateBy(pivot, MSpace::kObject);

		// Set xform's pivot to 0,0,0 with balancing on
		// so that Maya will adjust for us.
		transform.setRotatePivot(MPoint(0,0,0), MSpace::kObject, true);
		transform.setScalePivot(MPoint(0,0,0), MSpace::kObject, true);
		// without balancing:
		//transform.setRotatePivot(MPoint(0,0,0), MSpace::kObject, false);
		//transform.setScalePivot(MPoint(0,0,0), MSpace::kObject, false);
	}

	// Traverse Childern 
	//
	int numChildren = transform.childCount();
	cout << "number of children: " << numChildren << endl;

	for (int i = 0; i < numChildren; i++) 
	{
		MStatus status;
		MObject obj = transform.child(i);
		MFnTransform trans_child(obj, &status);
		if (status)
		{
			PivotFuncs::RemoveXformPivots(trans_child);
		}
	}
}

