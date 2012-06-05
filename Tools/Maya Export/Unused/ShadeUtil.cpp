/****************************************************************************\
**  ShadeUtil.cpp
**
**      ShadeUtil contains functions for pre-lighting vertices
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include "ShadeUtil.hpp"

#include <maya/MFnMesh.h>
#include <maya/MIntArray.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>
#include <maya/MStatus.h>

#include "Core/app/appTime.hpp"
#include "Core/geo/geoKDTree.hpp"
#include "Core/geo/geoPickedPoint.hpp"
#include "Core/ma/maAxisBox.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"


//============================================================================
//============================================================================
namespace ShadeUtil
{

//============================================================================
//============================================================================
namespace
{
	//========================================================================
	// Compute set of rays to use for all vertices
	//========================================================================
	void get_rays(std::vector<maPoint3d> &o_Rays)
	{
		const int c_NumRays = 255; //32;
		o_Rays.resize(c_NumRays);
		for (int r=0; r<c_NumRays; r++)
		{
			maVector3d vec(maFunctions::FloatRand(-1, 1),
							maFunctions::FloatRand(-1, 1),
							maFunctions::FloatRand(-1, 1));
			vec.Normalize();
			o_Rays[r] = vec;
		}
	}

	//========================================================================
	//========================================================================
	MString get_time_string(float i_Seconds)
	{
		int minutes = int(i_Seconds) / 60;
		i_Seconds -= (minutes * 60.0f);

		int hours = minutes / 60;
		minutes -= (hours * 60);

		MString val;
		if (hours > 0)
		{
			val += hours;
			val += " hours ";
		}
		if (minutes > 0)
		{
			val += minutes;
			val += " minutes ";
		}

		val += i_Seconds;
		val += " seconds ";

		return val;
	}

	//========================================================================
	//  Fill kdTree with polygons from this i_Mesh
	//========================================================================
	void construct_kdtree(MFnMesh &i_Mesh, geoKDTree &o_Tree)
	{
		MStatus status;

		int nVerts = i_Mesh.numVertices(&status);
		MPointArray  vertices;
  		i_Mesh.getPoints(  vertices );

		//cout << "Num verts " << nVerts << endl;
		std::vector<maPoint3d> verts(nVerts);
		for (int i=0; i<nVerts; i++)
		{
			//cout << "Vert " << i << ": " << vertices[i][0] << " " 
			//	<< vertices[i][1] << " " << vertices[i][2] << endl;
			verts[i].Set(vertices[i][0], vertices[i][1], vertices[i][2]);
		}

		std::vector<envType::UInt32> indices;
		int nPolys = i_Mesh.numPolygons(&status);
		for (int pi=0; pi<nPolys; pi++)
		{
			MIntArray  vertList;
			i_Mesh.getPolygonVertices ( pi,  vertList ); 
			//cout << "Poly " << i << " ";
			for (int vi=2; vi<vertList.length(); vi++)
			{
				//cout << vertList[vi] << " ";
				indices.push_back((envType::UInt32) vertList[0]);
				indices.push_back((envType::UInt32) vertList[vi-1]);
				indices.push_back((envType::UInt32) vertList[vi]);
			}

			//cout << endl;
		}

		o_Tree.AddTriangles(&verts[0], nVerts, &indices[0], indices.size());

		// Set up kdTree based on size of fragment
		maAxisBox box;
		box.Union(&verts[0], nVerts);
		float diag = box.GetRadius() / 16.0f;
		o_Tree.SetMinBox(diag, diag, diag);
		o_Tree.Create(); 
	}

}	// end of namespace

	//========================================================================
	//  Color vertices based on ambient occlusion term
	//========================================================================
	void ComputeAmbientOcclusion(MFnMesh &i_Mesh)
	{
		cout << "Shading ambient: " << endl;

		geoKDTree kdTree;
		construct_kdtree(i_Mesh, kdTree);

		cout << "Constructed kdTree. " << endl;

		std::vector<maPoint3d> rays;
		get_rays(rays);

		MStatus status;

		int nPolys = i_Mesh.numPolygons(&status);
		int nVerts = i_Mesh.numVertices(&status);
		int nNormals = i_Mesh.numNormals(&status);

		MPointArray  vertices;
  		i_Mesh.getPoints(  vertices );
		MFloatVectorArray normals;
		i_Mesh.getNormals( normals );

		int perc = nPolys / 10;
		int per_cnt = 1;
		
		float begin_time = appTime::GetTime();

		MColorArray colors;
		MIntArray faceList;
		MIntArray vertexList;
		for (int pi=0; pi<nPolys; pi++)
		{
			if (pi >= per_cnt * perc)
			{
				per_cnt++;
				char buff[128];
				sprintf(buff, "%d out of %d polygons", pi, nPolys);
				MayaUtil::PrintStatus( buff );
				cout << buff << endl;

				float cur_time = appTime::GetTime();
				float est_time = (cur_time - begin_time) * nPolys / float(pi);
				cout << "Est time: " << get_time_string(est_time) << endl;
			}	

			MIntArray  vertList, normalList;
			i_Mesh.getPolygonVertices ( pi,  vertList ); 
			i_Mesh.getFaceNormalIds ( pi,  normalList );
			
			for (int vi=0; vi<vertList.length(); vi++)
			{
				MPoint vert = vertices[ vertList[vi] ];
				maPoint3d point(vert[0], vert[1],vert[2]);
				MVector norm = normals[ normalList[vi] ];
				maVector3d normal(norm[0], norm[1],norm[2]);

				//i_Mesh.getFaceVertexNormal( pi, vertList[vi], normal );
				//cout << " normal: " << norm[0] << " " << norm[1] << " " << norm[2] << endl;

				// Cast multiple rays to average occlusion term
				int num_rays = rays.size();
				int count = 0;
				for (int r=0; r<num_rays; r++)
				{
					maVector3d vec = rays[r];

					// We only want rays that are in the hemisphere pointing
					// away from the normal
					if (normal * vec < 0)
						vec *= -1.0f;

					// Construct a ray to test occlusion
					geoPickedPoint pick_pt;
					const float c_Offset = 0.01f; // slightly offset ray start along normal
					maPoint3d ray_start = point + normal * c_Offset;
					const float c_Length = 9999.9f; // long ray, will get clipped by kdTree's bbox
					maPoint3d ray_end = point + vec * c_Length;
					//maPoint3d ray_end = point + normal * c_Length;
					if (!kdTree.ComputeRayIntersection(	ray_start, ray_end, pick_pt))
						count++;
				}

				// convert occlusion term to number between 0-1
				float occl = count / float(num_rays); 

				//float occl = (float)norm[1] * 0.5f + 0.5f;
				//cout << " Occl: " << pi << " " << vi << " " << occl << endl;
			//	status = i_Mesh.setFaceVertexColor( MColor(occl,occl,occl), pi, vertList[vi] );
				//status = i_Mesh.setFaceVertexColor( MColor(0.0f, 1.0f, 0.0f), pi, vertList[vi] );
			//	if (!status)
			//		cout << "Could not set color" << endl;

				colors.append( MColor(occl,occl,occl) );
				faceList.append( pi );
				vertexList.append( vertList[vi] );
			}
		}	
		
		status = i_Mesh.setFaceVertexColors( colors,  faceList,  vertexList );
		if (!status)
			cout << "Could not set color" << endl;

		cout << "Done." << endl;
	}

}

