//*****************************************************************************
/*!	\file gxbmodelexporter.h
	\brief exports mesh geometry data
	
	
*/
//*****************************************************************************

#ifndef _GXBMODELEXPORTER_H_
#define _GXBMODELEXPORTER_H_

#include <stdio.h>
#include <tchar.h>
#include "gxbexporter.h"
#include "helper.h"
#include "sgpuModelExportScene.hpp"

class sgpuNode;
class sgpuMesh;

class GXBModelExporter : public GXBExporter
{
public:
	GXBModelExporter( GXBExportDoc &doc , bool i_bVertexAnimation );
	~GXBModelExporter();

	bool DoExport();

protected:

	typedef std::vector<sgpuVertex>     vecVertex_t;
	void					   enumProps( const XSI::X3DObject & xobj );
	bool                       exportObject(XSI::X3DObject & xobj, sgpuNode & parent_node, TransformStack &tmStack );
	bool                        exportMesh( const XSI::X3DObject &xobj, sgpuNode & node , TransformStack &tmStack );
	bool                        exportNurbsMesh(const XSI::X3DObject &xobj, const XSI::Material & xmat, sgpuNode &node, TransformStack &tmStack);
	bool						   exportSubdiv(const XSI::X3DObject &xobj, sgpuNode & node, TransformStack &tmStack );
	void						buildIndicesForAnimation( 
										std::vector <sgpuVertex> & vtxVector, 
										std::vector <int> & idxVector );
	void						buildIndices(
										std::vector <sgpuVertex> & vtxVector, 
										std::vector <int> & idxVector );
	
	void						flipTriangleIndices( std::vector< int > indices );
	bool                       saveAsOBJ(const TCHAR* name, std::vector <sgpuVertex> & vtx, std::vector <int> & idx);
	sgpuMesh                   addMesh(sgpuNode & node, vecVertex_t & vtx, std::vector <int> & idx);
	XSI::CString                        addMaterial(const XSI::Material & xmat);
	bool                       enumMeshes(XSI::X3DObject & xobj, TObjectsTobeExported & tobeExported );
	XSI::CColor                getCororParameter ( const XSI::Material & xmat, TCHAR* in_szParamName);
	void                       dumpMaterial(int nMat);
	void                       dumpProperties(const XSI::X3DObject & xobj);
	void                       dumpProperties(const XSI::Material & xmat);
	void						CopyTextures();
	XSI::CString				GetDstDirForTexturesToBeCopied() const;
	void                       GetShader( const XSI::Material& xmat, const XSI::CString& type, const XSI::CString& channel, XSI::Shader& tex );
	void						TransformUV( const XSI::Material& xmat, std::vector<sgpuVertex> &io_sgpuVertices );

protected:
	GXBModelExporter &operator=( const GXBModelExporter &other ) { return *this; }
public:
	sgpuScene							m_scene;
	int									m_numNodes;
	int									m_numMeshes;
	std::vector <sgpuMaterialInfo>		m_matsInfo;    
	std::vector <sgpuMaterial>			m_mats;
	bool								m_bVertexAnimation;
	int									m_meshId;
	TObjectsTobeExported					m_objectsTobeExported;
};

#endif // GXBMODELEXPORTER