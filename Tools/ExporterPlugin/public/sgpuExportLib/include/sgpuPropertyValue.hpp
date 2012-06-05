/****************************************************************************\
**  sgpuPropertyValue.hpp
**
**      sgpuPropertyValue.hpp defines an exception class that is usd by the sdk
**		Expect the routines of the sdk to throw this exception.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_PROPERTYVALUE_HPP
#define SGPU_PROPERTYVALUE_HPP

#include "sgpuExportLib.hpp"
#include "sgpuString.hpp"

//============================================================================
//============================================================================

/**
//A vaue-type tuple 
*/
class SGPUEXPORTLIB_API sgpuPropertyValue
{
public:
	typedef enum { eUnknown=0, eBool, eFloat, eInt, eString } PType;
	sgpuPropertyValue();

	sgpuPropertyValue( const sgpuPropertyValue &i_Other);

	sgpuPropertyValue & operator=( const sgpuPropertyValue & i_Other );

	bool operator==( const sgpuPropertyValue & i_Other ) const ;

	bool GetBoolValue() const;

	~sgpuPropertyValue(){}

public:
	sgpuString m_Value;
	PType m_Type;
};


#endif // #ifndef SGPU_PROPERTYVALUE_HPP
