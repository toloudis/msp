//*****************************************************************************
/*!	\file gxbexporter.h
\base class of all different intent-s of exporting
\eg: gxbmodelexporter, gxbvertanimexporter, gxbcameraanimexporter
\are derived from this class
*/
//*****************************************************************************

#ifndef _GXBEXPORTER_H_
#define _GXBEXPORTER_H_

#include <xsi_string.h>
#include <xsi_math.h>
#include <xsi_vector3f.h>
#include <xsi_vector2f.h>
#include <xsi_ref.h>
#include <xsi_x3dobject.h>
#include <stdio.h>
#include <tchar.h>


#define EXPORT_AS_SUBDIV false


namespace XSI
{
	class CString;
	class PolygonMesh;
	class NurbsSurfaceMesh;
	class Material;
	struct CColor;
	class Parameter;
	class Shader;
	class CFloatArray;
	class CDoubleArray;
	class CustomProperty;
}


class GXBExportDoc;
struct AnimParams;

class GXBExporter
{
public:
	GXBExporter( GXBExportDoc &doc );
	virtual ~GXBExporter();
	virtual bool DoExport()=0;
	static void GetAnimParameters( AnimParams &io_Params);
	void dumpParameter(const XSI::Parameter & param);
protected:
	static double Determinant (const XSI::MATH::CMatrix4 & mat);
protected:

	struct sgpuVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;

		bool operator < (const sgpuVertex& v2) const 
		{  
			const float* p = &x;
			const float* p2 = &v2.x;
			for (int i = 0; i < 8; i++)
			{
				if (p[i] != p2[i]) return (p[i] < p2[i]);      
			}
			return false;  
		}

	};

	struct sgpuMaterialInfo
	{
		XSI::CString         name;
		XSI::MATH::CVector3f diffuse;
		XSI::MATH::CVector3f specular;
		float                opacity;
		float                shininess;
		XSI::CString         diffuseTexture;
	};

	static const XSI::MATH::CMatrix4	m_zaxisUpToYaxisUp;
	static const XSI::MATH::CMatrix4	m_invZaxisUpToYaxisUp;


	XSI::CRefArray					m_selection;


protected:
	GXBExporter &operator=( const GXBExporter &other ) { return *this; }
public:
	GXBExportDoc					&m_exportDoc;	
};
#endif // GXBEXPORTER