/********************************************************************************************\
**  xtraPropertyDataParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Support/xtra/xtraPropertyDataParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace xtraPropertyDataParser
{

namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_XPBL = chDefs::MakeName('X', 'P', 'B', 'L');	// custom BooLean property
	const chDefs::Name c_XPFL = chDefs::MakeName('X', 'P', 'F', 'L');	// custom FLoat property
	const chDefs::Name c_XPCL = chDefs::MakeName('X', 'P', 'C', 'L');	// custom CoLor property
	const chDefs::Name c_XPOR = chDefs::MakeName('X', 'P', 'O', 'R');	// custom ORientation property
	const chDefs::Name c_XPPS = chDefs::MakeName('X', 'P', 'P', 'S');	// custom PoSition property
	const chDefs::Name c_XPST = chDefs::MakeName('X', 'P', 'S', 'T');	// custom STring property
	const chDefs::Name c_XPTX = chDefs::MakeName('X', 'P', 'T', 'X');	// custom TeXture property

	//========================================================================
	//   Boolean
	//========================================================================
	void ReadBooleanPropertyData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					xtraBooleanPropertyData& o_Data )
	{
		o_Data.m_Value.Read(i_Reader);
		o_Data.m_Name.Read(i_Reader);
		o_Data.m_Category.Read(i_Reader);
		o_Data.m_Description.Read(i_Reader);
	}
	void WriteBooleanPropertyData(	chWriter& o_Writer,
					const xtraBooleanPropertyData& i_Data )
	{
		const int c_XPBL_Version = 0;
		o_Writer.WriteChunkHeader( c_XPBL, c_XPBL_Version, false );
		i_Data.m_Value.Write(o_Writer);
		i_Data.m_Name.Write(o_Writer);
		i_Data.m_Category.Write(o_Writer);
		i_Data.m_Description.Write(o_Writer);
		o_Writer.FinishChunk();
	}

	//========================================================================
	//   Float
	//========================================================================
	void ReadFloatPropertyData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					xtraFloatPropertyData& o_Data )
	{
		o_Data.m_Value.Read(i_Reader);
		o_Data.m_Name.Read(i_Reader);
		o_Data.m_Category.Read(i_Reader);
		o_Data.m_Description.Read(i_Reader);
		o_Data.m_Minimum.Read(i_Reader);
		o_Data.m_Maximum.Read(i_Reader);
	}
	void WriteFloatPropertyData(	chWriter& o_Writer,
					const xtraFloatPropertyData& i_Data )
	{
		const int c_XPFL_Version = 0;
		o_Writer.WriteChunkHeader( c_XPFL, c_XPFL_Version, false );
		i_Data.m_Value.Write(o_Writer);
		i_Data.m_Name.Write(o_Writer);
		i_Data.m_Category.Write(o_Writer);
		i_Data.m_Description.Write(o_Writer);
		i_Data.m_Minimum.Write(o_Writer);
		i_Data.m_Maximum.Write(o_Writer);
		o_Writer.FinishChunk();
	}

	//========================================================================
	//   Color
	//========================================================================
	void ReadColorPropertyData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					xtraColorPropertyData& o_Data )
	{
		o_Data.m_Value.Read(i_Reader);
		o_Data.m_Name.Read(i_Reader);
		o_Data.m_Category.Read(i_Reader);
		o_Data.m_Description.Read(i_Reader);
		o_Data.m_bShowAlpha.Read(i_Reader);
	}
	void WriteColorPropertyData(	chWriter& o_Writer,
					const xtraColorPropertyData& i_Data )
	{
		const int c_XPCL_Version = 0;
		o_Writer.WriteChunkHeader( c_XPCL, c_XPCL_Version, false );
		i_Data.m_Value.Write(o_Writer);
		i_Data.m_Name.Write(o_Writer);
		i_Data.m_Category.Write(o_Writer);
		i_Data.m_Description.Write(o_Writer);
		i_Data.m_bShowAlpha.Write(o_Writer);
		o_Writer.FinishChunk();
	}

	//========================================================================
	//   String
	//========================================================================
	void ReadStringPropertyData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					xtraStringPropertyData& o_Data )
	{
		o_Data.m_Value.Read(i_Reader);
		o_Data.m_Name.Read(i_Reader);
		o_Data.m_Category.Read(i_Reader);
		o_Data.m_Description.Read(i_Reader);
		o_Data.m_bMultiline.Read(i_Reader);
	}
	void WriteStringPropertyData(	chWriter& o_Writer,
					const xtraStringPropertyData& i_Data )
	{
		const int c_XPST_Version = 0;
		o_Writer.WriteChunkHeader( c_XPST, c_XPST_Version, false );
		i_Data.m_Value.Write(o_Writer);
		i_Data.m_Name.Write(o_Writer);
		i_Data.m_Category.Write(o_Writer);
		i_Data.m_Description.Write(o_Writer);
		i_Data.m_bMultiline.Write(o_Writer);
		o_Writer.FinishChunk();
	}

	//========================================================================
	//   Position
	//========================================================================
	void ReadPositionPropertyData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					xtraPositionPropertyData& o_Data )
	{
		o_Data.m_Value.Read(i_Reader);
		o_Data.m_Name.Read(i_Reader);
		o_Data.m_Category.Read(i_Reader);
		o_Data.m_Description.Read(i_Reader);
	}
	void WritePositionPropertyData(	chWriter& o_Writer,
					const xtraPositionPropertyData& i_Data )
	{
		const int c_XPPS_Version = 0;
		o_Writer.WriteChunkHeader( c_XPPS, c_XPPS_Version, false );
		i_Data.m_Value.Write(o_Writer);
		i_Data.m_Name.Write(o_Writer);
		i_Data.m_Category.Write(o_Writer);
		i_Data.m_Description.Write(o_Writer);
		o_Writer.FinishChunk();
	}
	
	//========================================================================
	//   Orientation
	//========================================================================
	void ReadOrientationPropertyData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					xtraOrientationPropertyData& o_Data )
	{
		o_Data.m_Value.Read(i_Reader);
		o_Data.m_Name.Read(i_Reader);
		o_Data.m_Category.Read(i_Reader);
		o_Data.m_Description.Read(i_Reader);
	}
	void WriteOrientationPropertyData(	chWriter& o_Writer,
					const xtraOrientationPropertyData& i_Data )
	{
		const int c_XPOR_Version = 0;
		o_Writer.WriteChunkHeader( c_XPOR, c_XPOR_Version, false );
		i_Data.m_Value.Write(o_Writer);
		i_Data.m_Name.Write(o_Writer);
		i_Data.m_Category.Write(o_Writer);
		i_Data.m_Description.Write(o_Writer);
		o_Writer.FinishChunk();
	}

	//========================================================================
	//   Texture
	//========================================================================
	void ReadTexturePropertyData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					xtraTexturePropertyData& o_Data )
	{
		o_Data.m_Value.Read(i_Reader);
		o_Data.m_Name.Read(i_Reader);
		o_Data.m_Category.Read(i_Reader);
		o_Data.m_Description.Read(i_Reader);
	}
	void WriteTexturePropertyData(	chWriter& o_Writer,
					const xtraTexturePropertyData& i_Data )
	{
		const int c_XPTX_Version = 0;
		o_Writer.WriteChunkHeader( c_XPTX, c_XPTX_Version, false );
		i_Data.m_Value.Write(o_Writer);
		i_Data.m_Name.Write(o_Writer);
		i_Data.m_Category.Write(o_Writer);
		i_Data.m_Description.Write(o_Writer);
		o_Writer.FinishChunk();
	}
}	// local namespace


//========================================================================
//   ReadCustomPropertyData
//========================================================================
void ReadCustomPropertyData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				std::vector<shared_ptr<xtraPropertyData>>& o_Properties )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if (name == c_XPBL)
		{
			xtraBooleanPropertyData *bool_prty = new xtraBooleanPropertyData();
			shared_ptr<xtraPropertyData> custom_prty(bool_prty);
			ReadBooleanPropertyData(i_Reader, version, size, *bool_prty);
			o_Properties.push_back(custom_prty);
		}
		else if (name == c_XPCL)
		{
			xtraColorPropertyData *float_prty = new xtraColorPropertyData();
			shared_ptr<xtraPropertyData> custom_prty(float_prty);
			ReadColorPropertyData(i_Reader, version, size, *float_prty);
			o_Properties.push_back(custom_prty);
		}
		else if (name == c_XPFL)
		{
			xtraFloatPropertyData *float_prty = new xtraFloatPropertyData();
			shared_ptr<xtraPropertyData> custom_prty(float_prty);
			ReadFloatPropertyData(i_Reader, version, size, *float_prty);
			o_Properties.push_back(custom_prty);
		}
		else if (name == c_XPPS)
		{
			xtraPositionPropertyData *float_prty = new xtraPositionPropertyData();
			shared_ptr<xtraPropertyData> custom_prty(float_prty);
			ReadPositionPropertyData(i_Reader, version, size, *float_prty);
			o_Properties.push_back(custom_prty);
		}
		else if (name == c_XPOR)
		{
			xtraOrientationPropertyData *float_prty = new xtraOrientationPropertyData();
			shared_ptr<xtraPropertyData> custom_prty(float_prty);
			ReadOrientationPropertyData(i_Reader, version, size, *float_prty);
			o_Properties.push_back(custom_prty);
		}
		else if (name == c_XPTX)
		{
			xtraTexturePropertyData *float_prty = new xtraTexturePropertyData();
			shared_ptr<xtraPropertyData> custom_prty(float_prty);
			ReadTexturePropertyData(i_Reader, version, size, *float_prty);
			o_Properties.push_back(custom_prty);
		}
		else if (name == c_XPST)
		{
			xtraStringPropertyData *float_prty = new xtraStringPropertyData();
			shared_ptr<xtraPropertyData> custom_prty(float_prty);
			ReadStringPropertyData(i_Reader, version, size, *float_prty);
			o_Properties.push_back(custom_prty);
		}
		i_Reader.FinishChunk();
	}
}

//========================================================================
//   WriteCustomPropertyData
//========================================================================
void WriteCustomPropertyData(	chWriter& o_Writer,
						const std::vector<shared_ptr<xtraPropertyData>>& i_Properties )
{
	int num_properties = i_Properties.size();
	//DBG_LOG("Num properties writing: " << num_properties);
	for (int i=0; i<num_properties; i++)
	{
		// Write property based on type
		DBG_ASSERT(i_Properties[i].get(), "WriteCustomPropertyData: Null property info");
		if (i_Properties[i]->m_Type == e_Boolean)
		{
			xtraBooleanPropertyData *pBoolPrty = dynamic_cast<xtraBooleanPropertyData*>(i_Properties[i].get());
			DBG_ASSERT(pBoolPrty, "Wrong data structure type for custom property");
			WriteBooleanPropertyData( o_Writer, *pBoolPrty );
		}
		else if (i_Properties[i]->m_Type == e_Color)
		{
			xtraColorPropertyData *pColorPrty = dynamic_cast<xtraColorPropertyData*>(i_Properties[i].get());
			DBG_ASSERT(pColorPrty, "Wrong data structure type for custom property");
			WriteColorPropertyData( o_Writer, *pColorPrty );
		}
		else if (i_Properties[i]->m_Type == e_Float)
		{
			xtraFloatPropertyData *pFloatPrty = dynamic_cast<xtraFloatPropertyData*>(i_Properties[i].get());
			DBG_ASSERT(pFloatPrty, "Wrong data structure type for custom property");
			WriteFloatPropertyData( o_Writer, *pFloatPrty );
		}
		else if (i_Properties[i]->m_Type == e_Position)
		{
			xtraPositionPropertyData *pPositionPrty = dynamic_cast<xtraPositionPropertyData*>(i_Properties[i].get());
			DBG_ASSERT(pPositionPrty, "Wrong data structure type for custom property");
			WritePositionPropertyData( o_Writer, *pPositionPrty );
		}
		else if (i_Properties[i]->m_Type == e_Orientation)
		{
			xtraOrientationPropertyData *pOrientationPrty = dynamic_cast<xtraOrientationPropertyData*>(i_Properties[i].get());
			DBG_ASSERT(pOrientationPrty, "Wrong data structure type for custom property");
			WriteOrientationPropertyData( o_Writer, *pOrientationPrty );
		}
		else if (i_Properties[i]->m_Type == e_Texture)
		{
			xtraTexturePropertyData *pTexturePrty = dynamic_cast<xtraTexturePropertyData*>(i_Properties[i].get());
			DBG_ASSERT(pTexturePrty, "Wrong data structure type for custom property");
			WriteTexturePropertyData( o_Writer, *pTexturePrty );
		}
		else if (i_Properties[i]->m_Type == e_String)
		{
			xtraStringPropertyData *pStringPrty = dynamic_cast<xtraStringPropertyData*>(i_Properties[i].get());
			DBG_ASSERT(pStringPrty, "Wrong data structure type for custom property");
			WriteStringPropertyData( o_Writer, *pStringPrty );
		}
		else
		{
			DBG_ASSERT(false, "Unrecognized property type: " << i_Properties[i]->m_Type);
		}
	}
}


}	// end of namespace

