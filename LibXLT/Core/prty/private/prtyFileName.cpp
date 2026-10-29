/****************************************************************************\
**	prtyFileName.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Core/prty/prtyFileName.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtyFileName::prtyFileName()
:	prtyPropertyTemplate( "FileName", std::string() )
{
	//DBG_LOG("file name (%s)", i_Name.c_str() );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtyFileName::prtyFileName(const std::string& i_Name)
:	prtyPropertyTemplate( i_Name, std::string() )
{
	//DBG_LOG("file name (" << i_Name.c_str() << ")" );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyFileName::prtyFileName(const std::string& i_Name, const std::string& i_InitialValue)
:	prtyPropertyTemplate( i_Name, i_InitialValue )
{
	//DBG_LOG("file name (" << i_Name.c_str() << ")" );
}

//--------------------------------------------------------------------
//	The type of property it is
//--------------------------------------------------------------------
const char* prtyFileName::GetType()
{
	return "FileName";
}

//--------------------------------------------------------------------
//	return a null-terminated std::string
//--------------------------------------------------------------------
const std::string prtyFileName::GetString() const
{
	return m_Value;
}

//--------------------------------------------------------------------
//	operators											
//--------------------------------------------------------------------
prtyFileName& prtyFileName::operator =(const prtyFileName& i_Property)
{
	// copy base data
	prtyProperty::operator =(i_Property);

	SetValue(i_Property.GetValue());
	return *this;
}
prtyFileName& prtyFileName::operator =(const std::string& i_Value)
{
	SetValue(i_Value);
	return *this;
}

//--------------------------------------------------------------------
//	comparison operators											
//--------------------------------------------------------------------
bool prtyFileName::operator ==(const prtyFileName& i_Property) const
{
	return (m_Value == i_Property.GetValue());
}
bool prtyFileName::operator !=(const prtyFileName& i_Property) const
{
	return (m_Value != i_Property.GetValue());
}
bool prtyFileName::operator ==(const std::string& i_Value) const
{
	return (m_Value == i_Value);
}
bool prtyFileName::operator !=(const std::string& i_Value) const
{
	return (m_Value != i_Value);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void prtyFileName::Read(chReader& io_Reader)
{
	std::string temp;
	chChunkParserUtil::ReadItString(io_Reader,temp);
	SetValue(temp);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void prtyFileName::Write(chWriter& io_Writer) const
{
	chChunkParserUtil::Write(io_Writer, GetValue());
}
