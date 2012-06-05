/****************************************************************************\
**	sprtSpriteData.hpp
**
**		The sprtSpriteData component defines class sprtSpriteData, which 
**	contains information about a rectangular sprite.  The sprtSpriteData is
**	mostly useful in combination with the sprtSpriteGroupModel.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SPRT_SPRITEDATA_HPP
#error sprtSpriteData.hpp multiply included
#endif
#define SPRT_SPRITEDATA_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class sprtSpriteData
{
	public:
		//--------------------------------------------------------------------
		//	default constructor
		//--------------------------------------------------------------------
		sprtSpriteData();

		//--------------------------------------------------------------------
		//	Linked list accessors
		//--------------------------------------------------------------------
		sprtSpriteData* GetNext();
		const sprtSpriteData* GetNext() const;
		sprtSpriteData* GetPrev();
		const sprtSpriteData* GetPrev() const;
		
		//--------------------------------------------------------------------
		//	Linked list mutators
		//--------------------------------------------------------------------
		void SetNext(sprtSpriteData* i_Next);
		void SetPrev(sprtSpriteData* i_Prev);

		//--------------------------------------------------------------------
		//	Get/SetStartPosition.  The default is the origin.
		//--------------------------------------------------------------------
		const maPoint3d& GetStartPosition() const;
		void SetStartPosition(const maPoint3d& i_StartPosition);

		//--------------------------------------------------------------------
		//	Get/SetPosition.  The default is the origin.
		//--------------------------------------------------------------------
		const maPoint3d& GetPosition() const;
		void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	Get/SetLastPosition.  This is used in streak rendering
		//  to create a particle that stretches from the last position
		//	to the current position.
		//--------------------------------------------------------------------
		const maPoint3d& GetLastPosition() const;
		void SetLastPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	Alpha multiplier - in multiplication with whatever 
		//	alpha/translucency the material used to rendered this may have, 
		//	setting Alpha to less than one will make the sprite translucent.
		//	The default value is 1.0.
		//--------------------------------------------------------------------
		float GetAlpha() const;
		void SetAlpha(float i_Alpha);

		//--------------------------------------------------------------------
		//	Get/SetRotation modifies the rotation about the camera view axis.
		//	The default value is 0.
		//--------------------------------------------------------------------
		float GetRotation() const;
		void SetRotation(float i_Rotation);

		//--------------------------------------------------------------------
		//	Get/SetStartRotation modifies the StartRotation about the camera view axis.
		//	The default value is 0.
		//--------------------------------------------------------------------
		float GetStartRotation() const;
		void SetStartRotation(float i_StartRotation);

		//--------------------------------------------------------------------
		//	Get/SetScale modifies the scale (size) of the sprite.  The default
		//	is 1.0.
		//--------------------------------------------------------------------
		float GetScale() const;
		void SetScale(float i_Scale);

		//--------------------------------------------------------------------
		//	Get/SetUVAFrame changes which frame of a UVA the sprite will be
		//	rendered with.  If this value is negative, the "natural" value
		//	of the UVA is used (the frame of the UVA animation at the frame 
		//	time it is being rendered at).  The default is -1.
		//--------------------------------------------------------------------
		int GetUVAFrame() const;
		void SetUVAFrame(int i_UVAFrame);

	private:
		sprtSpriteData* m_Next;
		sprtSpriteData* m_Prev;
		maPoint3d m_Position;
		maPoint3d m_LastPosition;
		maPoint3d m_StartPosition;
		float m_Rotation;
		float m_StartRotation;
		float m_Alpha;
		float m_Scale;
		int m_UVAFrame;
};


//--------------------------------------------------------------------
//	default constructor
//--------------------------------------------------------------------
inline sprtSpriteData::sprtSpriteData()
:		m_Position(0.0f, 0.0f, 0.0f),
		m_StartPosition(0.0f, 0.0f, 0.0f),
		m_Rotation(0.0f),
		m_StartRotation(0.0f),
		m_Scale(1.0f),
		m_UVAFrame(-1),
		m_Next(NULL),
		m_Prev(NULL)
{
}

//--------------------------------------------------------------------
//	Linked list accessors
//--------------------------------------------------------------------
inline sprtSpriteData* sprtSpriteData::GetNext()
{
	return m_Next;
}

inline const sprtSpriteData* sprtSpriteData::GetNext() const
{
	return m_Next;
}

inline sprtSpriteData* sprtSpriteData::GetPrev()
{
	return m_Prev;
}

inline const sprtSpriteData* sprtSpriteData::GetPrev() const
{
	return m_Prev;
}

//--------------------------------------------------------------------
//	Linked list mutators
//--------------------------------------------------------------------
inline void sprtSpriteData::SetNext(sprtSpriteData* i_Next)
{
	m_Next = i_Next;
}

inline void sprtSpriteData::SetPrev(sprtSpriteData* i_Prev)
{
	m_Prev = i_Prev;
}

//--------------------------------------------------------------------
//	Get/SetStartPosition.  The default is the origin.
//--------------------------------------------------------------------
inline const maPoint3d& sprtSpriteData::GetStartPosition() const
{
	return m_StartPosition;
}

inline void sprtSpriteData::SetStartPosition(const maPoint3d& i_StartPosition)
{
	m_StartPosition = i_StartPosition;
	m_LastPosition = i_StartPosition;
}


//--------------------------------------------------------------------
//	Get/SetPosition.  The default is the origin.
//--------------------------------------------------------------------
inline const maPoint3d& sprtSpriteData::GetPosition() const
{
	return m_Position;
}

inline void sprtSpriteData::SetPosition(const maPoint3d& i_Position)
{
	m_Position = i_Position;
}


//--------------------------------------------------------------------
//	Get/SetLastPosition.  This is used in streak rendering
//  to create a particle that stretches from the last position
//	to the current position.
//--------------------------------------------------------------------
inline const maPoint3d& sprtSpriteData::GetLastPosition() const
{
	return m_LastPosition;
}

inline void sprtSpriteData::SetLastPosition(const maPoint3d& i_Position)
{
	m_LastPosition = i_Position;
}


//--------------------------------------------------------------------
//	Alpha multiplier - in multiplication with whatever 
//	alpha/translucency the material used to rendered this may have, 
//	setting Alpha to less than one will make the sprite translucent.
//	The default value is 1.0.
//--------------------------------------------------------------------
inline float sprtSpriteData::GetAlpha() const
{
	return m_Alpha;
}

inline void sprtSpriteData::SetAlpha(float i_Alpha)
{
	m_Alpha = i_Alpha;
}


//--------------------------------------------------------------------
//	Get/SetRotation modifies the rotation about the camera view axis.
//	The default value is 0.
//--------------------------------------------------------------------
inline float sprtSpriteData::GetRotation() const
{
	return m_Rotation;
}

inline void sprtSpriteData::SetRotation(float i_Rotation)
{
	m_Rotation = i_Rotation;
}


//--------------------------------------------------------------------
//	Get/SetStartRotation modifies the StartRotation about the camera view axis.
//	The default value is 0.
//--------------------------------------------------------------------
inline float sprtSpriteData::GetStartRotation() const
{
	return m_StartRotation;
}

inline void sprtSpriteData::SetStartRotation(float i_StartRotation)
{
	m_StartRotation = i_StartRotation;
}


//--------------------------------------------------------------------
//	Get/SetScale modifies the scale (size) of the sprite.  The default
//	is 1.0.
//--------------------------------------------------------------------
inline float sprtSpriteData::GetScale() const
{
	return m_Scale;
}

inline void sprtSpriteData::SetScale(float i_Scale)
{
	m_Scale = i_Scale;
}


//--------------------------------------------------------------------
//	Get/SetUVAFrame changes which frame of a UVA the sprite will be
//	rendered with.  If this value is negative, the "natural" value
//	of the UVA is used (the frame of the UVA animation at the frame 
//	time it is being rendered at).  The default is -1.
//--------------------------------------------------------------------
inline int sprtSpriteData::GetUVAFrame() const
{
	return m_UVAFrame;
}

inline void sprtSpriteData::SetUVAFrame(int i_UVAFrame)
{
	m_UVAFrame = i_UVAFrame;
}


