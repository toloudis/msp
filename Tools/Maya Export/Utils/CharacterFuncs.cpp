/*****************************************************************************
**  CharacterFuncs.cpp
**
**   Namespace functions related to character output.
**	Characters consist of multiple meshes controlled by a joint
**	hierarchy and feature morph targets for emotions and phonemes.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <CharacterFuncs.hpp>
#include <AnimFuncs.hpp>
#include <SceneFuncs.hpp>
#include <SubdivFuncs.hpp>
#include <MaterialUtil.hpp>
#include <MayaFlagUtil.hpp>
#include <VertexFuncs.hpp>

#include <maya/MFloatVectorArray.h>
#include <maya/MFnSubd.h>
#include <maya/MFnBlendShapeDeformer.h>
#include <maya/MFnComponentListData.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnGeometryFilter.h>
#include <maya/MFnMesh.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnSet.h>
#include <maya/MFnSingleIndexedComponent.h>
//#include <maya/MFnSubd.h>
#include <maya/MObjectArray.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPointArray.h>
#include <maya/MSelectionList.h>
#include <maya/MUint64Array.h>

#include <vector>


#include "Core/ch/chWriter.hpp"
#include "Core/ma/maVector3d.hpp"


//=============================================================================
//	Chunk types
//=============================================================================
const chDefs::Name c_MRPH = chDefs::MakeName('M', 'R', 'P', 'H');
const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
const chDefs::Name c_WNAM = chDefs::MakeName('W', 'N', 'A', 'M');
const chDefs::Name c_GVER = chDefs::MakeName('G', 'V', 'E', 'R');
const chDefs::Name c_NVER = chDefs::MakeName('N', 'V', 'E', 'R');
const chDefs::Name c_MDLT = chDefs::MakeName('M', 'D', 'L', 'T');


namespace
{
	bool	bWriteInputDetails = false;
	bool	bWriteAutoDetectDetails = false;
	bool	l_bAutoDetectDeformation = false;

	//========================================================================
	//========================================================================
	void AddUnique(MObjectArray &objects, MObject obj)
	{
		for (int i=0; i<objects.length(); i++)
			if (objects[i] == obj) return;

		if (bWriteInputDetails)
			cout << "Adding unique object: " << obj.apiTypeStr() << endl;
		objects.append(obj);
	}


	//========================================================================
	// Try to find out about deltas through plugs
	//========================================================================
	void GetMorphDeltas(MFnBlendShapeDeformer blendShape, 
							int weight_index, 
							MPointArray& o_SparseDeltas,
							MIntArray& o_SparseIndices)
	{
		//cout << "GetMorphDeltas" << endl;
		MStatus status;

		// Can't get to a nested plug in a single step, so we have to dig down to it...
		//MPlug test_plug = blendShape.findPlug("inputTarget[0]", &status);
		//if (status == MS::kSuccess) 
		//{
		//	cout << "Test plug inputTarget[0]: " << test_plug.numElements() << " compound? " << test_plug.isCompound() << endl;
		//	cout << "  num kids: " << test_plug.numChildren() << endl;
		//}
		//else cout << "Test plug failed." << endl;

		// Dig down to the component deltas plugs.
		// We want a plug with a name like: 
		// "blendShape.inputTarget[0].inputTargetGroup[weight_index].inputTargetItem[6000].inputPointsTarget"
		MPlug input_plug = blendShape.findPlug("inputTarget", &status);
		if (status == MS::kSuccess) 
		{	
			// inputTarget[0]
			MPlug target0 = MayaUtil::AccessPlugIndex(input_plug, 0, status);
			if (status == MS::kSuccess) 
			{	
				// inputTargetGroup
				MPlug itg = MayaUtil::AccessPlugIndex(target0, 0, status);
				//MayaUtil::DebugPlug(itg);
				if (status == MS::kSuccess) 
				{	
					// inputTargetGroup[weight_index]
					MPlug itg_weight = MayaUtil::AccessPlugIndex(itg, weight_index, status);
					//MayaUtil::DebugPlug(itg_weight);
					if (status == MS::kSuccess) 
					{	
						// inputTargetItem
						MPlug iti = MayaUtil::AccessPlugIndex(itg_weight, 0, status);
						//MayaUtil::DebugPlug(iti);
						if (status == MS::kSuccess) 
						{	
							// inputTargetItem[6000]
							MPlug iti_item = MayaUtil::AccessPlugIndex(iti, 6000, status);
							//MayaUtil::DebugPlug(iti_item);
							if (status == MS::kSuccess) 
							{	
								// inputPointsTarget - pointsArray
								MPlug ipt = MayaUtil::AccessPlugIndex(iti_item, 1, status);
								//MayaUtil::DebugPlug(ipt);
								
								MFnPointArrayData points( ipt.asMObject(), &status );
								if (status == MS::kSuccess) 
								{
									//cout << "Got points array! size: " << points.length() << endl;

									// inputComponentsTarget - componentList
									MPlug ict = MayaUtil::AccessPlugIndex(iti_item, 2, status);
									MFnComponentListData comps( ict.asMObject(), &status );
									if (status == MS::kSuccess) 
									{
										//cout << "Got components array! size: " << comps.length();
										if (comps.length() > 0)
										{
											//MObject comp = comps[0];
											//cout << " Type of component: " << comp.apiTypeStr() << endl;
											MFnSingleIndexedComponent ind_cmp(comps[0], &status);
											if (status == MS::kSuccess) 
											{
												// Fill in return values
												ind_cmp.getElements(o_SparseIndices);
												//cout << " length of components array: " << o_SparseIndices.length() << endl;
				
												o_SparseDeltas.setLength(points.length());
												points.copyTo(o_SparseDeltas);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

	}				

	//========================================================================
	//========================================================================
	bool GetTargetsFromBlendShape(MFnBlendShapeDeformer blendShape,
		MObjectArray &targets,
		MStringArray &aliasedNames)
	{
		MStatus status;
		int num_weights = blendShape.numWeights();
		if (num_weights == 0)
		{
			cout << "BlendShape " << blendShape.name() << " has zero weights." << endl;
			return false;
		}
		MIntArray indexList;
		blendShape.weightIndexList( indexList );
		if (indexList.length() != num_weights)
		{
			cout << "BlendShape " << blendShape.name() << " could not match up weight indices " << indexList.length() << " vs " << num_weights << endl;
			return false;
		}

		// weight is an array plug, one per target object
		MStatus weight_plug_status;
		MPlug weight_plug = blendShape.findPlug("weight", &weight_plug_status);

		MObjectArray objects;
		blendShape.getBaseObjects(objects);
		if (objects.length() == 0)
		{
			cout << "BlendShape " << blendShape.name() << " has no base objects." << endl;
			return false;
		}
		
		// Just get weights from first base object for now
		for (int i=0; i<num_weights; i++)
		{
			int weight_index = indexList[i];
			if (bWriteInputDetails)
				cout << "Weight index " << i << " is " << weight_index << endl;

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
					}
				}
			}

			MObjectArray shape_targets;
			blendShape.getTargets(objects[0], weight_index, shape_targets);
			if (shape_targets.length() == 0)
			{
				if (bWriteInputDetails)
					cout << "BlendShape weight " << i << " has no target objects." << endl;
			}
			else
			{
				if (bWriteInputDetails)
					cout << "BlendShape weight " << i << " has " << shape_targets.length() << " target objects." << endl;

				// Append these targets into our combined array
				for (int t=0; t<shape_targets.length(); t++)
				{
					targets.append(shape_targets[t]);
					aliasedNames.append(aliasName);
				}
			}
		}

		return (targets.length() > 0);
	}

	//========================================================================
	//========================================================================
	void DebugBlendShape(MFnBlendShapeDeformer blendShape)
	{
		MStatus status;
		cout << "** DebugBlendShape, named " << blendShape.name()<< endl;

		MObjectArray objects;
		blendShape.getBaseObjects(objects);
		cout << "Number of base objects: " << objects.length() << endl;

		cout << "Number of weights: " << blendShape.numWeights() << endl;

		for (int i=0; i<objects.length(); i++)
		{
			MObject baseObj = objects[i];
			cout << "baseObject is of type " << baseObj.apiTypeStr() << endl;

			MFnSubd baseSubd(baseObj, &status);
			if (status == MS::kSuccess) 
			{
				cout << " base is a subdiv, named " << baseSubd.name()<< endl;
			}
			MFnMesh baseMesh(baseObj, &status);
			if (status == MS::kSuccess) 
			{
				cout << " base is a mesh, named " << baseMesh.name()<< endl;
			}

			for (int w=0; w<blendShape.numWeights(); w++)
			{
				MObjectArray targets;
				blendShape.getTargets(baseObj, w, targets);
				cout << "Number of targets: " << targets.length() << endl;

				for (int t=0; t<targets.length(); t++)
				{
					cout << "target is of type " << targets[t].apiTypeStr() << endl;

					MFnSubd targetSubd(targets[t], &status);
					if (status == MS::kSuccess) 
					{
						cout << " target is a subdiv, named " << targetSubd.name()<< endl;
					}
					MFnMesh targetMesh(targets[t], &status);
					if (status == MS::kSuccess) 
					{
						cout << " target is a mesh, named " << targetMesh.name()<< endl;
					}
				}
			}
		}

		MayaUtil::PrintInputs( blendShape );
		MayaUtil::PrintAttributeTypes( blendShape );
		MayaUtil::PrintPlugs( blendShape );
	}

	//========================================================================
	//========================================================================
	bool GetInputNodes(MFnGeometryFilter geomFilter,
							MObjectArray &inputs)
	{
		MStatus status;

		// get compund "input" plug, which has two components,
		// "inputGeometry" and "groupId". We want the first one.
		MPlug inputPlug = geomFilter.findPlug("input", &status);
		if (status == MS::kSuccess) 
		{
			MPlug inputGeometry = inputPlug.child(0); // "inputGeometry" plug

			MPlugArray connections;
			if (inputGeometry.connectedTo(connections, true, true, &status)) 
			{
				if (status == MS::kSuccess && connections.length() > 0) 
				{
					if (bWriteInputDetails)
						cout << "num connections to inputPlug = " << connections.length() << endl;
					for (int c = 0; c < connections.length(); c++) 
					{
						MObject obj = connections[c].node(&status);

						// This is a while loop so that it can go through
						// multiple layers of group parts and subd add topology nodes
						//
						while (!obj.isNull())
						{
							if (obj.apiType() == MFn::kGroupParts)
							{
								if (bWriteInputDetails) cout << "Have group parts" << endl;
								MFnDependencyNode node(obj);
								MObject group_obj = MayaUtil::GetObjectConnectedToPlug(node, "inputGeometry", status);
								if (status == MS::kSuccess)
								{
									obj = group_obj;
								}
								else
								{
									cout << "Can't get input from kGroupParts" << endl;
									break;
								}
							}
							else if (obj.apiType() == MFn::kSubdAddTopology)
							{
								if (bWriteInputDetails) cout << "Have subd add topology." << endl;
								MFnDependencyNode node(obj);
								MObject add_obj = MayaUtil::GetObjectConnectedToPlug(node, "inSubdiv", status);
								if (status == MS::kSuccess)
								{
									obj = add_obj;
								}
								else
								{
									cout << "Can't get input from kSubdAddTopology" << endl;
									break;
								}
							}
							else
							{
								AddUnique(inputs, obj);
								break;
							}
						}
					}
					return true;
				}
				else cout << "connections.length() == 0?" << endl;
			}
			else cout << "Not connected?" << endl;
		}

		return false;
	}

	//========================================================================
	//========================================================================
	void DebugGeometryFilter(MFnGeometryFilter geomFilter)
	{
		MStatus status;
		cout << "** DebugGeometryFilter, named " << geomFilter.name()<< endl;

		MObjectArray inputs;
		GetInputNodes(geomFilter, inputs);
		
		for (int i=0; i<inputs.length(); i++)
		{
			MObject baseObj = inputs[i];
			cout << "input geom is of type " << baseObj.apiTypeStr() << endl;

			MFnDependencyNode node(baseObj);
			cout << "named: " << node.name() << endl;
		}

/*		MObject obj = MayaUtil::GetObjectConnectedToPlug( geomFilter, "input[0].inputGeometry", status); 
		if (status == MS::kSuccess) 
		{
			cout << "input geom is of type " << obj.apiTypeStr() << endl;

			MFnDependencyNode node(obj);
			cout << "named: " << node.name() << endl;

			//MFnGeometryFilter filter(obj, &status);
			//if (status == MS::kSuccess) 
			//{
			//	DebugGeometryFilter(filter);
			//	MayaUtil::PrintInputs(filter);
			//}
		}
*/
/*		MObjectArray objects;
		geomFilter.getInputGeometry(objects);
		cout << "Number of input geometries: " << objects.length() << endl;

		for (int i=0; i<objects.length(); i++)
		{
			MObject baseObj = objects[i];
			cout << "input geom is of type " << baseObj.apiTypeStr() << endl;

			MFnDependencyNode node(baseObj);
			cout << "named: " << node.name() << endl;
		}

		MObject set_obj = geomFilter.deformerSet();
		MFnSet set(set_obj, &status);
		if (status == MS::kSuccess) 
		{
			MSelectionList members;
			bool flatten = true;
			set.getMembers( members, flatten );
			cout << "  num members " << members.length()<< endl;
			
			MObject obj;
			for (int i=0; i<members.length(); i++)
			{
				members.getDependNode( i, obj );
				cout << "  member is type: " << obj.apiTypeStr() << endl;

				MFnDependencyNode node(obj);
				cout << "named: " << node.name() << endl;
			}
		}
*/
	}

	//========================================================================
	// forward declaration
	//========================================================================
	void GetGeometryFiltersFromInputs(MFnGeometryFilter &filter, MObjectArray &objects);


	//========================================================================
	// Look for filters through the PolyBlindData nodes
	//========================================================================
	void SeekInputsToModifiers(MObject &polyModifier, MObjectArray &objects)
	{
		MStatus status;
		MFnDependencyNode node(polyModifier, &status);
		if (status == MS::kSuccess) 
		{
			if (bWriteInputDetails)
				cout << "Recursing on poly modifier node named: " << node.name() << endl;

			MObject obj = MayaUtil::GetObjectConnectedToPlug( node, "inMesh", status); 
			if (status == MS::kSuccess) 
			{
				MFnGeometryFilter filter(obj, &status);
				if (status == MS::kSuccess) 
				{
					AddUnique(objects, obj);
					GetGeometryFiltersFromInputs(filter, objects);
				}
				else if (obj.hasFn(MFn::kPolyBlindData))
				{
					SeekInputsToModifiers(obj, objects);
				}
			}
		}
	}

	//========================================================================
	//========================================================================
	void GetGeometryFiltersFromInputs(MFnGeometryFilter &filter, MObjectArray &objects)
	{
		MStatus status;
		MObjectArray inputs;
		GetInputNodes(filter, inputs);
		
		for (int i=0; i<inputs.length(); i++)
		{
			MObject obj = inputs[i];

			// Add all types of nodes
			AddUnique(objects, obj);

			// recurse only on geometry filters
			MFnGeometryFilter filter(obj, &status);
			if (status == MS::kSuccess) 
			{
				GetGeometryFiltersFromInputs(filter, objects);
			}	
			else if (obj.hasFn(MFn::kPolyBlindData))
			{
				SeekInputsToModifiers(obj, objects);
			}
		}
	}


	//========================================================================
	// Get morph target info from mesh of target shape
	//========================================================================
	bool GetMorphTargetInfo(MFnMesh &morph, CharacterFuncs::MorphTargetInfo &o_Info)
	{
		morph.getPoints(o_Info.m_PositionVecs);
		o_Info.m_bVecsAreDeltas = false;
		return true;
	}

	//========================================================================
	// MESH version
	//========================================================================
	void WriteMorphTarget(MFnMesh &morph, const char* i_NodeName, const char* i_WeightName, chWriter &o_Writer)
	{
		int nVerts = morph.numVertices();
		
		cout << "Have mesh as blend shape target, " << morph.name() << " aliased " << i_WeightName << " nVerts = " << nVerts << endl;

		o_Writer.WriteChunkHeader(c_MRPH, 0, true);

		// Name of blend shape node
		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		o_Writer.Write(MayaUtil::PrepareName(i_NodeName).asUTF8()); 
		o_Writer.FinishChunk();

		// Aliased name for weight (this one is visible to the artist in Maya)
		o_Writer.WriteChunkHeader(c_WNAM, 0, false);
		o_Writer.Write(MayaUtil::PrepareName(i_WeightName).asUTF8()); 
		o_Writer.FinishChunk();

		// Positions
		o_Writer.WriteChunkHeader(c_GVER, 0, false);
		o_Writer.Write(envType::Int16(nVerts));
		for(int v = 0; v < nVerts; v++) 
		{
			MPoint point;
			morph.getPoint(v, point);
			o_Writer.Write(float(point[0]));
			o_Writer.Write(float(point[1]));
			o_Writer.Write(float(point[2]));
		}
		o_Writer.FinishChunk();
		
		// Normals
		int nNormals = morph.numNormals();
		o_Writer.WriteChunkHeader(c_NVER, 0, false);
		o_Writer.Write(envType::Int16(nNormals));
		MFloatVectorArray normalArray;
		morph.getNormals(normalArray);
		for (int n = 0; n < nNormals; n++) 
		{
			MFloatVector normal = normalArray[n];
			o_Writer.Write(float(normal[0]));
			o_Writer.Write(float(normal[1]));
			o_Writer.Write(float(normal[2]));
		}
		o_Writer.FinishChunk();

		o_Writer.FinishChunk();

	}

	//========================================================================
	// SUBDIV version
	//========================================================================
	//void WriteMorphTarget(MFnSubd &morph, const char* i_NodeName, const char* i_WeightName, chWriter &o_Writer)
	//{
	//	MPointArray positions;
	//	morph.vertexBaseMeshGet( positions );
	//	int nVerts = positions.length();

	//	cout << "Have subdiv as blend shape target, " << morph.name() << " aliased " << i_WeightName << " nVerts = " << nVerts << endl;

	//	o_Writer.WriteChunkHeader(c_MRPH, 0, true);

	//	// Name of blend shape node
	//	o_Writer.WriteChunkHeader(c_NNAM, 0, false);
	//	o_Writer.Write(MayaUtil::PrepareName(i_NodeName).asUTF8()); 
	//	o_Writer.FinishChunk();

	//	// Aliased name for weight (this one is visible to the artist in Maya)
	//	o_Writer.WriteChunkHeader(c_WNAM, 0, false);
	//	o_Writer.Write(MayaUtil::PrepareName(i_WeightName).asUTF8()); 
	//	o_Writer.FinishChunk();

	//	// Positions
	//	o_Writer.WriteChunkHeader(c_GVER, 0, false);
	//	o_Writer.Write(envType::Int16(nVerts));
	//	MPoint point;
	//	for(int i = 0; i < nVerts; i++) 
	//	{
	//		point = positions[i];
	//		o_Writer.Write(float(point[0]));
	//		o_Writer.Write(float(point[1]));
	//		o_Writer.Write(float(point[2]));
	//	}
	//	o_Writer.FinishChunk();

	//	
	//	/*// Normals
	//	int nNormals = morph.numNormals();
	//	o_Writer.WriteChunkHeader(c_NVER, 0, false);
	//	o_Writer.Write(envType::Int16(nNormals));
	//	MFloatVectorArray normalArray;
	//	morph.getNormals(normalArray);
	//	for (int n = 0; n < nNormals; n++) 
	//	{
	//		MFloatVector normal = normalArray[n];
	//		o_Writer.Write(float(normal[0]));
	//		o_Writer.Write(float(normal[1]));
	//		o_Writer.Write(float(normal[2]));
	//	}
	//	o_Writer.FinishChunk();*/

	//	o_Writer.FinishChunk();

	//}

	//========================================================================
	// Deltas version - writes only the difference between the base mesh 
	//	and the morph target
	//========================================================================
	void WriteMorphDeltas(const MPointArray& i_SparseDeltas,
						  const MIntArray& i_SparseIndices,
						  const char* i_NodeName, 
						  const char* i_WeightName, 
						  chWriter &o_Writer)
	{
		int nVerts = i_SparseDeltas.length();
		if (nVerts != i_SparseIndices.length())
		{
			cout << "WriteMorphDeltas: deltas and indices need to match.";
			return;
		}
		
		cout << "Have deltas for blend shape target, " << i_NodeName << " aliased " << i_WeightName << " nVerts = " << nVerts << endl;

		o_Writer.WriteChunkHeader(c_MRPH, 0, true);

		// Name of blend shape node
		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		o_Writer.Write(MayaUtil::PrepareName(i_NodeName).asUTF8()); 
		o_Writer.FinishChunk();

		// Aliased name for weight (this one is visible to the artist in Maya)
		o_Writer.WriteChunkHeader(c_WNAM, 0, false);
		o_Writer.Write(MayaUtil::PrepareName(i_WeightName).asUTF8()); 
		o_Writer.FinishChunk();

		// Deltas
		o_Writer.WriteChunkHeader(c_MDLT, 0, false);
		o_Writer.Write(envType::Int16(nVerts));
		MPoint delta;
		for(int v = 0; v < nVerts; v++) 
		{
			delta = i_SparseDeltas[v];
			o_Writer.Write(float(delta[0]));
			o_Writer.Write(float(delta[1]));
			o_Writer.Write(float(delta[2]));
		}
		// Write sparse indices immediately afterwards in same chunk
		for(int v = 0; v < nVerts; v++) 
		{
			o_Writer.Write(envType::Int32(i_SparseIndices[v]));
		}
		o_Writer.FinishChunk();

		o_Writer.FinishChunk();
	}


	//========================================================================
	// See if the infos list already has a target with the given name
	//========================================================================
	bool have_morph_with_name(MString &aliasName, 
		const std::vector< shared_ptr<CharacterFuncs::MorphTargetInfo> > &i_Infos)
	{
		const int num_infos = i_Infos.size();
		for (int i=0; i<num_infos; ++i)
		{
			if (i_Infos[i]->m_AliasName == aliasName)
				return true;
		}
		return false;
	}

	//========================================================================
	// Try to find out morph target info deltas through plugs
	//========================================================================
	bool GetMorphDeltaInfos(MFnBlendShapeDeformer &blendShape, 
		std::vector< shared_ptr<CharacterFuncs::MorphTargetInfo> > &o_Infos,
		MString &o_Message)
	{		
		MStatus status;
		int num_weights = blendShape.numWeights();
		if (num_weights == 0)
		{
			if (bWriteInputDetails)
				cout << "BlendShape " << blendShape.name() << " has zero weights." << endl;
			return false;
		}
		MIntArray indexList;
		blendShape.weightIndexList( indexList );
		if (indexList.length() != num_weights)
		{
			cout << "BlendShape " << blendShape.name() << " could not match up weight indices " << indexList.length() << " vs " << num_weights << endl;
			return false;
		}

		// weight is an array plug, one per target object
		MStatus weight_plug_status;
		MPlug weight_plug = blendShape.findPlug("weight", &weight_plug_status);

		bool bHaveEmptyAlias = false;

		// Just get weights from first base object for now
		for (int i=0; i<num_weights; i++)
		{
			int weight_index = indexList[i];
			if (bWriteInputDetails)
				cout << "Weight index " << i << " is " << weight_index << endl;

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
					}
				}
			}

			// Try to find out about deltas through plugs
			shared_ptr<CharacterFuncs::MorphTargetInfo> target_info(new CharacterFuncs::MorphTargetInfo);
			GetMorphDeltas(blendShape, weight_index, 
							target_info->m_PositionVecs, target_info->m_SparseIndices);
			if (target_info->m_PositionVecs.length() > 0)
			{
				if (bWriteInputDetails)
					cout << "Have morph target deltas: " << blendShape.name() << " alias " << aliasName << endl;

				if (aliasName.length() == 0)
					bHaveEmptyAlias = true;

				if (!have_morph_with_name(aliasName, o_Infos))
				{
					target_info->m_bVecsAreDeltas = true;
					target_info->m_BlendShapeName = blendShape.name();
					target_info->m_AliasName = aliasName;
					o_Infos.push_back(target_info);		
				}
				else if (bWriteInputDetails)
					cout << "Already have target with same alias, skipping deltas." << endl;

			}
		}

		// If a blend shape has multiple targets, then the alias names
		// are used to differentiate them. If an alias is empty string, then
		// we need to warn the user.
		if ((num_weights > 1) && bHaveEmptyAlias)
		{
			o_Message += (blendShape.name() + " blend shape has unnamed weights. \\n");
		}

		return true;

	}

	//========================================================================
	//========================================================================
	bool WriteMorphDeltasFromBlendShape(MFnBlendShapeDeformer blendShape, 
										chWriter &o_Writer)
	{
		MStatus status;
		int num_weights = blendShape.numWeights();
		if (num_weights == 0)
		{
			cout << "BlendShape " << blendShape.name() << " has zero weights." << endl;
			return false;
		}
		MIntArray indexList;
		blendShape.weightIndexList( indexList );
		if (indexList.length() != num_weights)
		{
			cout << "BlendShape " << blendShape.name() << " could not match up weight indices " << indexList.length() << " vs " << num_weights << endl;
			return false;
		}

		// weight is an array plug, one per target object
		MStatus weight_plug_status;
		MPlug weight_plug = blendShape.findPlug("weight", &weight_plug_status);

		// Just get weights from first base object for now
		for (int i=0; i<num_weights; i++)
		{
			int weight_index = indexList[i];
			if (bWriteInputDetails)
				cout << "Weight index " << i << " is " << weight_index << endl;

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
					}
				}
			}

			// Try to find out about deltas through plugs
			MPointArray sparse_deltas;
			MIntArray sparse_indices;
			GetMorphDeltas(blendShape, weight_index, sparse_deltas, sparse_indices);
			if (sparse_deltas.length() > 0)
			{
				WriteMorphDeltas(sparse_deltas, sparse_indices, blendShape.name().asUTF8(), aliasName.asUTF8(), o_Writer);
			}
		}
		return true;

	}

	//========================================================================
	// Return true if the mesh is being influenced by a deformation that
	// we cannot handle - forces animation to baked vertex animation.
	//========================================================================
	bool AutoDetectDeformation(MFnMesh &mesh)
	{
		// Look for animation on the "pnts" plug directly
		if (VertexFuncs::MeshHashVertexAnimation(mesh))
		{
			if (bWriteAutoDetectDetails)
				cout << mesh.name() << " has animation on points channel" << endl;
			return true;
		}

		// Gather up nodes in the history, look for deformations we cannot handle
		MObjectArray inputs;
		CharacterFuncs::GetGeometryFiltersFromInputs(mesh, inputs);
			
		//cout << "Num inputs: " << inputs.length() << endl;
		for (int i=0; i<inputs.length(); i++)
		{
			MObject obj = inputs[i];
			//cout << "input geom is of type " << obj.apiTypeStr() << endl;

			// Geometry Filter is the base class for all deforming nodes
			if (obj.hasFn(MFn::kGeometryFilt))
			{
				// Skin Clusters and Tweak nodes are okay, all others force baked vertex anim
				if (!obj.hasFn(MFn::kSkinClusterFilter) &&
					!obj.hasFn(MFn::kTweak))
				{
					if (bWriteAutoDetectDetails)
						cout << "Mesh " << mesh.name() << " has deforming geometry filter of of type " << obj.apiTypeStr() << endl;
					return true;
				}
			}
		}

		// If we got here then all inputs are acceptable, or we did not
		// have any inputs.
		return false;
	}

}	// end of namespace


//========================================================================
//	DebugInputs - print out inputs that are used to 
//	create this shape
//========================================================================
void CharacterFuncs::DebugInputs(MFnMesh &mesh)
{
	//MayaUtil::PrintInputs(mesh);

	MObjectArray inputs;
	GetGeometryFiltersFromInputs(mesh, inputs);
		
	cout << "*** Num inputs: " << inputs.length() << endl;
	for (int i=0; i<inputs.length(); i++)
	{
		MObject baseObj = inputs[i];
		cout << "input geom is of type " << baseObj.apiTypeStr() << endl;

		MFnDependencyNode node(baseObj);
		cout << "named: " << node.name() << endl;
	}
}

//========================================================================
//	DebugInputs - print out inputs that are used to 
//	create this shape
//========================================================================
//void CharacterFuncs::DebugInputs(MFnSubd &subdiv)
//{
//	//MayaUtil::PrintInputs(mesh);
//
//	MObjectArray inputs;
//	GetGeometryFiltersFromInputs(subdiv, inputs);
//		
//	cout << "Num inputs: " << inputs.length() << endl;
//	for (int i=0; i<inputs.length(); i++)
//	{
//		MObject baseObj = inputs[i];
//		cout << "input geom is of type " << baseObj.apiTypeStr() << endl;
//
//		MFnDependencyNode node(baseObj);
//		cout << "named: " << node.name() << endl;
//	}
//}



//========================================================================
//========================================================================
void CharacterFuncs::GetGeometryFiltersFromInputs(MFnMesh &mesh, MObjectArray &objects)
{
	MStatus status;
	MObject obj = MayaUtil::GetObjectConnectedToPlug( mesh, "inMesh", status); 
	while (status == MS::kSuccess) 
	{
		if (bWriteInputDetails)
			cout << "Have input mesh of type: " << obj.apiTypeStr() << endl;

		if (obj.hasFn(MFn::kGeometryFilt))
		{
			MFnGeometryFilter filter(obj, &status);
			if (status == MS::kSuccess) 
			{
				AddUnique(objects, obj);
				::GetGeometryFiltersFromInputs(filter, objects);

				// We got the inputs, so break out of loop
				break;
			}
		}
		else if (obj.hasFn(MFn::kPolyBlindData))
		{
			MFnDependencyNode polyBlind(obj, &status);
			if (status == MS::kSuccess) 
			{
				obj = MayaUtil::GetObjectConnectedToPlug( polyBlind, "inMesh", status);
				if (status == MS::kSuccess) 
				{
					if (bWriteInputDetails)
						cout << "PolyBlind connected to object of type: " << obj.apiTypeStr() << endl;
				}
			}
		}
		else if (obj.hasFn(MFn::kMidModifier) || // Not sure about this, it looks like a common base for poly modifiers
				 obj.hasFn(MFn::kPolyTransfer) ||
				 obj.hasFn(MFn::kPolyNormal)) 
		{
			MFnDependencyNode polyModifier(obj, &status);
			if (status == MS::kSuccess) 
			{
				obj = MayaUtil::GetObjectConnectedToPlug( polyModifier, "inputPolymesh", status);
				if (status == MS::kSuccess) 
				{
					if (bWriteInputDetails)
						cout << "PolyModifier connected to object of type: " << obj.apiTypeStr() << endl;
				}
			}
		}
		else if (obj.hasFn(MFn::kGroupParts))
		{
			MFnDependencyNode group_parts(obj);
			obj = MayaUtil::GetObjectConnectedToPlug(group_parts, "inputGeometry", status);
			if (status == MS::kSuccess)
			{
				if (bWriteInputDetails)
					cout << "GroupParts connected to object of type: " << obj.apiTypeStr() << endl;
			}
		}
		else
		{
			// if unrecognized node, break out
			if (bWriteInputDetails)
				cout << "Unrecognized poly modifier: " << obj.apiTypeStr() << " on mesh " << mesh.name() << endl;
			break;
		}
	}
}

//========================================================================
//========================================================================
//void CharacterFuncs::GetGeometryFiltersFromInputs(MFnSubd &subdiv, MObjectArray &objects)
//{
//	MStatus status;
//	MObject obj = MayaUtil::GetObjectConnectedToPlug( subdiv, "create", status); 
//	if (status == MS::kSuccess) 
//	{
//		MFnGeometryFilter filter(obj, &status);
//		if (status == MS::kSuccess) 
//		{
//			AddUnique(objects, obj);
//			::GetGeometryFiltersFromInputs(filter, objects);
//		}
//		else if (obj.hasFn(MFn::kSubdModifier))
//		{
//			SeekInputsToModifiers(obj, objects);
//		}
//		else
//		{
//			cout << "Unexpected node, Subdiv 'create' plug connected to node of type: " << obj.apiTypeStr() << endl;
//		}
//	}
//}

//========================================================================
// Set boolean for whether to detect deforning animation ourselves, or to 
// just look for Maya flag "sgpuDeformGeom"
//========================================================================
void CharacterFuncs::SetAutoDetectDeformation(bool i_bEnable)
{
	l_bAutoDetectDeformation = i_bEnable;
}
bool CharacterFuncs::GetAutoDetectDeformation()
{
	return l_bAutoDetectDeformation;
}

//========================================================================
// Return true if the mesh is being influenced by a deformation that
// we cannot handle - forces animation to baked vertex animation.
//========================================================================
bool CharacterFuncs::IsDeformingGeometry(MFnMesh &mesh)
{
	if (l_bAutoDetectDeformation)
	{
		// Special hard-coded case of this attribute, because we
		// want to know if the flag exists or not.
		if (MayaFlagUtil::EngineFlagExists(mesh, "sgpuDeformGeom"))
		{
			// If the attribute exists, then use that as a user-override value
			bool bCloth =  (MayaFlagUtil::GetEngineFlag(mesh, "sgpuDeformGeom") != 0);
			if (bWriteAutoDetectDetails)
				cout << mesh.name() << " contains sgpuDeformGeom flag, value: " << bCloth << endl;
			return bCloth;
		}

		return AutoDetectDeformation(mesh);
	}
	else
	{
		return MayaFlagUtil::GetClothFlag(mesh);
	}
}

//========================================================================
// Get information about the blend shapes for this mesh.
//========================================================================
void CharacterFuncs::GetMorphTargets(MFnMesh &mesh, 
									 std::vector< shared_ptr<MorphTargetInfo> >& o_MorphTargets,
									 MString &o_Message)
{
	MStatus status;

	// Gather up blend shape nodes
	MObjectArray inputs;
	GetGeometryFiltersFromInputs(mesh, inputs);
		
	//cout << "Num inputs: " << inputs.length() << endl;
	for (int i=0; i<inputs.length(); i++)
	{
		MObject obj = inputs[i];
		//cout << "input geom is of type " << obj.apiTypeStr() << endl;

		MFnBlendShapeDeformer blend(obj, &status);
		if (status == MS::kSuccess) 
		{
			if (bWriteInputDetails)	
				cout << "Found blend shape: " << blend.name() << endl;
			//DebugBlendShape(blend);
			
			MObjectArray targets;
			MStringArray aliases;
			if (GetTargetsFromBlendShape(blend, targets, aliases))
			{
				int num_targets = targets.length();
				if (bWriteInputDetails)	
					cout << " num targets: " << num_targets << endl;
				for (int t=0; t<num_targets; t++)
				{
					MFnMesh morph(targets[t], &status);
					if (status == MS::kSuccess) 
					{
						if (bWriteInputDetails)	
							cout << "Morph target " << t << " name=" << morph.name() << " alias=" << aliases[t] << endl;
						shared_ptr<MorphTargetInfo> target_info(new MorphTargetInfo);
						target_info->m_BlendShapeName = morph.name();
						target_info->m_AliasName = aliases[t];
						if (GetMorphTargetInfo(morph, *target_info))
							o_MorphTargets.push_back(target_info);
					}
					//else cout << "Blend shape target " << t << " is not a mesh, cannot be written." << endl;
				}
			}

			// Look for morph deltas even if we found targets
			//else
			{
				// If the targets have been deleted in Maya (which is common to save memory),
				// then the above code will not find any targets.  Have to use the plugs themselves
				// to get at the morph deltas.
				GetMorphDeltaInfos(blend, o_MorphTargets, o_Message);
			}
		}
	}
}		

//========================================================================
//	WriteMeshAndMorphTargets - gets base mesh and morph targets
//		for this mesh by looking through inputs in the mesh's history.
//		Writes the info for them to the writer.
//========================================================================
void CharacterFuncs::WriteMeshAndMorphTargets(MFnMesh &mesh, 
											  chWriter &o_Writer, 
											  bool i_bWriteAsSubdiv)
{
	MStatus status;

	if (i_bWriteAsSubdiv)
	{
		// Interpret polygon mesh as the control mesh for a subdivision surface.
		// Make things easier for the artists sometimes to just export as subdiv
		// instead of converting in Maya.
		SubdivFuncs::WriteMeshAsSubdiv(mesh, o_Writer);
	}
	else
	{
		// Write mesh info from the given mesh. This should probably be changed
		// to get the vertex position info from the original mesh in the 
		// inputs list.
		SceneFuncs::WriteBRepToFile(mesh, o_Writer);
	}

	cout << "Write MorphTargets: name " << mesh.name() << endl;

	// Gather up blend shape nodes
	MObjectArray inputs;
	GetGeometryFiltersFromInputs(mesh, inputs);
		
	//cout << "Num inputs: " << inputs.length() << endl;
	for (int i=0; i<inputs.length(); i++)
	{
		MObject obj = inputs[i];
		//cout << "input geom is of type " << obj.apiTypeStr() << endl;

		MFnBlendShapeDeformer blend(obj, &status);
		if (status == MS::kSuccess) 
		{
			cout << "Found blend shape: " << blend.name() << endl;
			//DebugBlendShape(blend);
			
			MObjectArray targets;
			MStringArray aliases;
			if (GetTargetsFromBlendShape(blend, targets, aliases))
			{
				int num_targets = targets.length();
				cout << " num targets: " << num_targets << endl;
				for (int t=0; t<num_targets; t++)
				{
					MFnMesh morph(targets[t], &status);
					if (status == MS::kSuccess) 
					{
						cout << "Morph target " << t << " name=" << morph.name() << " alias=" << aliases[t] << endl;
						WriteMorphTarget(morph, blend.name().asUTF8(), aliases[t].asUTF8(), o_Writer);
					}
					else cout << "Blend shape target " << t << " is not a mesh, cannot be written." << endl;
				}
			}
			else
			{
				// If the targets have been deleted in Maya (which is common to save memory),
				// then the above code will not find any targets.  Have to use the plugs themselves
				// to get at the morph deltas.
				WriteMorphDeltasFromBlendShape(blend, o_Writer);
			}
		}
	}
}

//========================================================================
//	WriteSubdivAndMorphTargets - gets base subdiv and morph targets
//		for this subdiv by looking through inputs in the subdiv's history.
//		Writes the info for them to the writer.
//========================================================================
//void CharacterFuncs::WriteSubdivAndMorphTargets(MFnSubd &subdiv, chWriter &o_Writer)
//{
//	MStatus status;
//
//	SubdivFuncs::WriteSubdivToFile(subdiv, o_Writer);
//
//	cout << "Write MorphTargets: name " << subdiv.name() << endl;
//	
//	// Gather up blend shape nodes
//	MObjectArray inputs;
//	GetGeometryFiltersFromInputs(subdiv, inputs);
//		
//	if (bWriteInputDetails)
//		cout << "Num inputs: " << inputs.length() << endl;
//	for (int i=0; i<inputs.length(); i++)
//	{
//		MObject obj = inputs[i];
//		if (bWriteInputDetails)
//			cout << "input geom is of type " << obj.apiTypeStr() << endl;
//
//		MFnBlendShapeDeformer blend(obj, &status);
//		if (status == MS::kSuccess) 
//		{
//			cout << "Found blend shape: " << blend.name() << endl;
//			//DebugBlendShape(blend);
//
//			MObjectArray targets;
//			MStringArray aliases;
//			if (GetTargetsFromBlendShape(blend, targets, aliases))
//			{
//				int num_targets = targets.length();
//				cout << " num targets: " << num_targets << endl;
//				for (int t=0; t<num_targets; t++)
//				{
//					MFnSubd morph(targets[0], &status);
//					if (status == MS::kSuccess) 
//					{
//						cout << "Blend shape target is subdiv, named " << morph.name() << " alias=" << aliases[t] << endl;
//						WriteMorphTarget(morph, blend.name().asUTF8(), aliases[t].asUTF8(), o_Writer);
//					}
//					else cout << "Blend shape target " << t << " is not a subdiv, cannot be written." << endl;
//				}
//			}
//		}
//	}
//}

//========================================================================
//	WriteBlendShapeAnimation - writes blend shape animation
//	for this subdivision.
//========================================================================
//void CharacterFuncs::WriteBlendShapeAnimation(MFnSubd &subdiv, 
//											  chWriter &o_Writer, 
//											  bool i_bSinglePose)
//{
//	MStatus status;
//
//	// Gather up blend shape nodes
//	MObjectArray inputs;
//	GetGeometryFiltersFromInputs(subdiv, inputs);
//		
//	if (bWriteInputDetails)
//		cout << "Num inputs: " << inputs.length() << endl;
//	for (int i=0; i<inputs.length(); i++)
//	{
//		MObject obj = inputs[i];
//		if (bWriteInputDetails)
//			cout << "input geom is of type " << obj.apiTypeStr() << endl;
//
//		MFnBlendShapeDeformer blend(obj, &status);
//		if (status == MS::kSuccess) 
//		{
//			cout << "Found blend shape: " << blend.name() << endl;
//
//			AnimFuncs::WriteWeightAnimation(o_Writer, blend, i_bSinglePose);
//		}
//	}
//}

//========================================================================
//========================================================================
void CharacterFuncs::WriteBlendShapeAnimation(MFnMesh &mesh, 
											  chWriter &o_Writer, 
											  double i_MinTime, 
											  double i_MaxTime, 
											  bool i_bSinglePose)
{
	MStatus status;

	// Gather up blend shape nodes
	MObjectArray inputs;
	GetGeometryFiltersFromInputs(mesh, inputs);
		
	if (bWriteInputDetails)
		cout << "Num inputs: " << inputs.length() << endl;
	for (int i=0; i<inputs.length(); i++)
	{
		MObject obj = inputs[i];
		if (bWriteInputDetails)
			cout << "input geom is of type " << obj.apiTypeStr() << endl;

		MFnBlendShapeDeformer blend(obj, &status);
		if (status == MS::kSuccess) 
		{
			if (bWriteInputDetails)	
				cout << "Found blend shape: " << blend.name() << endl;
			AnimFuncs::WriteWeightAnimation(o_Writer, blend, i_MinTime, i_MaxTime, i_bSinglePose);
		}
	}
}
