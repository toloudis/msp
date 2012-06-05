/********************************************************************************************\
**  chtrExpressionData.hpp
**
**	Data structures for current state of an expression
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef CHTR_EXPRESSIONDATA_HPP
#error chtrExpressionData.hpp multiply included
#endif
#define CHTR_EXPRESSIONDATA_HPP

#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class chtrExpressionData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrExpressionData() 
	:	m_Name("Expression Name"),
		m_Weight("Weight")
	{};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrExpressionData(const std::string& i_Name, const float i_Weight)
	:	m_Name("Expression Name"),
		m_Weight("Weight")
	{
		m_Name.SetValue( i_Name );
		m_Weight.SetValue( i_Weight );
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual chtrExpressionData* Clone() const
	{
		return NULL;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual bool operator == (const chtrExpressionData& i_Item)
	{
		return ((this->m_Name == i_Item.m_Name) && (this->m_Weight == i_Item.m_Weight));
	};

public:
	prtyText		m_Name;
	prtyFloat		m_Weight;
};


//============================================================================
//============================================================================
class chtrSingleExpressionData : public chtrExpressionData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrSingleExpressionData() 
	:	m_FileName("Expression Anim")
	{};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual chtrExpressionData* Clone() const
	{
		return new chtrSingleExpressionData( *this );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const chtrSingleExpressionData& i_Item)
	{
		return ((this->m_Name == i_Item.m_Name) && (this->m_Weight == i_Item.m_Weight) && (this->m_FileName == i_Item.m_FileName));
	};

public:
	prtyFilePath	m_FileName;
};


//============================================================================
//============================================================================
class chtrDualExpressionData : public chtrExpressionData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrDualExpressionData() 
	:	m_FileNameLeft("Expression Left Anim"),
		m_FileNameRight("Expression Right Anim")
	{};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual chtrExpressionData* Clone() const
	{
		return new chtrDualExpressionData( *this );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const chtrDualExpressionData& i_Item)
	{
		return (   (this->m_Name == i_Item.m_Name) 
				&& (this->m_Weight == i_Item.m_Weight) 
				&& (this->m_FileNameLeft == i_Item.m_FileNameLeft) 
				&& (this->m_FileNameRight == i_Item.m_FileNameRight));
	};

public:
	prtyFilePath	m_FileNameLeft;
	prtyFilePath	m_FileNameRight;
};


//============================================================================
//============================================================================
class chtrQuadExpressionData : public chtrExpressionData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrQuadExpressionData() 
	:	m_FileNameLeft("Expression Left Anim"),
		m_FileNameRight("Expression Right Anim"),
		m_FileNameUp("Expression Up Anim"),
		m_FileNameDown("Expression Down Anim")
	{};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual chtrExpressionData* Clone() const
	{
		return new chtrQuadExpressionData( *this );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const chtrQuadExpressionData& i_Item)
	{
		return (   (this->m_Name == i_Item.m_Name) 
				&& (this->m_Weight == i_Item.m_Weight) 
				&& (this->m_FileNameLeft == i_Item.m_FileNameLeft) 
				&& (this->m_FileNameRight == i_Item.m_FileNameRight)
				&& (this->m_FileNameUp == i_Item.m_FileNameUp)
				&& (this->m_FileNameDown == i_Item.m_FileNameDown));
	};

public:
	prtyFilePath	m_FileNameLeft;
	prtyFilePath	m_FileNameRight;
	prtyFilePath	m_FileNameUp;
	prtyFilePath	m_FileNameDown;
};


//============================================================================
//============================================================================
typedef std::vector<chtrExpressionData*> chtrExpressionsData;
