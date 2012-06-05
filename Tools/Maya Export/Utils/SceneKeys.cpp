/*****************************************************************************
**  SceneKeys.cpp
**
**   Namespace for baking animations in transformation hierarchies   
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <SceneKeys.hpp>
#include <MayaFlagUtil.hpp>
#include <SceneIterator.hpp>

#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>

#include "Core/ch/chBinWriter.hpp"

namespace SceneKeys
{

	namespace
	{
		bool	bWriteKeyDetails = false;

		//=============================================================================
		//	Chunk types
		//=============================================================================
		const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
		const chDefs::Name c_ANLV = chDefs::MakeName('A', 'N', 'L', 'V');

		//class GatherIterator : public JointIterator
		//{
		//public:
		//	//========================================================================
		//	//========================================================================
		//	GatherIterator(std::list<PathKeys> &o_Keys)
		//		:	m_Keys(o_Keys)	{}

		//	//========================================================================
		//	// Virtual function for visiting a node in the hierarchy.
		//	//========================================================================
		//	virtual void Visit(MObject &i_Object)
		//	{
		//		MStatus status;
		//		MFnTransform transform(i_Object, &status);
		//		if (status)
		//		{
		//			PathKeys keys;
		//			keys.m_Transform.setObject( i_Object );

		//			// visibility animation
		//			bool bWriteVisible = MayaFlagUtil::GetVisibleAnimFlag(transform);
		//			//cout << "write visibility?: visibleAnim = " << bWriteVisible << endl;
		//			keys.m_TransformKeys.m_bWriteVisibility = bWriteVisible;

		//			m_Keys.push_back(keys);
		//		}

		//		// Recurse on children
		//		JointIterator::Visit(i_Object);
		//	}

		//private:
		//	std::list<PathKeys> &m_Keys;
		//};

		//class WriteIterator : public JointIterator
		//{
		//public:
		//	//========================================================================
		//	//========================================================================
		//	WriteIterator(chWriter &o_Writer, const std::list<PathKeys> &i_Keys,
		//				 float i_TimeOffset)
		//		:	m_Writer(o_Writer), m_Iterator(i_Keys.begin()), m_TimeOffset(i_TimeOffset)
		//	{
		//	}

		//	//========================================================================
		//	// Virtual function for visiting a node in the hierarchy.
		//	//========================================================================
		//	virtual void Visit(MObject &i_Object)
		//	{
		//		MStatus status;
		//		MFnTransform transform(i_Object, &status);
		//		if (status)
		//		{
		//			m_Writer.WriteChunkHeader(c_ANLV, 1, true);

		//			// Node name
		//			m_Writer.WriteChunkHeader(c_NNAM, 0, false);
		//			m_Writer.Write(MayaUtil::PrepareName(transform.name()).asUTF8());
		//			m_Writer.FinishChunk();

		//			bool bWriteDeltas = false;
		//			AnimKeys::WriteAnimation(m_Writer, m_Iterator->m_TransformKeys, m_TimeOffset, bWriteDeltas);
		//			++m_Iterator;
		//		} 
		//	
		//		// Recurse on children
		//		JointIterator::Visit(i_Object);
		//		
		//		if (status)
		//		{
		//			m_Writer.FinishChunk();	// c_ANLV
		//		}
		//	}

		//private:
		//	chWriter& m_Writer;
		//	std::list<PathKeys>::const_iterator m_Iterator;
		//	float m_TimeOffset;
		//};

	}	// end of namespace


	//========================================================================
	// Gets traverses hierarchy and gathers a list of key structures
	//	in order to bake animation later.
	//========================================================================
	//void GatherHierarchy(MFnTransform &i_Transform,
	//					 std::list<PathKeys> &o_Keys)
	//{
	//	GatherIterator gather(o_Keys);
	//	gather.Traverse(i_Transform);
	//}

	//========================================================================
	// Gets current state of transforms and puts the data into the
	//	list data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(std::list<PathKeys> &io_Keys, 
					float i_CurrentTime)
	{
		MStatus status;
		std::list<PathKeys>::iterator it;
		for (it = io_Keys.begin(); it != io_Keys.end(); ++it)
		{
			MFnTransform transform(it->m_DagPath, &status);	
			if (status == MS::kSuccess) 
			{	
				AnimKeys::GatherKeys( it->m_TransformKeys, i_CurrentTime, transform );	
			}
			else
			{
				cout << "Error getting transform node from MDagPath handle." << endl;
			}
		}
	}

	//========================================================================
	//	WriteAnimation - write animation based on the given keys
	//		that were gathered earlier.
	//========================================================================
	//void WriteAnimation(chWriter &o_Writer,
	//					 MFnTransform &i_Transform,
	//					 const std::list<PathKeys> &i_Keys,
	//					 float i_TimeOffset)
	//{
	//	WriteIterator writer(o_Writer, i_Keys, i_TimeOffset);
	//	writer.Traverse(i_Transform);
	//}
	
	//========================================================================
	// Gets mesh out of DAG path to surface and checks for
	//	a visibility animation flag.
	//========================================================================
	void GatherSurfaceVisibility(MDagPath &i_DagPath,
								 std::list<SurfaceVisKeys> &o_Keys,
								 const std::string &i_Name)
	{
		MStatus status;
		// Look for the mesh at the end of the path
		if (i_DagPath.hasFn(MFn::kMesh))
		{
			MFnMesh mesh(i_DagPath, &status);
			if (status == MS::kSuccess)
			{
				//bga -  Note: this is now altered so that WriteVerts assumes
				// a visibility animation is needed for all meshes...
				//
				// visibility animation, detect flag on mesh
				//bool bWriteVisible = (MayaFlagUtil::GetVisibleAnimFlag(mesh));
				//cout << "write visibility?: visibleAnim = " << bWriteVisible << endl;
				//if (bWriteVisible)
				{
					SurfaceVisKeys keys;
					keys.m_DagPath = i_DagPath;
					keys.m_Keys.m_Name = i_Name;
					o_Keys.push_back(keys);
				}
			}
		}
	}
	
	//========================================================================
	// Gets current state of visibility and puts the data into the
	//	list data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(std::list<SurfaceVisKeys> &io_Keys, 
					float i_CurrentTime)
	{
		MStatus status;
		std::list<SurfaceVisKeys>::iterator it;
		for (it = io_Keys.begin(); it != io_Keys.end(); ++it)
		{
			//MFnMesh mesh(it->m_DagPath, &status);
			//if (status == MS::kSuccess)
			//{
			//	AnimKeys::GatherKeys(it->m_Keys, i_CurrentTime, mesh);
			//}
			AnimKeys::GatherKeys(it->m_Keys, i_CurrentTime, it->m_DagPath);
		}
	}

	//========================================================================
	//	WriteAnimation - write animation based on the given keys
	//		that were gathered earlier.
	//========================================================================
	void WriteAnimation(chWriter &o_Writer,
						const std::list<SurfaceVisKeys> &i_Keys,
						float i_TimeOffset)
	{
		std::list<SurfaceVisKeys>::const_iterator it;
		for (it = i_Keys.begin(); it != i_Keys.end(); ++it)
		{
			AnimKeys::WriteSkinVisibility(o_Writer, it->m_Keys, i_TimeOffset);
		}

	}
}