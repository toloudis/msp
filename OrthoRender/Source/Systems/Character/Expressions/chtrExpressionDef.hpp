/********************************************************************************************\
**  chtrExpressionDef.hpp
**
**	Data structures supporting .chd character definition file, containing expressions
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef CHTR_EXPRESSIONDEF_HPP
#error chtrExpressionDef.hpp multiply included
#endif
#define CHTR_EXPRESSIONDEF_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
class chtrExpressionDef
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrExpressionDef()
	:	m_Name("Expression Name"), 
		m_Filename("Expression Animation File")
	{}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrExpressionDef(const std::string& i_Name, const itString &i_Filename) 
	:	m_Name("Expression Name"), 
		m_Filename("Expression Animation File")
	{
		m_Name.SetValue( i_Name );
		m_Filename.SetValue( i_Filename );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const chtrExpressionDef& i_Item)
	{
		return (this->m_Name == i_Item.m_Name) && (this->m_Filename == i_Item.m_Filename);
	}

public:
	prtyText		m_Name;
	prtyFileName	m_Filename;
};


//============================================================================
//============================================================================
class chtrExpressionModelDef
{
public:
//------------------------------------------------------------------------
//	Single expression
//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddExpression(const std::string& i_Name, const itString &i_Filename)
	{
		m_Expressions.push_back(chtrExpressionDef(i_Name, i_Filename));
	}

	prtyFileName m_ModelFilename;
	prtyFileName m_RestAnimFilename;
	std::vector<chtrExpressionDef> m_Expressions;

//------------------------------------------------------------------------
// Can pair up expressions to use in one slider
//------------------------------------------------------------------------
	class MultiPair
	{
	public:
		MultiPair()
		:	m_Name("Pair Name"),
			m_Left("Left Max Value"),
			m_Right("Right Max Value")
		{}

		prtyText m_Name;
		prtyText m_Left, m_Right;
	};
	std::vector<MultiPair> m_MultiPairs;
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddPair(const std::string& i_Name, 
				 const std::string& i_Left, 
				 const std::string& i_Right)
	{
		MultiPair pair;
		pair.m_Name.SetValue(i_Name);
		pair.m_Left.SetValue(i_Left);
		pair.m_Right.SetValue(i_Right);

		m_MultiPairs.push_back(pair);
	}

//------------------------------------------------------------------------
// Can group four expressions into one multi slider
//------------------------------------------------------------------------
	class MultiFour
	{
	public:
		MultiFour()
		:	m_Name("Pair Name"),
			m_Left("Left Max Value"),
			m_Right("Right Max Value"),
			m_Up("Up Max Value"),
			m_Down("Down Max Value")
		{}
		
	public:
		prtyText m_Name;
		prtyText m_Left, m_Right;
		prtyText m_Down, m_Up;
	};
	std::vector<MultiFour> m_MultiFours;
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddFour(const std::string& i_Name, 
				 const std::string& i_Left, 
				 const std::string& i_Right, 
				 const std::string& i_Down, 
				 const std::string& i_Up)
	{
		MultiFour four;
		four.m_Name.SetValue(i_Name);
		four.m_Left.SetValue(i_Left);
		four.m_Right.SetValue(i_Right);
		four.m_Up.SetValue(i_Up);
		four.m_Down.SetValue(i_Down);

		m_MultiFours.push_back(four);
	}
};

