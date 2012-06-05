//
//		A base data class for an xxx system object
//
#ifdef XXX_BASEDATA_HPP
#error xxxBaseData.hpp multiply included
#endif
#define XXX_BASEDATA_HPP

//============================================================================
//============================================================================
class xxxBaseData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	xxxBaseData()
	:	m_bEditorVisible(true),
		m_FOV(34.4f),
		m_Tilt(1.1f),
		m_Near(23.4f),
		m_Far(1000.99f)
	{
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	xxxBaseData(xxxBaseData& i_Data)
	:	m_bEditorVisible(i_Data.m_bEditorVisible),
		m_FOV(34.4f),
		m_Tilt(1.1f),
		m_Near(23.4f),
		m_Far(1000.99f)
	{
	}

public:
	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	bool		m_bEditorVisible;
	float		m_FOV;		// degrees wide camera angle
	float		m_Tilt;		// degrees tilt
	float		m_Near;		// near clipping plane
	float		m_Far;		// far clipping plane
};

