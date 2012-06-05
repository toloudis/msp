//*****************************************************************************
/*!	\file  exportmesh_command.h
	\brief exportmesh_command classes for reading and writing mesh data.
	
	
*/
//*****************************************************************************

#ifndef _EXPORTMESH_COMMAND_H_
#define _EXPORTMESH_COMMAND_H_
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <list>
#include <xsi_progressbar.h>
#include <xsi_value.h>
#include <xsi_model.h>
#include <boost/boost/shared_ptr.hpp>
#include "Log.h"
#include "sgpuModelExportScene.hpp"
class XSI::CString;
class XSI::CValue;
class XSI::CStatus;

class sgpuNode;
class sgpuMesh;
class GXBExporter;

int SafeLongToInt( LONG l );

typedef enum  { eModelExport, eVertexAnimExport, eCameraAnimExport } TExportIntent;

#define PLUGIN_VERSION L"1.2.3.17"

class GXBExportDoc
{
public:
	struct Texture_ExportFilepath
	{
		typedef enum { eRelative, eAbsolute, eCopyTextureToExportDir } Value;

		static const XSI::CString m_scriptName;
		static const XSI::CString m_friendlyName;
		static const XSI::CString m_valueNames[3];
		static const Value m_default;
		static const Value m_min;
		static const Value m_max;
	};

	struct MergeBasedOnMtls
	{
		typedef bool Value;
		static const XSI::CString m_scriptName;
		static const XSI::CString m_friendlyName;
		static const XSI::CString m_valueNames[1];
		static const Value m_default;
	};

	
	struct ExportGeomForVertexAnim
	{
		typedef bool Value;
		static const XSI::CString m_scriptName;
		static const XSI::CString m_friendlyName;
		static const XSI::CString m_valueNames[1];
		static const Value m_default;
	};
	
	struct MaxNumTrianglesInMergedMesh
	{
		typedef LONG Value;

		static const XSI::CString m_scriptName;
		static const XSI::CString m_friendlyName;
		static const XSI::CString m_valueNames[1];
		static const Value m_default;
		static const Value m_min;
		static const Value m_max;
	};

	struct AnimExportStartFrame
	{
		typedef LONG Value;

		static const XSI::CString m_scriptName;
		static const XSI::CString m_friendlyName;
		static const XSI::CString m_valueNames[1];
		static const Value m_default;
	};

	struct AnimExportEndFrame
	{
		typedef LONG Value;

		static const XSI::CString m_scriptName;
		static const XSI::CString m_friendlyName;
		static const XSI::CString m_valueNames[1];
		static const Value m_default;
	};
	struct TexturesToCopy
	{
		XSI::CString m_src;
		XSI::CString m_dst;
	};
public:
	GXBExportDoc(XSI::CString & filename, XSI::CValueArray args);
	~GXBExportDoc();
	bool DoExport(  );
	void InitializeProgressBar( int maxSteps );
	void UpdateStep( int step );
	void UpdateCaption( const TCHAR * pzCaption );	
	void WriteLog( const wchar_t * format, ... );
	void WriteLog( const char* format, ... );
protected:
	void ResolveFromExportIntent();
public:
	Log						m_log;
	XSI::CString					m_filename;

	XSI::siConstructionMode			m_constMode;
	XSI::siSubdivisionRuleType		m_subdType;
	LONG							m_subdLevel;
	XSI::ProgressBar				m_bar;
	boost::shared_ptr< GXBExporter >		m_exporter;
	TExportIntent					m_exportIntent;
	XSI::CString					m_newExt;
	Texture_ExportFilepath::Value	m_textureExportFilepath;
	MergeBasedOnMtls::Value			m_mergeBasedOnMtls;
	ExportGeomForVertexAnim::Value		m_exportGeomForVertexAnim;
	MaxNumTrianglesInMergedMesh::Value		m_maxNumTrianglesInMergedMesh;
	AnimExportStartFrame::Value					m_animExportStartFrame;
	AnimExportEndFrame::Value					m_animExportEndFrame;
	std::list< TexturesToCopy >					m_texturesToCopy;
};



#endif