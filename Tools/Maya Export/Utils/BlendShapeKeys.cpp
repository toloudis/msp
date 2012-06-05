/*****************************************************************************
**  BlendShapeKeys.cpp
**
**   Namespace for baking animations from blend shape weights   
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <BlendShapeKeys.hpp>
#include <CharacterFuncs.hpp>

#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
//#include <maya/MFnSubd.h>
#include <maya/MPlug.h>

namespace BlendShapeKeys
{

	namespace
	{
		bool	bWriteInputDetails = false;


		//========================================================================
		// Gathers list of blend shapes for the list of geometry filters
		//========================================================================
		void gather_blend_shapes(MObjectArray &inputs,
							     std::list<BlendKeys> &o_Keys)
		{
			MStatus status;
			if (bWriteInputDetails)
				cout << "Num inputs: " << inputs.length() << endl;
			for (int i=0; i<inputs.length(); i++)
			{
				MObject obj = inputs[i];
				if (bWriteInputDetails)
					cout << "input geom is of type " << obj.apiTypeStr() << endl;

				MFnBlendShapeDeformer blendShape(obj, &status);
				if (status == MS::kSuccess) 
				{
					if (bWriteInputDetails)
						cout << "Found blend shape: " << blendShape.name() << endl;

					int num_weights = blendShape.numWeights();
					if (num_weights == 0)
					{
						if (bWriteInputDetails)
							cout << "BlendShape " << blendShape.name() << " has zero weights." << endl;
						continue;
					}
					MIntArray indexList;
					blendShape.weightIndexList( indexList );
					if (indexList.length() != num_weights)
					{
						if (bWriteInputDetails)
							cout << "BlendShape " << blendShape.name() << " could not match up weight indices " << indexList.length() << " vs " << num_weights << endl;
						continue;
					}

					// weight is an array plug, one per target object
					MStatus weight_plug_status;
					MPlug weight_plug = blendShape.findPlug("weight", &weight_plug_status);
					
					for (int i=0; i<num_weights; i++)
					{
						int weight_index = indexList[i];
						//cout << "Weight index " << i << " is " << weight_index << endl;

						BlendKeys keys;
						//keys.m_Blend.setObject( blendShape.object() );
						keys.m_BlendShapeObject = blendShape.object();
						keys.m_WeightIndex = weight_index;
						keys.m_BlendKeys.m_BlendName = MayaUtil::PrepareName(blendShape.name()).asUTF8();

						// Try to find aliased name for this weight
						MString aliasName;
						if (weight_plug_status == MS::kSuccess) 
						{	
							//cout << "Num elements: " << plug.numElements() << endl;
							if (weight_plug.numElements() > i)
							{
								MPlug elem = weight_plug.elementByPhysicalIndex(i);
								if (blendShape.getPlugsAlias(elem, aliasName, &status))
								{
									if (bWriteInputDetails)
										cout << "Have aliased name " << aliasName << endl;
									keys.m_BlendKeys.m_AliasName = MayaUtil::PrepareName(aliasName).asUTF8();
								}
							}
						}

						o_Keys.push_back(keys);
					}
				}
			}
		}


	}	// end of namespace

	//========================================================================
	// Gathers list of blend shapes for this subdiv
	//========================================================================
	//void GatherBlendShapes(MFnSubd &i_Subdiv,
	//					   std::list<BlendKeys> &o_Keys)
	//{
	//	MStatus status;

	//	// Gather up blend shape nodes
	//	MObjectArray inputs;
	//	CharacterFuncs::GetGeometryFiltersFromInputs(i_Subdiv, inputs);
	//		
	//	gather_blend_shapes(inputs, o_Keys);
	//}

	//========================================================================
	// Gathers list of blend shapes for this mesh
	//========================================================================
	void GatherBlendShapes(MFnMesh &i_Mesh,
						   std::list<BlendKeys> &o_Keys)
	{
		MStatus status;

		// Gather up blend shape nodes
		MObjectArray inputs;
		CharacterFuncs::GetGeometryFiltersFromInputs(i_Mesh, inputs);
			
		gather_blend_shapes(inputs, o_Keys);
	}

	//========================================================================
	// Gets current state of blend shape weights and puts the data into the
	//	list data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(std::list<BlendKeys> &io_Keys, 
					float i_CurrentTime)
	{
		MStatus status;
		std::list<BlendKeys>::iterator it;
		for (it = io_Keys.begin(); it != io_Keys.end(); ++it)
		{
			MFnBlendShapeDeformer blendShape(it->m_BlendShapeObject, &status);	
			if (status == MS::kSuccess) 
			{	
				AnimKeys::GatherKeys( it->m_BlendKeys, i_CurrentTime, blendShape, it->m_WeightIndex );	
			}
			else
			{
				cout << "Error getting blend shape node from MObject handle." << endl;
			}
		}
	}

	//========================================================================
	//	WriteAnimation - write animation based on the given keys
	//		that were gathered earlier.
	//========================================================================
	void WriteAnimation(chWriter &o_Writer,
						const std::list<BlendKeys> &i_Keys,
						float i_TimeOffset)
	{
		std::list<BlendKeys>::const_iterator it;
		for (it = i_Keys.begin(); it != i_Keys.end(); ++it)
		{
			AnimKeys::WriteAnimation(o_Writer, it->m_BlendKeys, i_TimeOffset);
		}
	}

	
}