/****************************************************************************\
**	prtyComboBoxUIInfo.hpp
**
**		ComboBox UI info
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef PRTY_COMBOBOXUIINFO_HPP
#error prtyComboBoxUIInfo.hpp multiply included
#endif
#define PRTY_COMBOBOXUIINFO_HPP

#ifndef PRTY_PROPERTYUIINFO_HPP
#include "Core/prty/prtyPropertyUIInfo.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
class prtyProperty;


//============================================================================
//============================================================================
class prtyComboBoxUIInfo : public prtyPropertyUIInfo
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyComboBoxUIInfo(prtyProperty* i_pProperty);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyComboBoxUIInfo(prtyProperty* i_pProperty, const std::string& i_Category, const std::string& i_Description);

		//--------------------------------------------------------------------
		// Return pointer to new equivalent prtyPropertyUIInfo
		//--------------------------------------------------------------------
		virtual prtyPropertyUIInfo* Clone();

		//--------------------------------------------------------------------
		// Clear out the items in the list.
		//--------------------------------------------------------------------
		void ClearItems();

		//--------------------------------------------------------------------
		//	Add an item to the combobox to display.  If no index is given
		//	it will be added to the end.
		//--------------------------------------------------------------------
		void AddItem(const std::string& i_Label, int i_Index = -1 );

		//--------------------------------------------------------------------
		//	width of control in pixels
		//--------------------------------------------------------------------
		const int GetWidth() const;
		void SetWidth(const int i_Width);

		//--------------------------------------------------------------------
		//	An integer property's value and the item that shows it, for the
		//	toolkits' integer combo boxes. When every item is a whole number
		//	("256,512,1024") the value is the item's number; otherwise (named
		//	items, "Opaque,Mask,Blend") it is the item's index.
		//	ValueOfItem - the value item i_Item stands for (0 if out of range)
		//	ItemOfValue - the item that shows i_Value, -1 if none
		//--------------------------------------------------------------------
		static int ValueOfItem(const std::vector<std::string>& i_Items, int i_Item);
		static int ItemOfValue(const std::vector<std::string>& i_Items, int i_Value);

	public:
		std::vector<std::string> m_List;

	private:
		int m_Width;
};
