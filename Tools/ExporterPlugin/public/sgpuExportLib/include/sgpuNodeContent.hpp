/****************************************************************************\
**  sgpuNodeContent.hpp
**
**      sgpuNodeContent.hpp defines the base class for handles such as
** sgpuMesh, sgpuSubdiv and sgpuPathReference which are contents of the node
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_NODECONTENT_HPP
#define SGPU_NODECONTENT_HPP

#include "sgpuExportLib.hpp"
#include "sgpuVector.hpp"

//============================================================================
//============================================================================
class sgpuMaterial;
class sgpuNode;


//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuNodeContent
{
public:
	typedef enum { eNone, eMesh, eSubdiv, ePathReference }ESgpuNodeContentType  ;
	//========================================================================
	// Set name of the polygon mesh
	//========================================================================
	virtual ~sgpuNodeContent();
	ESgpuNodeContentType GetNodeContent()const { return m_NodeContentType ; }
protected:
	sgpuNodeContent();
	sgpuNodeContent( const sgpuNodeContent & i_Other );
	sgpuNodeContent & operator=( const sgpuNodeContent &i_Other);
	ESgpuNodeContentType m_NodeContentType;

	friend sgpuNode;
};

#endif // #ifndef SGPU_MESH_HPP