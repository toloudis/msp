/****************************************************************************\
**	prtyComboBoxUIInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Core/prty/prtyComboBoxUIInfo.hpp"

#include "Core/prty/prtyProperty.hpp"

#include <cerrno>
#include <climits>
#include <cstdlib>


//============================================================================
//============================================================================
const int c_COMBOBOX_MAXWIDTH = 120;

namespace
{
	//	Whether i_Text is a whole number that fits an int (o_Value)
	bool parse_int(const std::string& i_Text, int& o_Value)
	{
		if (i_Text.empty())
			return false;
		const char* begin = i_Text.c_str();
		char* end = nullptr;
		errno = 0;
		const long value = std::strtol(begin, &end, 10);
		if (end == begin || *end != '\0' || errno == ERANGE || value < INT_MIN || value > INT_MAX)
			return false;
		o_Value = int(value);
		return true;
	}

	//	Whether every item is a whole number (an empty list isn't)
	bool all_numbers(const std::vector<std::string>& i_Items)
	{
		int value;
		for (const std::string& item : i_Items)
		{
			if (!parse_int(item, value))
				return false;
		}
		return !i_Items.empty();
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtyComboBoxUIInfo::prtyComboBoxUIInfo(prtyProperty* i_pProperty)
:	prtyPropertyUIInfo(i_pProperty),
	m_Width(c_COMBOBOX_MAXWIDTH)
{
	SetControlName("ComboBox");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtyComboBoxUIInfo::prtyComboBoxUIInfo(	prtyProperty* i_pProperty, 
										const std::string& i_Category, 
										const std::string& i_Description)
:	prtyPropertyUIInfo(i_pProperty, i_Category, i_Description),
	m_Width(c_COMBOBOX_MAXWIDTH)
{
	SetControlName("ComboBox");
}

//--------------------------------------------------------------------
// Return pointer to new equivalent prtyPropertyUIInfo
//--------------------------------------------------------------------
//virtual 
prtyPropertyUIInfo* prtyComboBoxUIInfo::Clone()
{
	return new prtyComboBoxUIInfo( this->GetProperty(0) );
}

//--------------------------------------------------------------------
// Clear out the items in the list.
//--------------------------------------------------------------------
void prtyComboBoxUIInfo::ClearItems()
{
	m_List.clear();
}

//--------------------------------------------------------------------
//	Add an item to the combobox to display.  If no index is given
//	it will be added to the end.
//--------------------------------------------------------------------
void prtyComboBoxUIInfo::AddItem(const std::string& i_Label, int i_Index )
{
	size_t add_index = i_Index;
	if (i_Index == -1)
	{
		add_index = m_List.size();
	}

	if (   (m_List.empty())
		|| (add_index >= m_List.size()))
	{
		m_List.resize( add_index+1 );
	}

	m_List[add_index] = i_Label;
}


//--------------------------------------------------------------------
//	width of control in pixels
//--------------------------------------------------------------------
const int prtyComboBoxUIInfo::GetWidth() const
{
	return m_Width;
}
void prtyComboBoxUIInfo::SetWidth(const int i_Width)
{
	m_Width = i_Width;
}

//--------------------------------------------------------------------
//	An integer property's value and the item that shows it
//--------------------------------------------------------------------
//static
int prtyComboBoxUIInfo::ValueOfItem(const std::vector<std::string>& i_Items, int i_Item)
{
	if (i_Item < 0 || i_Item >= int(i_Items.size()))
		return 0;
	int value = i_Item;
	if (all_numbers(i_Items))
		parse_int(i_Items[i_Item], value);
	return value;
}

//static
int prtyComboBoxUIInfo::ItemOfValue(const std::vector<std::string>& i_Items, int i_Value)
{
	if (all_numbers(i_Items))
	{
		int value;
		for (size_t i = 0; i < i_Items.size(); ++i)
		{
			if (parse_int(i_Items[i], value) && value == i_Value)
				return int(i);
		}
		return -1;
	}
	return (i_Value >= 0 && i_Value < int(i_Items.size())) ? i_Value : -1;
}
