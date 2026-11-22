/****************************************************************************\
**	pqtName_ListBox.hpp
**
**		Intermediate class between the property (prtyName) and 
**	the control (ListBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtName_ListBox.hpp"

#include "Core/prty/prtyListBoxUIInfo.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
// Get list of string choices for combo box
//----------------------------------------------------------------------------
void pqtName_ListBox_Converter::GetChoices(prtyPropertyUIInfo* i_pUIInfo,
											std::vector<std::string>& o_Choices)
{
	prtyListBoxUIInfo* pUII = static_cast<prtyListBoxUIInfo*>(i_pUIInfo);
	o_Choices = pUII->m_List;
}

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtName_ListBox_Converter::SetValueIntoControl(QListWidget* i_pActualControl,
													nameString i_Value)
{
	pqtControlUtil::SelectString(i_pActualControl, tqtQString(i_Value.GetString()));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
nameString pqtName_ListBox_Converter::GetValueFromControl(QListWidget* i_pActualControl)
{
	QListWidgetItem* pItem = i_pActualControl->currentItem();
	std::string sel_text = pItem ? tqtStdString(pItem->text()) : std::string();
	return nameString(sel_text);
}

#endif
