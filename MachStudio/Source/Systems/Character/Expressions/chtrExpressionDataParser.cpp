/********************************************************************************************\
**  chtrExpressionDataParser.cpp
**
**		See .hpp
**
**  StudioGPU
**  Copyright(C) 2005-8 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Character/Expressions/chtrExpressionDataParser.hpp"
#include "Systems/Character/GUI/chtrAnimList.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"


//============================================================================
//============================================================================
namespace chtrExpressionDataParser
{

	//============================================================================
	//============================================================================
	namespace
	{
	const chDefs::Name c_EXPR = chDefs::MakeName('E', 'X', 'P', 'R');
	const chDefs::Name c_EXP1 = chDefs::MakeName('E', 'X', 'P', '1');
	const chDefs::Name c_EXP2 = chDefs::MakeName('E', 'X', 'P', '2');
	const chDefs::Name c_EXP4 = chDefs::MakeName('E', 'X', 'P', '4');

	//--------------------------------------------------------------------
	// Check absolute path and resolve single filenames into fullpaths
	//--------------------------------------------------------------------
	void resolve_animpath(prtyFilePath &io_FilePath)
	{
		fsLocator anim_loc = io_FilePath.GetValue();
		if (anim_loc.GetNumNames() > 0)
		{
			// This comparison is only needed to support old file formats
			// and could be removed in product
			if (anim_loc.GetNumNames() == 1)
			{
				itString filename = anim_loc.GetLastName();
				fsLocator char_dir; // not needed to resolve expressions
				anim_loc = chtrAnimList::GetAnimationLocator(filename, char_dir);
			}

			if (fsAbsolutePathMgr::ResolvePath(anim_loc, "Animations"))
				io_FilePath.SetValue( anim_loc );
		}
	}

}


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name GetChunkName()
{
	return c_EXPR;
}


//------------------------------------------------------------------------
//   ReadExpression1
//------------------------------------------------------------------------
void ReadExpression1(chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					chtrSingleExpressionData& o_Data )
{
	o_Data.m_Name.Read(i_Reader);
	o_Data.m_FileName.Read(i_Reader);
	resolve_animpath(o_Data.m_FileName);
}

//------------------------------------------------------------------------
//   ReadExpression2
//------------------------------------------------------------------------
void ReadExpression2(chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					chtrDualExpressionData& o_Data )
{
	o_Data.m_Name.Read(i_Reader);
	o_Data.m_FileNameLeft.Read(i_Reader);
	o_Data.m_FileNameRight.Read(i_Reader);
	resolve_animpath(o_Data.m_FileNameLeft);
	resolve_animpath(o_Data.m_FileNameRight);
}

//------------------------------------------------------------------------
//   ReadExpression4
//------------------------------------------------------------------------
void ReadExpression4(chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					chtrQuadExpressionData& o_Data )
{
	o_Data.m_Name.Read(i_Reader);
	o_Data.m_FileNameLeft.Read(i_Reader);
	o_Data.m_FileNameRight.Read(i_Reader);
	o_Data.m_FileNameUp.Read(i_Reader);
	o_Data.m_FileNameDown.Read(i_Reader);
	resolve_animpath(o_Data.m_FileNameLeft);
	resolve_animpath(o_Data.m_FileNameRight);
	resolve_animpath(o_Data.m_FileNameUp);
	resolve_animpath(o_Data.m_FileNameDown);
}

//------------------------------------------------------------------------
//   WriteExpression1
//------------------------------------------------------------------------
void WriteExpression1(chWriter& o_Writer,
					 const chtrSingleExpressionData& i_Data )
{
	const int l_EXP1_VERSION = 0;
	o_Writer.WriteChunkHeader( c_EXP1, l_EXP1_VERSION, false );

	i_Data.m_Name.Write(o_Writer);
	i_Data.m_FileName.Write(o_Writer);

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   WriteExpression2
//------------------------------------------------------------------------
void WriteExpression2(chWriter& o_Writer,
					 const chtrDualExpressionData& i_Data )
{
	const int l_EXP2_VERSION = 0;
	o_Writer.WriteChunkHeader( c_EXP2, l_EXP2_VERSION, false );

	i_Data.m_Name.Write(o_Writer);
	i_Data.m_FileNameLeft.Write(o_Writer);
	i_Data.m_FileNameRight.Write(o_Writer);

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   WriteExpression4
//------------------------------------------------------------------------
void WriteExpression4(chWriter& o_Writer,
					 const chtrQuadExpressionData& i_Data )
{
	const int l_EXP4_VERSION = 0;
	o_Writer.WriteChunkHeader( c_EXP4, l_EXP4_VERSION, false );

	i_Data.m_Name.Write(o_Writer);
	i_Data.m_FileNameLeft.Write(o_Writer);
	i_Data.m_FileNameRight.Write(o_Writer);
	i_Data.m_FileNameUp.Write(o_Writer);
	i_Data.m_FileNameDown.Write(o_Writer);

	o_Writer.FinishChunk();
}


//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				chtrExpressionsData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_EXP1 )
		{
			// expression
			chtrSingleExpressionData* data = new chtrSingleExpressionData();
			ReadExpression1(i_Reader, version, size, *data);
			o_Data.push_back(data);
		}
		else if ( name == c_EXP2 )
		{
			// multi-expression pair
			chtrDualExpressionData* data = new chtrDualExpressionData();
			ReadExpression2(i_Reader, version, size, *data);
			o_Data.push_back(data);
		}
		else if ( name == c_EXP4 )
		{
			// multi-expression four group
			chtrQuadExpressionData* data = new chtrQuadExpressionData();
			ReadExpression4(i_Reader, version, size, *data);
			o_Data.push_back(data);
		}
		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
// ReadData
//------------------------------------------------------------------------
void ReadData(	const fsLocator &i_Locator,
				chtrExpressionsData& o_Data )
{
	gfFileBin ifile(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
	ifile.ReadHeader();
	chBinReader reader(ifile);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_EXPR )
		{
			ReadData(reader, version, size, o_Data);
		}
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const chtrExpressionsData& i_Data )
{
	const int l_EXPR_VERSION = 0;
	o_Writer.WriteChunkHeader( c_EXPR, l_EXPR_VERSION, true );

	for (int i=0; i < i_Data.size(); i++)
	{
		chtrSingleExpressionData* pExp1 = dynamic_cast<chtrSingleExpressionData*>(i_Data[i]);
		if (pExp1 != NULL)
		{
			WriteExpression1(o_Writer, *pExp1);
		}
		else
		{
			chtrDualExpressionData* pExp2 = dynamic_cast<chtrDualExpressionData*>(i_Data[i]);
			if (pExp2 != NULL)
			{
				WriteExpression2(o_Writer, *pExp2);
			}
			else
			{
				chtrQuadExpressionData* pExp4 = dynamic_cast<chtrQuadExpressionData*>(i_Data[i]);
				if (pExp4 != NULL)
				{
					WriteExpression4(o_Writer, *pExp4);
				}
			}
		}

	}

	o_Writer.FinishChunk();
}


//------------------------------------------------------------------------
// WriteData
//------------------------------------------------------------------------
void WriteData( const fsLocator &i_Locator,
				const chtrExpressionsData& i_Data )
{
	// Create new file
	if( fsFileUtil::FileExists(i_Locator) )
		fsFileUtil::DeleteFile(i_Locator);
	fsFileUtil::CreateFile(i_Locator);

	gfFileBin ofile(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	ofile.WriteHeader();
	chBinWriter writer(ofile);

	//	Write out the data
	WriteData(writer, i_Data);
}

}	// end of namespace
