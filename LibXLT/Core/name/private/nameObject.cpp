/****************************************************************************\
**	nameObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Core/name/nameObject.hpp"
#include "Core/name/nameMgr.hpp"

#include "Tool/gui/guiMessageBox.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
nameObject::~nameObject()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void nameObject::SetName(const nameString& i_Name)
{
	if (m_Name.GetUID() != nameString::e_InvalidUID && m_Name.ExactMatch(i_Name))
		return;

	m_Name = i_Name;
	
	// Check existing names once, not every UID ever allocated. As before,
	// an older object keeps its name when a later UID has the same text.
	nameList names;
	if (i_Name.GetUID() > 0)
		nameMgr::GetNameList(names);
	for (const nameString* name : names)
	{
		const std::string text = name->GetString();
		if (name->GetUID() >= 0 && name->GetUID() < i_Name.GetUID()
			&& !text.empty() && m_Name.GetString() == text)
		{
			std::string msg = "Name \" " + text +"\" exists. Please use a different name.";
			guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
			m_Name.SetString(i_Name.GetString() + ".copy");
			break;
		}
	}
	nameMgr::RegisterName( m_Name );
	//DBG_LOG2( "registered Name -------> %d %s", m_Name.GetUID(), m_Name.GetString().c_str() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const nameString& nameObject::GetName() const
{
	return m_Name;
}

//--------------------------------------------------------------------
// UpdateName() is called when the gui sets the name of the object,
//	derived classes can set dirty bits and do "undo" operations, etc.
// The default behavior calls SetName()
//--------------------------------------------------------------------
//virtual 
//void nameObject::UpdateName(const std::string& i_Name)
//{
//	m_Name.SetString(i_Name);
//	this->SetName(i_Name);
//}
