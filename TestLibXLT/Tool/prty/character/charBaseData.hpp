//
//		A base data class for an char system object
//
#ifdef CHAR_BASEDATA_HPP
#error charBaseData.hpp multiply included
#endif
#define CHAR_BASEDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_DIRECTORY_HPP
#include "Core/prty/prtyDirectory.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif


//============================================================================
//============================================================================
class charBaseData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	charBaseData()
	:	m_bEditorVisible("Visible in Editor", true),
		m_FOV("Field of View", 34.4f),
		m_Position("Position"),
		m_Orientation("Orientation"),
		m_Dir("Texture Directory"),
		m_File("Character File"),
		m_FileName("File Name"),
		m_State("Emotional State")
	{
		SetStates();

		m_Position.SetValue(1.0f, 3.0f, 5.0f);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	charBaseData(charBaseData& i_Data)
	:	m_bEditorVisible("Visible in Editor", i_Data.m_bEditorVisible.GetValue()),
		m_FOV("Field of View", i_Data.m_FOV.GetValue()),
		m_Position("Position", i_Data.m_Position.GetValue()),
		m_Orientation("Orientation", i_Data.m_Orientation.GetValue()),
		m_Dir("Texture Directory", i_Data.m_Dir.GetValue()),
		m_File("Character File", i_Data.m_File.GetValue()),
		m_FileName("File Name", i_Data.m_FileName.GetValue()),
		m_State("Emotional State", i_Data.m_State.GetValue())
	{
		SetStates();

		m_Position.SetValue(1.0f, 3.0f, 3.0f);
	}

private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetStates()
	{
		m_State.SetEnumTag(0,"Happy");
		m_State.SetEnumTag(1,"Sad");
		m_State.SetEnumTag(2,"Grumpy");
		m_State.SetEnumTag(3,"Depressed");
		m_State.SetEnumTag(4,"Ecstatic");
	}
public:
	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	prtyBoolean		m_bEditorVisible;
	prtyFloat		m_FOV;
	prtyVector3d	m_Position;
	prtyVector3d	m_Orientation;
	prtyDirectory	m_Dir;
	prtyFilePath	m_File;
	prtyFileName	m_FileName;
	prtyEnum		m_State;
};

