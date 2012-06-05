//*****************************************************************************
/*!	\file helper.h
	\brief Helper classes for reading and writing mesh data.
	
	
*/
//*****************************************************************************

#ifndef _GXBCAMERAANIMEXPORTER_H_
#define _GXBCAMERAANIMEXPORTER_H_

#include <stdio.h>
#include <tchar.h>
#include <boost/boost/shared_ptr.hpp>
#include "xsi_ref.h"
#include "gxbexporter.h"
#include "helper.h"


class sgpuNode;
class sgpuMesh;
class sgpuCameraAnimExportScene;
struct VertexAnimParams;
class GXBCameraAnimExporter : public GXBExporter
{
public:
	GXBCameraAnimExporter( GXBExportDoc &doc );
	~GXBCameraAnimExporter();

	bool DoExport();

protected:

	typedef std::vector<sgpuVertex>     vecVertex_t;
	void						enumProps( const XSI::X3DObject & xobj );
	bool						exportObject(const XSI::X3DObject & xobj, int i_FrameNum );
	int							exportCamera(const XSI::X3DObject & xobj, int i_FrameNum  );
	bool						ExportPrefaceChunks( );
	int							SetCurrentFrame( int i_KeyNum );
protected:
	GXBCameraAnimExporter &operator=( const GXBCameraAnimExporter &other ) { return *this; }
public:
	boost::shared_ptr< sgpuCameraAnimExportScene >		m_scene;
	std::vector <XSI::MATH::CMatrix4> m_sTransform;
	std::vector <sgpuMaterialInfo>    m_matsInfo;    
	std::vector <sgpuMaterial>        m_mats;
	std::map< XSI::CString, int > m_meshVersusIndexInAnimFileMap;
	static const float				  m_vertexCacheLimit;
	ReportProgress						m_progress;
	XSI::CString						m_cameraName;
};
#endif // GXBCAMERAANIMEXPORTER