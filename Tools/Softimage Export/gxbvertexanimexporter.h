//*****************************************************************************
/*!	\file helper.h
	\brief Helper classes for reading and writing mesh data.
	
	
*/
//*****************************************************************************

#ifndef _GXBVERTEXANIMEXPORTER_H_
#define _GXBVERTEXANIMEXPORTER_H_

#include <stdio.h>
#include <tchar.h>
#include <boost/boost/shared_ptr.hpp>
#include "xsi_ref.h"
#include "gxbexporter.h"
#include "helper.h"

class sgpuMesh;
class sgpuVertexAnimExportScene;
struct VertexAnimParams;
class GXBVertexAnimExporter : public GXBExporter
{
public:
	GXBVertexAnimExporter( GXBExportDoc &doc );
	~GXBVertexAnimExporter();

	bool DoExport();

protected:

	typedef std::vector<sgpuVertex>     vecVertex_t;
	void					   enumProps( const XSI::X3DObject & xobj );
	bool                       exportObject(const XSI::X3DObject & xobj, int i_FrameNum );
	bool                        exportMesh( const XSI::X3DObject & xobj, int i_FrameNum ,  bool i_bSubdivMesh );
	bool						   exportNurbsMesh(sgpuNode & node, const XSI::X3DObject & xobj, const XSI::Material & xmat);
	bool							exportNurbsMesh( const XSI::X3DObject & xobj,  int i_FrameNum );
	void						GetVertAnimParameters( VertexAnimParams &io_Params);
	void                       buildIndices(std::vector <sgpuVertex> & vtx, std::vector <int> & idx);
	bool                       saveAsOBJ(const TCHAR* name, std::vector <sgpuVertex> & vtx, std::vector <int> & idx);
	sgpuMesh                   addMesh(sgpuNode & node, vecVertex_t & vtx, std::vector <int> & idx);
	XSI::CString                        addMaterial(const XSI::Material & xmat);
	void                       enumMeshes(const XSI::X3DObject & xobj, int & numMeshes);
	XSI::CColor                getCororParameter ( const XSI::Material & xmat, TCHAR* in_szParamName);
	void                       dumpProperties(const XSI::X3DObject & xobj);
	void                       dumpProperties(const XSI::Material & xmat);
	void                       dumpParameter(const XSI::Parameter & param);
	int							SetCurrentFrame( int i_KeyNum );
	void                       GetShader( const XSI::Material& xmat, const XSI::CString& type, const XSI::CString& channel, XSI::Shader& tex );
	bool						   PeekMesh( const XSI::X3DObject & xobj );
	bool							PeekNurbsSurface( const XSI::X3DObject & xobj );
	bool					   ExportPrefaceChunks( );
protected:
	GXBVertexAnimExporter &operator=( const GXBVertexAnimExporter &other ) { return *this; }
public:
	boost::shared_ptr< sgpuVertexAnimExportScene >		m_scene;
	int                               m_numNodes;
	int                               m_numMeshes;
	std::vector <XSI::MATH::CMatrix4> m_sTransform;
	std::vector <sgpuMaterialInfo>    m_matsInfo;    
	std::vector <sgpuMaterial>        m_mats;
	std::map< XSI::CString, int > m_meshVersusIndexInAnimFileMap;
	static const float				  m_vertexCacheLimit;
	ReportProgress						m_progress;
	int								m_meshId;
};
#endif // GXBVERTEXANIMEXPORTER