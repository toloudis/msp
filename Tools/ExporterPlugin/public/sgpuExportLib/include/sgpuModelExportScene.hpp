/****************************************************************************\
**  sgpuModelExportScene.hpp
**
**      sgpuModelExportScene.hpp defines class for a scene that can be exported
**	to a StudioGPU static geometry file (.gxb)
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_MODELEXPORTSCENE_HPP
#define SGPU_MODELEXPORTSCENE_HPP
#include "sgpuExportLib.hpp"
#include "sgpuString.hpp"
#include "sgpuBaseScene.hpp"


//============================================================================
//============================================================================
struct sgpuModelExportSceneImpl;
class sgpuNode;
class sgpuMaterial;

//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuModelExportScene : public sgpuBaseScene
{
public:
	//========================================================================
	//	default constructor
	//========================================================================
	sgpuModelExportScene();

	//========================================================================
	//	disallowed
	//========================================================================
	sgpuModelExportScene( const sgpuModelExportScene &);
	sgpuModelExportScene & operator=( const sgpuModelExportScene &);
	//========================================================================
	//	destructor
	//========================================================================
	~sgpuModelExportScene();

	//========================================================================
	// Write the scene's contents to the given filename, which should
	// have the extension ".gxb". Version string is written to 
	// file to track how the geometry file was generated.
	// Returns true if successful.
	//========================================================================
	bool WriteScene(const sgpuString& i_Filename,
		const sgpuString& i_VersionString);
	//========================================================================
	// Read the scene's content from a gxb file into the scene.
	// Any other hierarchical content is lost.
	//========================================================================
	bool ReadScene( const sgpuString& i_Filename);
	//========================================================================
	// Returns handle to the root node that is built into scene. With this 
	// node, you can then build a hieararchy and add meshes. 
	//========================================================================
	sgpuNode GetRootNode();

	//========================================================================
	// Materials are shared in the scene and must be uniquely named.
	// Create materials here and then assign them to sgpuMesh objects.
	// If you call this function with the same name twice, it will return
	// a handle to the first material instead of creating a new material.
	// The number of materials in the scene are returned by 'GetNumMaterials'.
	//For each material, in the scene, there is an id 
	//========================================================================
	//Creates a material in the scene with the name supplied.
	//or, if such a material already exist, return the nmaterial.
	sgpuMaterial CreateMaterial(const sgpuString& i_MaterialName);
	//returns true if such a material exist already in the scene
	bool IsMaterial( const sgpuString& i_MaterialName ) const;
	//retreives the material with the given name.
	//throws sgpuException, if it cant find one
	sgpuMaterial GetMaterial(const sgpuString& i_MaterialName) const;
	//get the ith material
	//throws sgpuException if i >= GetNumMaterials()
	sgpuMaterial GetMaterial( int i_MtlIdx )const;
	//get the number of materials in the database
	int GetNumMaterials() const;
	//makes an eror material, whose name is given by SGPU_ERROR_MATERIAL_NAME
	void MakeErrorMaterial( );
	//If the original scene had
	//a hierarchy as shown below 
	//				A (Ta)
	//		    ,/		\ 
	//		   /		 '\
	//		  B (Tb)	 C (Tc)
	//				    /  \ 
	//				 D (Td) E(Te)
	// Suppose that A, B, C, and D may contain sgopuMesh-es or
	// sgpuSubdiv nodeContent, where as E 's node content is an
	//sgpuPathReference to D.
	//This will result in the same hierarchy, but
	//with the following results.
	//	A's transform = identity
	//A's geometry, if any is transformed by Ta.
	// B's transform = identity
	//B's geomety = Tb x Ta
	// C's transform = identity
	//C's geometry transformed by Tc x Ta
	// D's transform = identity
	//D's geometry is transformed by Td x Tc x Ta
	// E's transform = Te x Tc x Ta
	void FlattenScene();
	//Do a material merge on the scene.
	//Each node in the hierarchy which contains an sgpuMesh,
	//will be detached from its corresponding parents and its geometry will be merged
	//in world space, based on the materials assigned to it.
	//The resulting merged nodes will be direct children of  the scene root,
	//and will be named (i_Prefix + "_" + materialName )
	//Further, for each deleted node, all ancestor nodes with empty content,
	//(ie no children and no node content) will be deleted.
	//Nodes whose meshes have multple materials will be split according
	//to materials and then merged based on  materials.
	//Meshes containing subdiv surfaces will be left alone.
	//i_Prefix, the merged nodes will be named i_Prefix + "_" + materialname
	//o_numMergesMerged = number of nodes in the scene that were subjected to merge
	//o_numMergeREsults = number of nodes produces as a result of merge
	void MergeByMaterials( sgpuString &i_Prefix,  int &o_numMeshesMerged, int &o_numMergeResults );
	
//	Output scene hierarchy and material table into the 'o_Hierarchy' string
	void Describe( sgpuString &o_Hierarchy );

#if defined( SGPU_SUPPORT_1200)
	//obsolete calls
	sgpuMaterial CreateMaterial(const char* i_MaterialName);
	sgpuMaterial CreateMaterial(const wchar_t* i_MaterialName);
		bool WriteScene(const char* i_Filename,
		const char* i_VersionString);
#endif
		
	bool WriteScene(const wchar_t* i_Filename,
		const wchar_t* i_VersionString);
	
	bool ReadScene( const wchar_t* i_Filename);

private:
	sgpuModelExportSceneImpl *m_pImpl;
};

#if defined( SGPU_SUPPORT_1200)
typedef sgpuModelExportScene sgpuScene;
#endif

#endif // #ifndef SGPU_MODELEXPORTSCENE_HPP