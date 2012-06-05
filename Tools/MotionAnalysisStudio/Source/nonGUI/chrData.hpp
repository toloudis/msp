/********************************************************************************************\
**  chrData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef CHR_DATA_HPP
#error chrData.hpp multiply included
#endif
#define CHR_DATA_HPP

#ifndef IT_STRING_HPP
#include "itString.hpp"
#endif

#include <string>
#include <vector>


class mcpExpression
{
public:
	mcpExpression() {}

	mcpExpression(const std::string& i_Name, const itString &i_Filename) 
		: m_Name(i_Name), m_Filename(i_Filename)
	{
	}

	bool operator == (const mcpExpression& i_Item)
	{
		return (this->m_Name == i_Item.m_Name) && (this->m_Filename == i_Item.m_Filename);
	}

	std::string m_Name;
	itString m_Filename;
};


class chrData
{
public:
	void AddExpression(const std::string& i_Name, const itString &i_Filename)
	{
		m_Expressions.push_back(mcpExpression(i_Name, i_Filename));
	}

	itString m_ModelFilename;
	itString m_RestAnimFilename;
	std::vector<mcpExpression> m_Expressions;
};

