/****************************************************************************\
**	g3dBake.hpp
**
**		g3dBake is an interface for renderers for the current
**	baking implementation. 
**
**	StudioGPU
**	Copyright(C) 2008. - All Rights Reserved
\****************************************************************************/
#ifdef G3D_BAKE_HPP
#error g3dBake.hpp multiply included
#endif
#define G3D_BAKE_HPP

#include <map>
#include <string>
#include <vector>


//============================================================================
//	Forward References
//============================================================================
class camCamera;
class fsLocator;
class g2dPFD;
class g3dBakeImpl;
class g3dFragment;
class g3dScene;
class matRenderTargetTexture;


//============================================================================
//============================================================================
class g3dBake
{
public:
	enum BakeMode
	{
		bk_Color = 0,
		bk_Normals,
		bk_BakeTypeNum
	};

	//--------------------------------------------------------------------
	//	Init scene for baking, return number of nodes to bake
	//--------------------------------------------------------------------
	static int Init(const g2dPFD& i_PFD, const g3dScene* i_pScene, const fsLocator& i_OutputPath, 
		float i_fSimTime, const camCamera& i_Camera, bool i_bIsSaveAndReplace,
		const std::string& i_OutputFormat, int i_Res, int i_TextureReduce);

	static int CleanUp();

	//--------------------------------------------------------------------
	//	Bake a node and return true if there are any left to bake.
	//--------------------------------------------------------------------
	static bool BakeNextNode(std::map<const g3dFragment*, std::string> &io_TextureNameMap);

	//--------------------------------------------------------------------
	//	Set the output directory
	//--------------------------------------------------------------------
	static void SetOutputDirectory(const fsLocator& i_OutputDir);

	//--------------------------------------------------------------------
	// Set new implementation method, returns pointer to last one
	// that was being used.  Both can be NULL.
	// Ownership for the pointer remains with the caller.
	//--------------------------------------------------------------------
	static g3dBakeImpl* SetImplementation(g3dBakeImpl* i_pCreator);

private:
	static g3dBakeImpl* sm_pImplementation;
};


//============================================================================
//============================================================================
class g3dBakeImpl
{
public:
	//--------------------------------------------------------------------
	//	Init scene for baking, return number of nodes to bake
	//--------------------------------------------------------------------
	virtual int Init(const g2dPFD& i_PFD, const g3dScene* i_pScene, const fsLocator& i_OutputPath,
		float i_fSimTime, const camCamera& i_Camera, bool i_bIsSaveAndReplace,
		const std::string& i_OutputFormat, int i_Res, int i_TextureReduce) = 0;

	virtual int CleanUp() = 0;

	//--------------------------------------------------------------------
	//	Bake a node and return true if there are any left to bake.
	//--------------------------------------------------------------------
	virtual bool BakeNextNode(std::map<const g3dFragment*, std::string> &io_TextureNameMap) = 0;

	//--------------------------------------------------------------------
	//	Set the output directory
	//--------------------------------------------------------------------
	virtual void SetOutputDirectory(const fsLocator& i_OutputDir) = 0;
};
