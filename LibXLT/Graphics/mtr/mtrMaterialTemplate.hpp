/*****************************************************************************
**  mtrMaterialTemplate.h
**
**      The mtrMaterialTemplate represents all intensive visual properties of a
**	surface.  It includes information about the ambient, diffuse, and
**	specular colors, and the texture maps applied to the surface.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#error "This file, mtrMaterialTemplate.h, is obsolete"

//#ifdef MTR_MATERIALTEMPLATE_HPP
//#error mtrMaterialTemplate.hpp multiply included
//#endif
//#define MTR_MATERIALTEMPLATE_HPP
//
//
//#ifndef MTR_TEXTURELAYER_HPP
//#include "Graphics/mtr/mtrTextureLayer.hpp"
//#endif
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef FS_LOCATOR_HPP
//#include "Core/fs/fsLocator.hpp"
//#endif
//#ifndef MA_FLOATRGBA_HPP
//#include "Core/ma/maFloatRGBA.hpp"
//#endif
//#ifndef MA_VECTOR4D_HPP
//#include "Core/ma/maVector4d.hpp"
//#endif
//#ifndef MA_MATRIX4X4_HPP
//#include "Core/ma/maMatrix4x4.hpp"
//#endif
//
//#include <vector>
//#include <string>
//#include <map>
//
//#ifndef MAT_SHADEREFFECT_HPP
//#include "Graphics/mat/matShaderEffect.hpp"
//#endif
//
//#ifndef EFF_FURDATA_HPP
//#include "Graphics/eff/effFurData.hpp"
//#endif
//#ifndef EFF_GLOWDATA_HPP
//#include "Graphics/eff/effGlowData.hpp"
//#endif
//
//struct mtrMatAnimInfo
//{
//	enum MatComponent
//	{
//		e_Ambient = 0,
//		e_Diffuse,
//		e_Specular,
//		e_Emissive,
//		e_SpecularPower,
//		e_TextureTranslation,
//		e_TextureScale,
//		e_TextureRotation
//	};
//
//	mtrMatAnimInfo()
//		: m_TextureLayer(0), m_bLooping(true), m_bReversing(true), m_bInitialActive(true)
//	{ }
//
//	MatComponent m_Component;
//	int	m_TextureLayer;
//	bool m_bLooping;	
//	bool m_bReversing;
//	bool m_bInitialActive;
//
//	std::vector<float> m_KeyTimes;
//	std::vector<float> m_KeyData;
//};
//
//struct mtrShaderParams
//{
//	enum ParamType
//	{
//		e_float = 0,
//		e_vector,
//		e_matrix,
//		e_texture
//	};
//	// on i/o these have to be ready for shader changes...
//	// drop variables that are not recognized by the shader 
//	// or are the wrong type
//	std::string m_shaderName;
//	std::map<std::string, maVector4d> m_vectors;
//	std::map<std::string, float> m_floats;
//	std::map<std::string, maMatrix4x4> m_matrices;
//	std::map<std::string, std::string> m_textures;
//	std::map<std::string, bool> m_bools;
//	std::map<std::string, std::string> m_strings;
//
//	bool GetFloat(const std::string& i_s, float& o_value) const
//	{
//		std::map<std::string, float>::const_iterator it = m_floats.find(i_s); 
//		if (it != m_floats.end()) 
//			o_value = (*it).second;
//		else
//			o_value = 0.0;
//		return (it != m_floats.end());
//	}
//	bool GetVector(const std::string& i_s, maVector4d& o_value) const
//	{
//		std::map<std::string, maVector4d>::const_iterator it = m_vectors.find(i_s); 
//		if (it != m_vectors.end()) 
//			o_value = (*it).second;
//		else
//			o_value = maVector4d(0,0,0,0);
//		return (it != m_vectors.end());
//	}
//	bool GetBool(const std::string& i_s, bool& o_value) const
//	{
//		std::map<std::string, bool>::const_iterator it = m_bools.find(i_s); 
//		if (it != m_bools.end()) 
//			o_value = (*it).second;
//		else
//			o_value = false;
//		return (it != m_bools.end());
//	}
//	bool GetString(const std::string& i_s, std::string& o_value) const
//	{
//		std::map<std::string, std::string>::const_iterator it = m_strings.find(i_s); 
//		if (it != m_strings.end()) 
//			o_value = (*it).second;
//		else
//			o_value = "";
//		return (it != m_strings.end());
//	}
//	bool GetTexture(const std::string& i_s, std::string& o_value) const
//	{
//		std::map<std::string, std::string>::const_iterator it = m_textures.find(i_s); 
//		if (it != m_textures.end()) 
//			o_value = (*it).second;
//		else
//			o_value = "";
//		return (it != m_textures.end());
//	}
//	void Clear()
//	{
//		m_shaderName = "";
//		m_vectors.clear();
//		m_floats.clear();
//		m_matrices.clear();
//		m_textures.clear();
//		m_bools.clear();
//		m_strings.clear();
//	}
//};
//
//class mtrMaterialTemplate
//{
//	public:
//		mtrMaterialTemplate()
//			: m_HasFur(false), m_HasGlow(false),
//				m_ShaderData(NULL)
//		{}
//		inline mtrMaterialTemplate(const mtrMaterialTemplate& i_CopyFrom);
//		inline mtrMaterialTemplate& operator = (const mtrMaterialTemplate& i_CopyFrom);
//		~mtrMaterialTemplate()
//		{
//			delete m_ShaderData;
//		}
//
//		//====================================================================
//		//	Material Name
//		//====================================================================
//		inline void SetMaterialName(const std::string& i_Name);
//		inline const std::string& GetMaterialName() const;
//
//		//====================================================================
//		//	GetHasSpecular returns true if the material should be rendered
//		//	with a specular highlight.
//		//====================================================================
//		inline bool GetHasSpecular() const;
//
//		//====================================================================
//		//	SetHasSpecular allows the user to specify if the material should
//		//	be rendered with a specular highlight.
//		//====================================================================
//		inline void SetHasSpecular(bool i_Specular);
//
//		//====================================================================
//		//	GetTextureLayer returns reference to texture layer with index
//		//====================================================================
//		inline const mtrTextureLayer& GetTextureLayer(int i_Index) const;
//		inline mtrTextureLayer& GetTextureLayer(int i_Index);
//		inline const mtrTextureLayer* GetTextureLayerByType(mtrTextureLayer::TextureType i_Type) const;
//		inline mtrTextureLayer* GetTextureLayerByType(mtrTextureLayer::TextureType i_Type);
//		
//		//====================================================================
//		//	AddSimpleTextureLayer adds basic layer using
//		//	the name of the texture
//		//====================================================================
//		inline void AddSimpleTextureLayer(const std::string& i_TextureName);
//
//		//====================================================================
//		//	TextureLayer management
//		//====================================================================
//		inline int	GetNumTextureLayers() const;
//		inline void AddTextureLayer(const mtrTextureLayer& i_Layer);
//		inline void InsertTextureLayer(const mtrTextureLayer& i_Layer, int i_Index);
//		inline void RemoveTextureLayer(int i_Index);
//		inline void RemoveAllTextures();
//		inline bool HasBumpMap() const;
//
//		//====================================================================
//		//	Material Animation management
//		//====================================================================
//		inline int	GetNumMatAnims() const;
//		inline const mtrMatAnimInfo& GetMatAnim(int i_Index) const;
//		inline mtrMatAnimInfo& GetMatAnim(int i_Index);
//		inline void AddMatAnim(const mtrMatAnimInfo& i_Anim);
//		inline void RemoveMatAnim(int i_Index);
//		inline void RemoveAllMatAnims();
//
//		//====================================================================
//		//	Shader Info
//		//====================================================================
//		inline const effShaderData* GetShaderData() const;
//		inline effShaderData* ShaderData();
//		inline void SetShader(std::string i_Shader, effShaderData* i_Data = NULL);
//		inline std::string GetShader() const;
//
//		//====================================================================
//		//	Fur Shader Info
//		//====================================================================
//		inline void SetHasFur(bool i_val);
//		inline bool GetHasFur() const;
//
//		inline const effFurData& GetFurParams() const;
//		inline effFurData& FurParams();
//
//		//====================================================================
//		//	Glow Shader Info
//		//====================================================================
//		inline void SetHasGlow(bool i_val);
//		inline bool GetHasGlow() const;
//
//		inline const effGlowData& GetGlowParams() const;
//		inline effGlowData& GlowParams();
//
//		//====================================================================
//		//	Material Library Info
//		//====================================================================
//		inline bool UsesMaterialLibrary() const;
//		inline const fsLocator& GetLibraryFilename() const;
//		inline void SetLibraryFilename(const fsLocator& i_Locator);
//		
//		
//	private:
//
//		struct
//		{
//			bool m_bHasSpecular		: 1;
//		} m_Flags;
//
//		std::string m_MaterialName;
//		//std::string m_TextureName;
//		std::vector<mtrTextureLayer> m_Textures;
//		std::vector<mtrMatAnimInfo> m_MatAnims;
//
//		std::string m_Shader;
//		effShaderData* m_ShaderData;
//		
//		bool m_HasFur;
//		effFurData m_FurData;
//
//		bool m_HasGlow;
//		effGlowData m_GlowData;
//
//		// material library path to .mtl file (relative from root of material library),
//		// can be empty to represent a local material
//		fsLocator m_LibraryFilename;
//
//};
//
////====================================================================
////	Material Name
////====================================================================
//inline void mtrMaterialTemplate::SetMaterialName(const std::string& i_Name)
//{
//	m_MaterialName = i_Name;
//}
//inline const std::string& mtrMaterialTemplate::GetMaterialName() const
//{
//	return m_MaterialName;
//}
//
////====================================================================
////	GetHasSpecular returns true if the material should be rendered
////	with a specular highlight.
////====================================================================
//inline bool mtrMaterialTemplate::GetHasSpecular() const
//{
//	return m_Flags.m_bHasSpecular;
//}
//
////====================================================================
////	SetHasSpecular allows the user to specify if the material should
////	be rendered with a specular highlight.
////====================================================================
//inline void mtrMaterialTemplate::SetHasSpecular(bool i_Specular)
//{
//	m_Flags.m_bHasSpecular = i_Specular;
//}
//
////====================================================================
////	GetTextureLayer returns reference to texture layer with index
////====================================================================
//inline const mtrTextureLayer& mtrMaterialTemplate::GetTextureLayer(int i_Index) const
//{
//	return m_Textures[i_Index];
//}
//inline mtrTextureLayer& mtrMaterialTemplate::GetTextureLayer(int i_Index)
//{
//	return m_Textures[i_Index];
//}
//inline const mtrTextureLayer* mtrMaterialTemplate::GetTextureLayerByType(mtrTextureLayer::TextureType i_Type) const
//{
//	for (int i = 0; i < m_Textures.size(); i++)
//	{
//		if (m_Textures[i].GetTextureType() == i_Type)
//			return &m_Textures[i];
//	}
//	return NULL;
//}
//inline mtrTextureLayer* mtrMaterialTemplate::GetTextureLayerByType(mtrTextureLayer::TextureType i_Type)
//{
//	for (int i = 0; i < m_Textures.size(); i++)
//	{
//		if (m_Textures[i].GetTextureType() == i_Type)
//			return &m_Textures[i];
//	}
//	return NULL;
//}
//
////====================================================================
////	AddSimpleTextureLayer adds basic layer using
////	the name of the texture
////====================================================================
//inline void mtrMaterialTemplate::AddSimpleTextureLayer(const std::string& i_TextureName)
//{
//	mtrTextureLayer layer;
//	layer.SetTextureName(i_TextureName);
//
//	// Check to see if this is a normal map. If so,
//	// set to use bump map blending
//	const char* ptr = ::strrchr(i_TextureName.c_str(), '.');
//	if (ptr && !_stricmp(ptr, ".nmp"))
//		layer.SetTextureType(mtrTextureLayer::e_BumpHeight);
//	else if (ptr && !_stricmp(ptr, ".scm"))
//		layer.SetTextureType(mtrTextureLayer::e_EnvMap);
//
//	m_Textures.push_back(layer);
//}
//
////====================================================================
////	GetNumTextureLayers 
////====================================================================
//inline int	mtrMaterialTemplate::GetNumTextureLayers() const
//{
//	return m_Textures.size();
//}
//
//inline bool mtrMaterialTemplate::HasBumpMap() const
//{
//	for (int i = 0; i < m_Textures.size(); i++)
//	{
//		if (m_Textures[i].GetTextureType() == mtrTextureLayer::e_BumpHeight)
//			return true;
//	}
//	return false;
//}
//
////====================================================================
////	AddTextureLayer 
////====================================================================
//inline void mtrMaterialTemplate::AddTextureLayer(const mtrTextureLayer& i_Layer)
//{
//	m_Textures.push_back(i_Layer);
//}
//
////====================================================================
////	InsertTextureLayer 
////====================================================================
//inline void mtrMaterialTemplate::InsertTextureLayer(const mtrTextureLayer& i_Layer,
//												   int i_Index)
//{
//	m_Textures.insert(m_Textures.begin() + i_Index, i_Layer);
//}
//
////====================================================================
////	RemoveTextureLayer 
////====================================================================
//inline void mtrMaterialTemplate::RemoveTextureLayer(int i_Index)
//{
//	m_Textures.erase(m_Textures.begin() + i_Index);
//}
//
////====================================================================
////	RemoveAllTextures 
////====================================================================
//inline void mtrMaterialTemplate::RemoveAllTextures()
//{
//	m_Textures.clear();
//}
//
////====================================================================
////	Material Animation management
////====================================================================
//inline int	mtrMaterialTemplate::GetNumMatAnims() const
//{
//	return m_MatAnims.size();
//}
//inline const mtrMatAnimInfo& mtrMaterialTemplate::GetMatAnim(int i_Index) const
//{
//	return m_MatAnims[i_Index];
//}
//inline mtrMatAnimInfo& mtrMaterialTemplate::GetMatAnim(int i_Index)
//{
//	return m_MatAnims[i_Index];
//}
//inline void mtrMaterialTemplate::AddMatAnim(const mtrMatAnimInfo& i_Anim)
//{
//	m_MatAnims.push_back(i_Anim);
//}
//inline void mtrMaterialTemplate::RemoveMatAnim(int i_Index)
//{
//	m_MatAnims.erase(m_MatAnims.begin() + i_Index);
//}
//inline void mtrMaterialTemplate::RemoveAllMatAnims()
//{
//	m_MatAnims.clear();
//}
//
////====================================================================
////	Shader Info
////====================================================================
//inline const effShaderData* mtrMaterialTemplate::GetShaderData() const
//{
//	return m_ShaderData;
//}
//inline effShaderData* mtrMaterialTemplate::ShaderData()
//{
//	return m_ShaderData;
//}
//inline std::string mtrMaterialTemplate::GetShader() const
//{
//	return m_Shader;
//}
//inline void mtrMaterialTemplate::SetShader(std::string i_Shader, effShaderData* i_Data)
//{
//	m_Shader = i_Shader;
//
//	if (i_Data)
//	{	
//		delete m_ShaderData;
//		m_ShaderData = NULL;
//		m_ShaderData = i_Data->Clone();
//	}
//}
//
//
////====================================================================
////	Fur Shader Info
////====================================================================
//inline void mtrMaterialTemplate::SetHasFur(bool i_val)
//{
//	m_HasFur = i_val;
//}
//inline bool mtrMaterialTemplate::GetHasFur() const
//{
//	return m_HasFur;
//}
//inline const effFurData& mtrMaterialTemplate::GetFurParams() const
//{
//	return m_FurData;
//}
//inline effFurData& mtrMaterialTemplate::FurParams()
//{
//	return m_FurData;
//}
//
////====================================================================
////	Glow Shader Info
////====================================================================
//inline void mtrMaterialTemplate::SetHasGlow(bool i_val)
//{
//	m_HasGlow = i_val;
//}
//inline bool mtrMaterialTemplate::GetHasGlow() const
//{
//	return m_HasGlow;
//}
//inline const effGlowData& mtrMaterialTemplate::GetGlowParams() const
//{
//	return m_GlowData;
//}
//inline effGlowData& mtrMaterialTemplate::GlowParams()
//{
//	return m_GlowData;
//}
//
////====================================================================
////	Material Library Info
////====================================================================
//inline bool mtrMaterialTemplate::UsesMaterialLibrary() const
//{
//	return (m_LibraryFilename.GetNumNames() > 0);
//}
//inline const fsLocator& mtrMaterialTemplate::GetLibraryFilename() const
//{
//	return m_LibraryFilename;
//}
//inline void mtrMaterialTemplate::SetLibraryFilename(const fsLocator& i_Locator)
//{
//	m_LibraryFilename = i_Locator;
//}
//
////====================================================================
////====================================================================
//mtrMaterialTemplate::mtrMaterialTemplate(const mtrMaterialTemplate& i_CopyFrom)
//: m_ShaderData(NULL)
//{
//	(*this) = i_CopyFrom;
//}
//mtrMaterialTemplate& mtrMaterialTemplate::operator = (const mtrMaterialTemplate& i_CopyFrom)
//{
//	if (&i_CopyFrom == this)
//		return *this;
//
//	m_Flags = i_CopyFrom.m_Flags;
//
//	m_MaterialName = i_CopyFrom.m_MaterialName;
//
//	m_Textures = i_CopyFrom.m_Textures;
//	m_MatAnims = i_CopyFrom.m_MatAnims;
//
//	m_Shader = i_CopyFrom.m_Shader;
//	if (m_ShaderData)
//	{
//		delete m_ShaderData;
//		m_ShaderData = NULL;
//	}
//	if (i_CopyFrom.m_ShaderData)
//		m_ShaderData = i_CopyFrom.m_ShaderData->Clone();
//	
//	m_HasFur = i_CopyFrom.m_HasFur;
//	m_FurData = i_CopyFrom.m_FurData;
//
//	m_HasGlow = i_CopyFrom.m_HasGlow;
//	m_GlowData = i_CopyFrom.m_GlowData;
//
//	m_LibraryFilename = i_CopyFrom.m_LibraryFilename;
//
//	return *this;
//}
