/****************************************************************************\
**	prtyFilePathComboBoxUIInfo.hpp
**
**		ComboBox UI info for a file path property (prtyFilePath). Each
**	entry has a label to display and the path it stands for, so a short
**	name ("Phong") can select a longer value ("Phong.fx").
\****************************************************************************/
#ifdef PRTY_FILEPATHCOMBOBOXUIINFO_HPP
#error prtyFilePathComboBoxUIInfo.hpp multiply included
#endif
#define PRTY_FILEPATHCOMBOBOXUIINFO_HPP

#ifndef PRTY_COMBOBOXUIINFO_HPP
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
class prtyFilePath;


//============================================================================
//============================================================================
class prtyFilePathComboBoxUIInfo : public prtyComboBoxUIInfo
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFilePathComboBoxUIInfo(prtyFilePath* i_pProperty, const std::string& i_Category, const std::string& i_Description);

		//--------------------------------------------------------------------
		// Return pointer to new equivalent prtyPropertyUIInfo
		//--------------------------------------------------------------------
		virtual prtyPropertyUIInfo* Clone();

		//--------------------------------------------------------------------
		// Add an entry to the end of the list: the label shown and the
		//	path the property is set to when it is picked.
		//--------------------------------------------------------------------
		void AddChoice(const std::string& i_Label, const fsLocator& i_Value);

		//--------------------------------------------------------------------
		// Path of each entry, in the same order as the labels in m_List
		//--------------------------------------------------------------------
		const std::vector<fsLocator>& GetValues() const;

		//--------------------------------------------------------------------
		// Index of the entry for the given path, or -1. See IsSamePath.
		//--------------------------------------------------------------------
		int FindChoice(const fsLocator& i_Value) const;

		//--------------------------------------------------------------------
		// Two paths are the same if they spell the same file name, ignoring
		//	case and '/' versus '\'.
		//--------------------------------------------------------------------
		static bool IsSamePath(const fsLocator& i_A, const fsLocator& i_B);

	private:
		std::vector<fsLocator> m_Values;
};
