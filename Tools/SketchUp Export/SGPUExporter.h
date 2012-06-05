#ifndef _SGPUEXPORTER_H_
#define _SGPUEXPORTER_H_


#include "sgpuMatrix.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMesh.hpp"
#include "sgpuNode.hpp"
#include "sgpuModelExportScene.hpp"

#include "Log.h"
#include "AutoPtr.h"
#include "InheritanceManager.h"

#define MERGE_FACES

typedef std::map<long, AutoPtr<sgpuMaterial> > MatMap;


struct MeshPart
{
	long matId;

	std::vector<int> indices;
	std::vector<float> pos, norm, uv;
};

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

#ifdef MERGE_FACES
typedef std::map<long, MeshPart> MeshMap;
#else
typedef std::vector<MeshPart> MeshMap;
#endif

class SGPUExporter
{
public:
	SGPUExporter();
	~SGPUExporter();

	bool DoExport(const wchar_t* fileName, IUnknown* activeDocument, IProgressCB* pCB);

	std::wstring MakeUniqueName(std::wstring name, bool isMaterial);
	sgpuScene& Scene() { return m_scene; }

	std::wstring GetStats();

private:
	sgpuScene m_scene;
	DefMat * m_def_mat;

	ISkpDocument* m_document;
    IProgressCB* m_progressBar;

	std::wstring m_filename;
	Log m_log;

	std::set<std::wstring> m_onames;
	std::set<std::wstring> m_matnames;

	std::map<long, bool> m_used;

	MatMap m_materials;
	MeshMap m_meshes;

	CInheritanceManager m_InheritanceManager;
	CComPtr<ISkpTextureWriter2> m_pTextureWriter;

	std::wstring m_currentName;
	int m_ocount, m_icount, m_ncount, m_vcount, m_tcount, m_ecount;

	bool m_isOk;

	bool StartExport();
	bool ExportScene();
	bool FinilizeExport();

	long GetEntityId(IUnknown* pUnk);

	//Materials
	void LoadMaterial(CComPtr<ISkpMaterial> pMaterial);
	void CheckLoadMaterial(CComPtr<ISkpMaterial> pMaterial, long matID);
	void LoadMaterials();
	sgpuMaterial& GetMaterial(long id);
	sgpuMaterial& GetDefMat();

	//
	bool WriteGeometry(); //return false if aborted or errors
	void WriteHierachy(CComPtr<ISkpEntityProvider> pEntProvider, int level);
	void ComputeHierachy(CComPtr<ISkpEntityProvider> pEntProvider, int level, bool& used);
	void WriteFace(CComPtr<ISkpFace> pFace);

	//Mesh operations
	void ClearMeshes() { m_meshes.clear(); }
	void AddMesh( long matId, ISkpPolygonMesh* mesh, ISkpUVHelper* uvHelper, bool isFront, bool inverTri, bool invertNormals );
	void WriteMeshes();

	//
	bool IsVisible( IUnknown* obj );
	bool IsVisibleN( IUnknown* obj );
};


#endif