/****************************************************************************\
**  iscnSceneImport.hpp
**
**      iscnSceneImport reads a scene file into an iscnSceneDesc through
**	whichever format implementation recognizes it (as entImport does for
**	model files). Formats register an iscnSceneImportImpl when their
**	package starts (gltfPackage: .gltf, .glb).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef ISCN_SCENEIMPORT_HPP
#error iscnSceneImport.hpp multiply included
#endif
#define ISCN_SCENEIMPORT_HPP

#include <string>

class fsLocator;
struct iscnSceneDesc;

//----------------------------------------------------------------------------
//	Settings of the scene the objects come into
//----------------------------------------------------------------------------
struct iscnSceneSettings
{
	float m_RenderAspect = 1;	// render output width / height (for cameras whose file gives none)
};

//============================================================================
//	A format
//============================================================================
class iscnSceneImportImpl
{
public:
	virtual ~iscnSceneImportImpl() {}

	//------------------------------------------------------------------------
	// Is this a file of this format?
	//------------------------------------------------------------------------
	virtual bool IsRecognized(const fsLocator& i_File) const = 0;

	//------------------------------------------------------------------------
	// The file name patterns of this format, ';'-separated ("*.gltf;*.glb")
	//------------------------------------------------------------------------
	virtual std::string GetFilePatterns() const = 0;

	//------------------------------------------------------------------------
	// Read a recognized file; false if it can't be read
	//------------------------------------------------------------------------
	virtual bool Read(const fsLocator& i_File, const iscnSceneSettings& i_Settings, iscnSceneDesc& o_Scene) = 0;
};

//============================================================================
//============================================================================
namespace iscnSceneImport
{
	//------------------------------------------------------------------------
	// Add a format (ownership passes here) or remove (and delete) one
	//------------------------------------------------------------------------
	void AddImplementation(iscnSceneImportImpl* i_pImplementation);
	void RemoveImplementation(iscnSceneImportImpl* i_pImplementation);

	//------------------------------------------------------------------------
	// Is this a file of a registered format?
	//------------------------------------------------------------------------
	bool IsRecognized(const fsLocator& i_File);

	//------------------------------------------------------------------------
	// Every registered format's patterns, ';'-separated; empty if none
	//------------------------------------------------------------------------
	std::string GetFilePatterns();

	//------------------------------------------------------------------------
	// Read the file with the format that recognizes it; false if none
	// does or it can't be read (logged)
	//------------------------------------------------------------------------
	bool Read(const fsLocator& i_File, const iscnSceneSettings& i_Settings, iscnSceneDesc& o_Scene);
}
