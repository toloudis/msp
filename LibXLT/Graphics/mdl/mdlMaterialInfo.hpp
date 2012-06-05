/*****************************************************************************
**	mdlMaterialInfo.hpp
**
**		mdlMaterialInfo represents a material for a surface. 
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MATERIALINFO_HPP
#error mdlMaterialInfo.hpp multiply included
#endif
#define MDL_MATERIALINFO_HPP

#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/eff/effShaderParams.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>


//============================================================================
// forward declarations
//============================================================================
class effDisplacementData;
class effGlowData;
class effNormalsData;
class effOutlineData;
class effPhongData;
class effReflectionMap;
class effShaderData;
class effUVTransform;
class effTextureFilterData;
class effRendermanOverrideData;
class prtyProperty;
struct g2dPixelR8G8B8;


//============================================================================
//============================================================================
class mdlMaterialInfo
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mdlMaterialInfo();
		mdlMaterialInfo(const mdlMaterialInfo& i_CopyFrom);

		//--------------------------------------------------------------------
		// Assignment operator clones shader data
		//--------------------------------------------------------------------
		mdlMaterialInfo& operator = (const mdlMaterialInfo& i_CopyFrom);

		//--------------------------------------------------------------------
		//	Material Name
		//--------------------------------------------------------------------
		void SetMaterialName(const std::string& i_Name);
		inline const std::string& GetMaterialName() const;

		//--------------------------------------------------------------------
		// Return number of layers in this material
		//--------------------------------------------------------------------
		inline int GetNumMaterialLayers() const;

		//--------------------------------------------------------------------
		// Add a layer to this material with the given data.
		//--------------------------------------------------------------------
		void AddMaterialLayer(const shared_ptr<effShaderParams>& i_ShaderParams,
							  effUVTransform& i_UVTransform);

		//--------------------------------------------------------------------
		//	Shader Info
		//--------------------------------------------------------------------
		shared_ptr<effShaderParams> GetShaderParams(int i_MaterialLayer = 0) const;
		fsLocator GetShader(int i_MaterialLayer = 0) const;

		//--------------------------------------------------------------------
		//	Set shader name and data at same time
		//--------------------------------------------------------------------
		void SetShaderParams(shared_ptr<effShaderParams> i_Params,
									int i_MaterialLayer = 0);
		
		//--------------------------------------------------------------------
		//	UV Transform Info
		//--------------------------------------------------------------------
		const effUVTransform& GetUVTransform(int i_MaterialLayer = 0) const;
		effUVTransform& UVTransform(int i_MaterialLayer = 0);

		//--------------------------------------------------------------------
		//	Glow Shader Info
		//--------------------------------------------------------------------
		bool GetHasGlow() const;
		void SetHasGlow(bool i_bHasGlow);
		const effGlowData& GetGlowParams() const;
		effGlowData& GlowParams();

		//--------------------------------------------------------------------
		//	Normals Shader Info
		//--------------------------------------------------------------------
		const effNormalsData& GetNormalsParams() const;
		effNormalsData& NormalsParams();

		//--------------------------------------------------------------------
		//	Texture Filter Shader Info
		//--------------------------------------------------------------------
		const effTextureFilterData& GetTextureFilterParams() const;
		effTextureFilterData& TextureFilterParams();

		//--------------------------------------------------------------------
		//	Outline Shader Info
		//--------------------------------------------------------------------
		bool GetHasOutline() const;
		void SetHasOutline(bool i_bHasOutline);

		const effOutlineData& GetOutlineParams() const;
		effOutlineData& OutlineParams();

		bool IsOutlineVisible() const;					//for visibility of the UI outline category
		void SetOutlineVisible( bool i_bVisible );

		//--------------------------------------------------------------------
		//	Material Library Info
		//--------------------------------------------------------------------
		inline bool UsesMaterialLibrary() const;
		inline const fsLocator& GetLibraryFilename() const;
		void SetLibraryFilename(const fsLocator& i_Locator);
		
		//--------------------------------------------------------------------
		//	Reflection Shader Info
		//--------------------------------------------------------------------
		bool GetHasReflection() const;
		void SetHasReflection(bool i_bHasReflection);
		const effReflectionMap& GetReflectionParams() const;
		effReflectionMap& ReflectionParams();

		//--------------------------------------------------------------------
		//	Displacement Shader Info
		//--------------------------------------------------------------------
		bool GetHasDisplacement() const;
		void SetHasDisplacement(bool i_bHasDisplacement);
		const effDisplacementData& GetDisplacementParams() const;
		effDisplacementData& DisplacementParams();

		//--------------------------------------------------------------------
		//	Renderman Override Shader Info
		//--------------------------------------------------------------------
		bool GetHasRendermanOverride() const;
		void SetHasRendermanOverride(bool i_bHasRendermanOverrideData);
		const effRendermanOverrideData& GetRendermanOverrideParams() const;
		effRendermanOverrideData& RendermanOverrideParams();

		//--------------------------------------------------------------------
		//	Set/Get if the material is active
		//--------------------------------------------------------------------
		void SetActive(bool i_bIsActive);
		bool GetActive();

		//--------------------------------------------------------------------
		//	In/Decrease material reference count
		//--------------------------------------------------------------------
		void IncreaseRefCount();
		void DecreaseRefCount();

		//--------------------------------------------------------------------
		//	Get material reference count
		//--------------------------------------------------------------------
		int GetRefCount();

	private:
		// Material name used to attach materials to surfaces
		std::string m_MaterialName;

		struct sMaterialLayer
		{
			// Shader name and shader data for the main material effect
			shared_ptr<effShaderParams> m_ShaderParams;
			effUVTransform m_UVTransform;
		};

		// material layers, always at least one
		std::vector<sMaterialLayer> m_MaterialLayers;
		
		// Glow data. If pointer is null, then no glow for this material
		bool m_bHasGlow;
		shared_ptr<effGlowData> m_GlowData;
		
		// Normal map data. If pointer is null, then no normal map for this material
		shared_ptr<effNormalsData> m_NormalsData;
		shared_ptr<effTextureFilterData> m_TextureFilterData;

		// Outline data. If pointer is null, then no outline for this material
		bool m_bHasOutline;
		bool m_bOutlineVisible;
		shared_ptr<effOutlineData> m_OutlineData;

		// Reflection map data. If pointer is null, then no reflection for this material
		bool m_bHasReflection;
		shared_ptr<effReflectionMap> m_ReflectionData;

		// Displacement data. If pointer is null, then no displacement for this material
		bool m_bHasDisplacement;
		shared_ptr<effDisplacementData> m_DisplacementData;

		// RendermanOverride data. If pointer is null, then no Renderman Override for this material
		bool m_bHasRendermanOverride;
		shared_ptr<effRendermanOverrideData> m_RendermanOverrideData;

		// material library path to .mtl file (relative from root of material library),
		// can be empty to represent a local material
		fsLocator m_LibraryFilename;

		bool m_bIsActive;
		int m_RefCount;

};

//--------------------------------------------------------------------
//	Material Name
//--------------------------------------------------------------------
inline const std::string& mdlMaterialInfo::GetMaterialName() const
{
	return m_MaterialName;
}

//--------------------------------------------------------------------
// Return number of layers in this material
//--------------------------------------------------------------------
inline int mdlMaterialInfo::GetNumMaterialLayers() const
{
	return m_MaterialLayers.size();
}

//--------------------------------------------------------------------
//	Material Library Info
//--------------------------------------------------------------------
inline bool mdlMaterialInfo::UsesMaterialLibrary() const
{
	return (m_LibraryFilename.GetNumNames() > 0);
}
inline const fsLocator& mdlMaterialInfo::GetLibraryFilename() const
{
	return m_LibraryFilename;
}

