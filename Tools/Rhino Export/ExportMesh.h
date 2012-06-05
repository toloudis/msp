#ifndef _EXPORTMESH_H_
#define _EXPORTMESH_H_

#include "sgpuMatrix.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMesh.hpp"
#include "sgpuNode.hpp"
#include "sgpuModelExportScene.hpp"

#include "Log.h"
#include "AutoPtr.h"



typedef std::map<long, AutoPtr<sgpuMaterial> > MatMap;

struct MatItem
{
	int num;

	bool Compare( CRhinoMaterialTable& table, int n );
};

typedef std::multimap<ON_wString, MatItem> RhinoMatMap;

struct DefMat
{
	sgpuMaterial m_def_mat;

	DefMat( const sgpuMaterial& m ):m_def_mat(m)
	{
		m_def_mat.SetDiffuseColor( 0.5f, 0.5f, 0.5f );
		m_def_mat.SetOpacity( 1.f );
		m_def_mat.SetSpecularColor( 0.0f, 0.0f, 0.0f );
		m_def_mat.SetShininess( 0.f );
	}
};

class ExportMesh
{
public:
	bool AddMesh(CRhinoDoc& doc, const CRhinoObjectMesh& mesh, Log& log);

	bool StartExport(const wchar_t* filename);
	bool FinishExport();

	ExportMesh();
	~ExportMesh();

private:
	sgpuScene scene;

	ON_wString m_filename;

	std::set<std::wstring> m_onames;
	std::set<std::wstring> m_matnames;

	MatMap m_materials;
	RhinoMatMap m_mreplaces;

	std::wstring MakeUniqueName(std::wstring name, bool isMaterial);

	DefMat* m_def_mat;

	void AssignDefMat( sgpuMesh& mesh );
	int CheckMaterial( CRhinoMaterialTable& table, int num );
};


#endif