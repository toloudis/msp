/*****************************************************************************
**  MaterialUtil.hpp
**
**   Namespace for subdivision surface related functions.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef MATERIALUTIL_HPP
#error MaterialUtil.hpp multiply included
#endif
#define MATERIALUTIL_HPP


#ifndef MDL_MATINFOTABLE_HPP
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#endif 

class MFnSubd;
class MPointArray;
class mdlMaterialInfo;

#include <vector>

// These were just local classes in SceneFuncs, moved here to allow more Utils
// to access Material Info 
class MaterialData {
public:
	MString name;
	float diffuse[3];
	float ambient[3];
	float specular[3];
	float power;
	MString texture;
	MString effect;

	MaterialData();

	void Print();
};

class MaterialTable {
public:
	std::vector<MaterialData *> entries;

	MaterialTable () {}
	~MaterialTable();
	void Clear();
	bool IsEmpty();
	int Find(MString &name);
	void Print();
};

namespace MaterialUtil
{

	//========================================================================
	//	GetMaterialData - get material info for this shader. The table
	//		is used to share materials by name. The return value is 
	//		an index into the table, a new material data structure
	//		will be added to the table if needed.
	//========================================================================
	int GetMaterialData(MObject &shader, MaterialTable &table);

	//========================================================================
	//	GetMaterialData - get material info for this shader into a
	//	LibXLT material structure.
	//========================================================================
	bool GetMaterialData(MObject &shader, mdlMaterialInfo &o_MatInfo);

	//========================================================================
	// Get material data from shader, adding it to the material table, 
	// if needed. Returns name for material to use for this shader object, 
	// or empty string if there was a problem.
	//========================================================================
	std::string ProcessMaterial(MObject &shader, 
								mdlMatInfoTable& io_MaterialTable);

	//========================================================================
	// Return name for material to use for this shader object, or
	// empty string if not found.
	//========================================================================
	std::string GetMaterialName(MObject &shader);

}


