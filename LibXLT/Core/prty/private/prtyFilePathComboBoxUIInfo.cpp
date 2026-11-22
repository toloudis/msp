/****************************************************************************\
**	prtyFilePathComboBoxUIInfo.cpp
**
**		see .hpp
\****************************************************************************/
#include "Core/prty/prtyFilePathComboBoxUIInfo.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/prty/prtyFilePath.hpp"

#include <string.h>


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	// the path as one string, lower case, with '\' separators
	//------------------------------------------------------------------------
	std::string normalized_path(const fsLocator& i_Path)
	{
		std::string path;
		fsFileUtil::LocatorToANSIFilename(i_Path, path);
		for (size_t i = 0; i < path.size(); i++)
		{
			if (path[i] == '/')
				path[i] = '\\';
			else if (path[i] >= 'A' && path[i] <= 'Z')
				path[i] = path[i] - 'A' + 'a';
		}
		return path;
	}
}	// end of namespace


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtyFilePathComboBoxUIInfo::prtyFilePathComboBoxUIInfo(prtyFilePath* i_pProperty,
													   const std::string& i_Category,
													   const std::string& i_Description)
:	prtyComboBoxUIInfo(i_pProperty, i_Category, i_Description)
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent prtyPropertyUIInfo
//--------------------------------------------------------------------
//virtual
prtyPropertyUIInfo* prtyFilePathComboBoxUIInfo::Clone()
{
	prtyFilePathComboBoxUIInfo* pClone = new prtyFilePathComboBoxUIInfo(
		static_cast<prtyFilePath*>(this->GetProperty(0)), GetCategory(), GetDescription());
	pClone->m_List = m_List;
	pClone->m_Values = m_Values;
	pClone->SetConfirmationString(GetConfirmationString());
	return pClone;
}

//--------------------------------------------------------------------
// Add an entry to the end of the list
//--------------------------------------------------------------------
void prtyFilePathComboBoxUIInfo::AddChoice(const std::string& i_Label, const fsLocator& i_Value)
{
	m_List.push_back(i_Label);
	m_Values.push_back(i_Value);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::vector<fsLocator>& prtyFilePathComboBoxUIInfo::GetValues() const
{
	return m_Values;
}

//--------------------------------------------------------------------
// Index of the entry for the given path, or -1
//--------------------------------------------------------------------
int prtyFilePathComboBoxUIInfo::FindChoice(const fsLocator& i_Value) const
{
	for (size_t i = 0; i < m_Values.size(); i++)
	{
		if (IsSamePath(m_Values[i], i_Value))
			return (int)i;
	}
	return -1;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static
bool prtyFilePathComboBoxUIInfo::IsSamePath(const fsLocator& i_A, const fsLocator& i_B)
{
	if (i_A == i_B)
		return true;
	return normalized_path(i_A) == normalized_path(i_B);
}
