/****************************************************************************\
**  fsXMLData.hpp
**
**	fsXMLData holds the node types shared by fsXMLReader and fsXMLParser.
**
**	Extra Large Technology
\****************************************************************************/

#ifdef FS_XMLDATA_HPP
#error fsXMLData.hpp multiply included
#endif
#define FS_XMLDATA_HPP


//============================================================================
//============================================================================
namespace fsXMLData
{
	enum fs_Node_Type
	{
		e_Element = 0,
		e_Text,
		e_EndElement,
		e_None,
		e_EOF
	};
}
