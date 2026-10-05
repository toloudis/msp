/****************************************************************************\
**  matShaderMgr.hpp
**
**      matShaderMgr manages an array of shaders that chooses the 
**	appropriate shader based on the combination of texture layers.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_SHADERMGR_HPP
#error matShaderMgr.hpp multiply included
#endif
#define MAT_SHADERMGR_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

#include <list>
#include <map>
#include <string>
#include <vector>

class effShaderData;
class fsLocator;
class matMaterial;
class matShaderEffect;

struct matShaderInfo
{
	itString m_Name;
	std::string m_UIName;
	std::string m_HelpString;
	bool m_bSupportsOutline;
	// effShaderData is for built-in shaders. other material shaders use effShaderParams.
	effShaderData* m_DataTemplate;

	matShaderEffect* m_pEffect;

	matShaderInfo():m_DataTemplate(NULL),m_pEffect(NULL),m_bSupportsOutline(false){}
};

class matShaderMgrImpl
{
public:
	//------------------------------------------------------------------------
	// Load effects from given directory
	//------------------------------------------------------------------------
	virtual void RegisterEffects(const fsLocator &i_ShaderDir,
			std::map<std::string, matShaderInfo>& io_ShaderMap) = 0;
	//------------------------------------------------------------------------
	// List the built-in shaders a user can pick: material shaders, named
	//	"Phong.fx", and post effects, named "Sepia.fx" (they live in the
	//	GetPostShaderFolder() folder). Only the info is filled in; nothing
	//	is loaded (m_pEffect and m_DataTemplate stay NULL).
	//------------------------------------------------------------------------
	virtual void RegisterSelectableShaders(std::vector<matShaderInfo>& o_Materials,
			std::vector<matShaderInfo>& o_PostEffects) = 0;

	virtual void UnloadEffect(const fsLocator& i_PathToShader,
		std::map<fsLocator, matShaderEffect*>& io_ShaderMap) = 0;

	virtual matShaderEffect* LoadEffect(const fsLocator& i_PathToShader,
		std::map<fsLocator, matShaderEffect*>& io_ShaderMap) = 0;

};

namespace matShaderMgr
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetImplementation(matShaderMgrImpl* i_pImpl);

	//------------------------------------------------------------------------
	// Initialize system
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	// Clean up system
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	// Get the main shaders root path
	//------------------------------------------------------------------------
	fsLocator GetDefaultShaderPath();

	//------------------------------------------------------------------------
	// Get the main shaders root path
	//------------------------------------------------------------------------
	std::string GetPostShaderFolder();

	//------------------------------------------------------------------------
	// ResolveShaderPath
	//------------------------------------------------------------------------
	fsLocator ResolveShaderPath(const fsLocator& i_ShaderName);
	
	//------------------------------------------------------------------------
	// GetEffect - return effect for given material. This will return NULL
	//	if no effect is needed.
	//------------------------------------------------------------------------
	matShaderEffect* GetEffect(const matMaterial &i_Material, int i_MaterialLayerIndex = 0);
	matShaderEffect* GetSpecialEffect(const std::string& i_Name);
	matShaderEffect* GetEffect(const fsLocator& i_PathToShader);
	matShaderEffect* GetPostEffect(const fsLocator& i_PathToShader);

	effShaderData* CreateData(const std::string& i_EffectID);

	//------------------------------------------------------------------------
	// The built-in shaders offered in the shader pickers, sorted by name.
	//	A material is chosen by its m_Name ("Phong.fx"); a post effect by
	//	GetPostShaderFolder() followed by its m_Name ("PostEffect\\Sepia.fx").
	//------------------------------------------------------------------------
	const std::vector<matShaderInfo>& GetMaterialShaders();
	const std::vector<matShaderInfo>& GetPostEffectShaders();

	void ReloadShader( matMaterial * i_pMaterial );

	//------------------------------------------------------------------------
	// Turn on/off use of the shader array.  If not enabled, the function
	//	"GetEffect" will return NULL for all materials.
	//------------------------------------------------------------------------
	bool GetUseShaderArray();
	void SetUseShaderArray(bool i_bEnable);

	//------------------------------------------------------------------------
	// True if the last name of the locator names a built-in material
	//	shader ("Phong.fx"), whatever folder precedes it.
	//------------------------------------------------------------------------
	bool IsMachStudioShader(const fsLocator& i_ShaderName);

	class matShaderMapVisitor {
	public:
		virtual void visit(const std::string& key, matShaderEffect* effect) = 0;
	};
	void visitShaders(matShaderMapVisitor* visitor);

}
