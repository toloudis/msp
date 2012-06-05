/********************************************************************************************\
**  chrData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef SETS_DATA_HPP
#error chrData.hpp multiply included
#endif
#define SETS_DATA_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

#include <string>
#include <vector>


class chrExpression
{
public:
	chrExpression() {}

	chrExpression(const std::string& i_Name, const itString &i_Filename) 
		: m_Name(i_Name), m_Filename(i_Filename)
	{
	}

	bool operator == (const chrExpression& i_Item)
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
		m_Expressions.push_back(chrExpression(i_Name, i_Filename));
	}

	itString m_ModelFilename;
	itString m_RestAnimFilename;
	std::vector<chrExpression> m_Expressions;

	// Can pair up expressions to use in one slider
	struct MultiPair
	{
		std::string m_Name;
		std::string m_Left, m_Right;
	};
	std::vector<MultiPair> m_MultiPairs;
	
	void AddPair(const std::string& i_Name, 
				 const std::string& i_Left, 
				 const std::string& i_Right)
	{
		MultiPair pair = { i_Name, i_Left, i_Right };
		m_MultiPairs.push_back(pair);
	}

	// Can group four expressions into one multi slider
	struct MultiFour
	{
		std::string m_Name;
		std::string m_Left, m_Right;
		std::string m_Down, m_Up;
	};
	std::vector<MultiFour> m_MultiFours;
	
	void AddFour(const std::string& i_Name, 
				 const std::string& i_Left, 
				 const std::string& i_Right, 
				 const std::string& i_Down, 
				 const std::string& i_Up)
	{
		MultiFour four = { i_Name, i_Left, i_Right, i_Down, i_Up };
		m_MultiFours.push_back(four);
	}
};

