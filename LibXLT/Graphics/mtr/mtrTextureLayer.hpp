/*****************************************************************************
**  mtrTextureLayer.h
**
**      mtrTextureLayer represents the properties of a texture
**	layer in a material.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#error "This file, mtrTextureLayer.hpp, is obsolete"

//#ifdef MTR_TEXTURELAYER_HPP
//#error mtrMaterialTemplate.hpp multiply included
//#endif
//#define MTR_TEXTURELAYER_HPP
//

//
//#ifndef MA_VECTOR2D_HPP
//#include "Core/ma/maVector2d.hpp"
//#endif
//
//#ifndef ENV_TYPE_HPP
//#include "Core/env/envType.hpp"
//#endif
//
//#include <string>
//#include <vector>
//
//class mtrTextureLayer
//{
//public:
//	enum LayerType
//	{
//		e_Basic = 0,
//		e_StaticCubeMap
//	};
//	enum TextureType
//	{
//		e_Normal = 0,
//		e_AlphaBlend,
//		e_BumpHeight,
//		e_SpecularMap,
//		e_EnvMap,
//		e_GlossMap
//	};
//
//	//====================================================================
//	//====================================================================
//	inline mtrTextureLayer();
//
//	//====================================================================
//	//====================================================================
//	inline bool operator==(const mtrTextureLayer& i_Layer) const;
//	
//	//====================================================================
//	//	LayerType
//	//====================================================================
//	inline LayerType GetLayerType() const;
//	inline void SetLayerType( LayerType i_Type );
//
//	//====================================================================
//	//	TextureType
//	//====================================================================
//	inline TextureType GetTextureType() const;
//	inline void SetTextureType( TextureType i_Type );
//
//	//====================================================================
//	//	TextureName 
//	//====================================================================
//	inline const std::string& GetTextureName() const;
//	inline void SetTextureName(const std::string& i_TextureName);
//
//	//====================================================================
//	//	Texture transformation values.
//	//====================================================================
//	inline void SetTranslation(const maVector2d& i_Translation);
//	inline void SetScale(const maVector2d& i_Scale);
//	inline void SetRotation(float i_Rot);
//
//	inline const maVector2d& GetTranslation() const;
//	inline const maVector2d& GetScale() const;
//	inline float GetRotation() const;
//
//	//====================================================================
//	//	TextureCoordinateSource
//	//	Index of the texture coordinates used 
//	//====================================================================
//	inline int GetTextureCoordinateSource() const;
//	inline void SetTextureCoordinateSource( int i_CoordinateSetIndex );
//
//	//====================================================================
//	//	TextureNames - list for static cube map 
//	//====================================================================
//	//inline const std::vector<std::string>& GetTextureNames() const;
//	//inline void AddTextureName(const std::string& i_TextureName);
//	//inline void ClearTextureNames();
//
//private:
//	std::string m_TextureName;
//	LayerType	m_LayerType;
//	TextureType	m_TextureType;
//	
//	maVector2d m_Translation;
//	maVector2d m_Scale;
//	float m_Rotation;	// radians
//
//	envType::Int8 m_UVSet;
//	
//	//std::vector<std::string> m_TextureNames;
//};
//
////====================================================================
////====================================================================
//inline mtrTextureLayer::mtrTextureLayer()
//:	m_LayerType(mtrTextureLayer::e_Basic),
//	m_TextureType(mtrTextureLayer::e_Normal),
//	m_Translation(0,0),
//	m_Scale(1,1),
//	m_Rotation(0),
//	m_UVSet(0)
//{
//	
//}
//
////====================================================================
////====================================================================
//inline bool mtrTextureLayer::operator==(const mtrTextureLayer& i_Layer) const
//{
//	return ((m_LayerType == i_Layer.m_LayerType) &&
//			(m_TextureName == i_Layer.m_TextureName) &&
//			(m_TextureType == i_Layer.m_TextureType) &&
//			(m_Translation == i_Layer.m_Translation) &&
//			(m_Scale == i_Layer.m_Scale) &&
//			(m_Rotation == i_Layer.m_Rotation) &&
//			(m_UVSet == i_Layer.m_UVSet));
//}
//
//
////====================================================================
////	LayerType
////====================================================================
//inline mtrTextureLayer::LayerType mtrTextureLayer::GetLayerType() const
//{
//	return m_LayerType;
//}
//inline void mtrTextureLayer::SetLayerType( LayerType i_Type )
//{
//	m_LayerType = i_Type;
//}
//
////====================================================================
////	TextureType
////====================================================================
//inline mtrTextureLayer::TextureType mtrTextureLayer::GetTextureType() const
//{
//	return m_TextureType;
//}
//inline void mtrTextureLayer::SetTextureType( TextureType i_Type )
//{
//	m_TextureType = i_Type;
//}
//
////====================================================================
////	GetTextureName returns the name of the texture
////====================================================================
//inline const std::string& mtrTextureLayer::GetTextureName() const
//{
//	return m_TextureName;
//}
//inline void mtrTextureLayer::SetTextureName(const std::string& i_TextureName)
//{
//	m_TextureName = i_TextureName;
//}
//
////====================================================================
////	Texture transformation values.
////====================================================================
//inline void mtrTextureLayer::SetTranslation(const maVector2d& i_Translation)
//{
//	m_Translation = i_Translation;
//}
//inline void mtrTextureLayer::SetScale(const maVector2d& i_Scale)
//{
//	m_Scale = i_Scale;
//}
//inline void mtrTextureLayer::SetRotation(float i_Rot)
//{
//	m_Rotation = i_Rot;
//}
//
//inline const maVector2d& mtrTextureLayer::GetTranslation() const
//{
//	return m_Translation;
//}
//
//inline const maVector2d& mtrTextureLayer::GetScale() const
//{
//	return m_Scale;
//}
//
//inline float mtrTextureLayer::GetRotation() const
//{
//	return m_Rotation;
//}
//
////====================================================================
////	TextureCoordinateSource
////	Index of the texture coordinates used 
////====================================================================
//inline int mtrTextureLayer::GetTextureCoordinateSource() const
//{
//	return m_UVSet;
//}
//inline void mtrTextureLayer::SetTextureCoordinateSource( int i_CoordinateSetIndex )
//{
//	m_UVSet = i_CoordinateSetIndex;
//}
//
////====================================================================
////	TextureNames - list for static cube map 
////====================================================================
////inline const std::vector<std::string>& mtrTextureLayer::GetTextureNames() const
////{
////	return m_TextureNames;
////}
////inline void mtrTextureLayer::AddTextureName(const std::string& i_TextureName)
////{
////	m_TextureNames.push_back(i_TextureName);
////}
////inline void mtrTextureLayer::ClearTextureNames()
////{
////	m_TextureNames.clear();
////}
