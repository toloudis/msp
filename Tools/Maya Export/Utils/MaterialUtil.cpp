/*****************************************************************************
**  MaterialUtil.cpp
**
**   Namespace for subdivision surface related functions. 
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <MaterialUtil.hpp>

#include "Core/Fs/fsFileUtil.hpp"
#include "Graphics/Eff/effPhongData.hpp"
#include "Graphics/mdl/mdlMatInfo.hpp"

#include <maya/MColor.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnSet.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MStringArray.h>

#include <vector>

namespace
{
	bool	bWriteMaterialDetails = false;

	//--------------------------------------------------------------------
	// Convert from Maya color to our color
	//--------------------------------------------------------------------
	maFloatRGBA conv_color(MColor &i_Col)
	{
		// MColor[] returns RGB color components
		return maFloatRGBA((float)i_Col[0], (float)i_Col[1], (float)i_Col[2], 1.0f);
	}

	//========================================================================
	// Given the shader set returned from mesh.getConnectedShaders(),
	// return an MObject for the surface shader from the set.
	//========================================================================
	MObject get_surface_shader(MObject &shader, MStatus &o_Status)
	{
		MFnSet set(shader, &o_Status);
		if (o_Status == MS::kSuccess) 
		{
			if (bWriteMaterialDetails)
				cout << "Material " << set.name() << endl;

			MObject plugObject;

			const char *c_ShaderNodeNames [] = { "surfaceShader", "miMaterialShader" };
			const int c_NumShaderNodeNames = 2;

			for (int si=0; si<c_NumShaderNodeNames; ++si)
			{
				MPlug plug = set.findPlug(c_ShaderNodeNames[si], &o_Status);
				if (o_Status == MS::kSuccess) 
				{
					MPlugArray connections;
					if (plug.connectedTo(connections, true, false, &o_Status)) 
					{
						if (o_Status == MS::kSuccess && connections.length() > 0) 
						{
							for (int j = 0; j < connections.length(); j++) 
							{
								MObject node = connections[j].node(&o_Status);
								MFnLambertShader shader(node, &o_Status);
								if (o_Status == MS::kSuccess) 
								{
									return node;
								}

								// Support some other types of surface shaders
								// by just looking for the name of the node
								// that is attached.
								MFnDependencyNode named_node(node, &o_Status);
								if (o_Status == MS::kSuccess) 
								{
									return node;
								}
							}
						}
					}
				}
			}
		}
		o_Status == MS::kFailure;
		return MObject();
	}

	//========================================================================
	//	get_texture_for_channel() - look on the channel with the given
	//	name and see if a texture filename node is an input to that
	//	plug. If so, get the full path filename to that texture.
	//  Returns true when it found a texture name.
	//========================================================================
	bool get_texture_for_channel(MFnLambertShader &shader, const char* pChannelName, fsLocator &o_TextureName)
	{
		MStatus status;
		MObject fileObject = MayaUtil::GetObjectConnectedToPlug(shader, pChannelName, status);
		if (status == MS::kSuccess) 
		{
			if (bWriteMaterialDetails)
				cout << "  " << pChannelName << " attr is connected to object of type " << fileObject.apiTypeStr() << endl;

			MFnDependencyNode dn(fileObject, &status);
			if (status == MS::kSuccess) 
			{
				if (bWriteMaterialDetails)
				{
					cout << "  can be DN of type " << dn.typeName() << endl;
					cout << "  name = " << dn.name() << endl;
				}

				MString fullTextureName = MayaUtil::GetTextureFileName(dn).asUTF8();

				if (fullTextureName.length() > 0)
				{
					//	check if the texture name has back-slashes instead of forward slashes
					//	for the directory breaks and split it up with the appropriate one.
					//
					MayaUtil::ConvertSlashes(fullTextureName);

					// Fullpath as fsLocator
					fsFileUtil::ANSIFilenameToLocator(fullTextureName.asUTF8(), o_TextureName);
				}
			}
		}
		return false;
	}

	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void AddToParams(const effPhongData& i_Phong, effShaderParams& o_Params)
	{
		o_Params.SetVersion(1);
		o_Params.AddParam(new effParamFloat("g_shininess", "g_shininess", i_Phong.m_SpecularPower));
		o_Params.AddParam(new effParamFloat("g_bumpMapScale", "g_bumpMapScale", i_Phong.m_BumpMapScale));
		o_Params.AddParam(new effParamFloat("g_reflectivity", "g_reflectivity", i_Phong.m_Reflectivity));
		float alpha = (i_Phong.m_Transparency < 1) ? i_Phong.m_Transparency : i_Phong.m_ColorDiffuse.GetAlpha();
		o_Params.AddParam(new effParamFloat("g_transparency", "g_transparency", alpha));
		o_Params.AddParam(new effParamColor("g_emissive", "g_emissive", i_Phong.m_ColorEmissive));
		o_Params.AddParam(new effParamColor("g_ambient", "g_ambient", i_Phong.m_ColorAmbient));
		o_Params.AddParam(new effParamColor("g_diffuse", "g_diffuse", i_Phong.m_ColorDiffuse));
		o_Params.AddParam(new effParamColor("g_specular", "g_specular", i_Phong.m_ColorSpecular));
		
		if (i_Phong.m_FullpathDiffuse.GetNumNames() > 0)
			o_Params.AddParam(new effParamTexture("diffuseMap", "diffuseMap", i_Phong.m_FullpathDiffuse));
		else
			o_Params.AddParam(new effParamTexture("diffuseMap", "diffuseMap", itString(i_Phong.m_NameDiffuse.c_str())));

		o_Params.AddParam(new effParamTexture("normalMap", "normalMap", itString(i_Phong.m_NameNormalMap.c_str())));
		o_Params.AddParam(new effParamTexture("cubeMap", "cubeMap", itString(i_Phong.m_NameEnvironment.c_str())));
		//o_Params.AddParam(new effParamTexture("specularMap", "specularMap", itString(i_Phong.m_NameSpecular.c_str())));
		o_Params.AddParam(new effParamTexture("glossMap", "glossMap", itString(i_Phong.m_NameGloss.c_str())));
		o_Params.AddParam(new effParamTexture("reflectFactorMap", "reflectFactorMap", itString(i_Phong.m_NameReflectFactorMap.c_str())));
		o_Params.AddParam(new effParamTexture("transparencyMap", "transparencyMap", itString(i_Phong.m_NameTransparencyMap.c_str())));
	}

}	// end of namespace



MaterialData::MaterialData()
{
	diffuse[0] = diffuse[1] = diffuse[2] = 0.5f;
	ambient[0] = ambient[1] = ambient[2] = 0.5f;
	specular[0] = specular[1] = specular[2] = 0.0f;
	power = 0.0f;
}

void MaterialData::Print()
{
	cout << "Material: " << name << endl
		 << "  [ " << diffuse[0] << " " << diffuse[1] << " " << diffuse[2] << " ]" << endl
		 << "  [ " << ambient[0] << " " << ambient[1] << " " << ambient[2] << " ]" << endl
		 << "  texture: " << texture << endl;
}


MaterialTable::~MaterialTable()
{
	Clear();
}

void MaterialTable::Clear()
{
	for(int i = 0; i < entries.size(); i++) {
		delete entries[i];
	}
	entries.clear();
}

bool MaterialTable::IsEmpty()
{
	return entries.empty();
}

int MaterialTable::Find(MString &name)
{
	for(int i = 0; i < entries.size(); i++) {
		if(name == entries[i]->name) {
			return i;
		}
	}
	return -1;
}

void MaterialTable::Print()
{
	cout << "Material Table -----------------------" << endl;
	for(int i = 0; i < entries.size(); i++) {
		entries[i]->Print();
	}
}


//========================================================================
//	GetMaterialData - get material info for this shader. The table
//		is used to share materials by name. The return value is 
//		an index into the table, a new material data structure
//		will be added to the table if needed.
//========================================================================
int MaterialUtil::GetMaterialData(MObject &shader, MaterialTable &table)
{
	MStatus status;
	int index = -1;

	MFnSet set(shader, &status);
	if (status == MS::kSuccess) {
		cout << "Material " << set.name() << endl;
		//cout << "Num polygons: " << polyCounts[i] << endl;

		MObject plugObject;
		MPlug plug = set.findPlug("surfaceShader", &status);

		if (status == MS::kSuccess) 
		{
			MPlugArray connections;
			
			if (plug.connectedTo(connections, true, false, &status)) 
			{
				if (status == MS::kSuccess && connections.length() > 0) 
				{
					for (int j = 0; j < connections.length(); j++) 
					{
						MObject node = connections[j].node(&status);
						MFnLambertShader shader(node, &status);
						if (status != MS::kSuccess) 
						{
							cout << " ** Cg shader **" << endl;
							//cout << node.apiTypeStr() << endl;
							
							MFnDependencyNode cg_shader(node, &status);
							if (status == MS::kSuccess) 
							{
								index = table.Find(MayaUtil::PrepareName(cg_shader.name()));

								if (index < 0) 
								{
									MString eff_name = MayaUtil::GetEffectFileName(cg_shader);
									//cout << "Effect name: " << eff_name << endl;

									MaterialData *material = new MaterialData;
									table.entries.push_back(material);
									material->name = MayaUtil::PrepareName(cg_shader.name());
									material->effect = eff_name;
								
									index = table.entries.size() - 1;
								}
							}
							else
								cout << "Get CgShader Node failed." << endl;
						}
						else 
						{
							index = table.Find(MayaUtil::PrepareName(shader.name()));

							if (index < 0) 
							{
								status = MS::kSuccess;
								cout << "  able to get Lambert:" << endl;
								cout << "  color = " << shader.color() << endl; 
								cout << "  ambient = " << shader.ambientColor() << endl;
							
								MaterialData *material = new MaterialData;
								table.entries.push_back(material);
								material->name = MayaUtil::PrepareName(shader.name());
								MColor diffuse = shader.color() * shader.diffuseCoeff();
								diffuse.get(material->diffuse);
								shader.ambientColor().get(material->ambient);

								
								MFnPhongShader phong_shader(node, &status);
								if (status == MS::kSuccess) 
								{
									// Get specular values, if phong
									//
									cout << "  able to get Phong:" << endl;
									phong_shader.specularColor().get(material->specular);
									material->power = phong_shader.cosPower();
								}
								else
								{
									// Default for specular when material is lambert
									//
									material->specular[0] = 0;
									material->specular[1] = 0;
									material->specular[2] = 0;
									material->power = 1.0;
								}
									
								MObject fileObject = MayaUtil::GetObjectConnectedToPlug(shader, "color", status);

								if (status == MS::kSuccess) 
								{
									cout << "   color attr is connected to object of type " << fileObject.apiTypeStr() << endl;
									MFnDependencyNode dn(fileObject, &status);
									if (status == MS::kSuccess) 
									{
										cout << "  can be DN of type " << dn.typeName() << endl;
										cout << "  name = " << dn.name() << endl;
									}
									material->texture = MayaUtil::GetTextureFileName(dn);

								}

								index = table.entries.size() - 1;
							}

						}
					}
				}
			}
		}
	}
	else 
	{
		cout << "Shader is not a set? API type: " << shader.apiTypeStr() << endl;
	}

	return index;
}

//========================================================================
//	GetMaterialData - get material info for this shader into a
//	LibXLT material structure.
//========================================================================
bool MaterialUtil::GetMaterialData(MObject &shader, mdlMaterialInfo &o_MatInfo)
{
	MStatus status;
	MObject surface_shader = get_surface_shader(shader, status);
	if (status == MS::kSuccess) 
	{
		MFnLambertShader shader(surface_shader, &status);
		if (status == MS::kSuccess) 
		{
			if (bWriteMaterialDetails)
			{
				cout << "  able to get Lambert:" << endl;
				cout << "  color = " << shader.color() << endl; 
				cout << "  ambient = " << shader.ambientColor() << endl;
			}
		
			o_MatInfo.SetMaterialName( MayaUtil::PrepareName(shader.name()).asUTF8() );
			shared_ptr<effPhongData> phong_data(new effPhongData());

			phong_data->m_ColorDiffuse = conv_color(shader.color() * shader.diffuseCoeff());
			// Force ambient to always be white
			//phong_data->m_ColorAmbient = conv_color(shader.ambientColor());
			phong_data->m_ColorAmbient.Set(1,1,1,1);
			phong_data->m_ColorEmissive.Set(0,0,0,1);

			// Additional texture layers possible...
			fsLocator specularMap;
			
			MFnPhongShader phong_shader(surface_shader, &status);
			if (status == MS::kSuccess) 
			{
				// Get specular values, if phong
				//
				if (bWriteMaterialDetails)
					cout << "  able to get Phong:" << endl;
				phong_data->m_ColorSpecular = conv_color(phong_shader.specularColor());
				phong_data->m_SpecularPower = phong_shader.cosPower();
			}
			else
			{
				phong_data->m_ColorSpecular.Set(0,0,0,1);
				phong_data->m_SpecularPower = 1.0f;
			}

			// Blinn attributes
			bool bBlinn = false;
			//float eccentricity = 0.3f;		// default from Maya
			float specularRollOff = 0.7f;	// default from Maya
			MFnBlinnShader blinn_shader(surface_shader, &status);
			if (status == MS::kSuccess) 
			{
				bBlinn = true;
				phong_data->m_ColorSpecular = conv_color(blinn_shader.specularColor());
				// apparently, eccentricity is the same as Shininess, which is exported through "SpecularPower"
				//eccentricity = blinn_shader.eccentricity();
				phong_data->m_SpecularPower = blinn_shader.eccentricity();
				specularRollOff = blinn_shader.specularRollOff();

				get_texture_for_channel(shader, "specularColor", specularMap);
			}

				
			MObject fileObject = MayaUtil::GetObjectConnectedToPlug(shader, "color", status);
			if (status == MS::kSuccess) 
			{
				if (bWriteMaterialDetails)
					cout << "   color attr is connected to object of type " << fileObject.apiTypeStr() << endl;

				MFnDependencyNode dn(fileObject, &status);
				if (status == MS::kSuccess) 
				{
					if (bWriteMaterialDetails)
					{
						cout << "  can be DN of type " << dn.typeName() << endl;
						cout << "  name = " << dn.name() << endl;
					}

					MString fullTextureName = MayaUtil::GetTextureFileName(dn).asUTF8();
					//	check if the texture name has back-slashes instead of forward slashes
					//	for the directory breaks and split it up with the appropriate one.
					//
					MayaUtil::ConvertSlashes(fullTextureName);

					//bga - This code took just the filename from the fullpath and 
					// stored this in the material data. Now, we are writing the
					// fullpath as a single itString.
					//MStringArray subs;
					//fullTextureName.split('/', subs);

					////	grab the filename only
					////
					//MString textureName = (subs.length() > 0) ? subs[subs.length() - 1] : fullTextureName;
					//phong_data->m_NameDiffuse = textureName.asUTF8();

					// Fullpath as fsLocator
					fsLocator tex_loc;
					fsFileUtil::ANSIFilenameToLocator(fullTextureName.asUTF8(), tex_loc);
					phong_data->m_FullpathDiffuse = tex_loc;

					// If we have a texture on the diffuse channel, then
					// force the color to be white
					phong_data->m_ColorDiffuse.Set(1,1,1,1);
				}
			}

			
			shared_ptr<effShaderParams> phong_params(new effShaderParams());
			//std::string shader_name = mdlMaterialLegacyParser::GetShaderNameForLegacyPhong(*phong_data);
			//phong_params->SetShaderName(itString(shader_name.c_str()));
			if (phong_data->m_FullpathDiffuse.GetNumNames() == 0)
				phong_params->SetShaderName(itString("Simple.fx"));
			else
				phong_params->SetShaderName(itString("Phong.fx"));
			// Add the phong parameters ourselves now
			//phong_data->AddToParams(*phong_params);
			AddToParams(*phong_data, *phong_params);

			//o_MatInfo.SetShader(shader_name, phong_data.get());
			o_MatInfo.SetShaderParams(phong_params);

			// handle blinn shader extra variables
			if (bBlinn)
			{
				phong_params->SetShaderName(itString("Blinn.fx"));
				//phong_params->AddParam(new effParamFloat("g_roughness", "g_roughness", eccentricity));
				phong_params->AddParam(new effParamFloat("g_IOR", "g_IOR", specularRollOff));
			}

			// Handle extra texture layers
			if (specularMap.GetNumNames() > 0)
				phong_params->AddParam(new effParamTexture("specularMap", "specularMap", specularMap));
			else
				phong_params->AddParam(new effParamTexture("specularMap", "specularMap", itString("")));


			return true;
		}
		
		// Fall back for surface shaders that are not lambert:
		MFnDependencyNode named_node(surface_shader, &status);
		if (status == MS::kSuccess) 
		{
			if (bWriteMaterialDetails)
			{
				cout << "  not lambert surface shader, using a default grey material" << endl;
			}
		
			o_MatInfo.SetMaterialName( MayaUtil::PrepareName(named_node.name()).asUTF8() );

			// Default grey
			shared_ptr<effPhongData> phong_data(new effPhongData());
			phong_data->m_ColorDiffuse.Set(1,1,1,1);
			phong_data->m_ColorAmbient.Set(1,1,1,1);
			phong_data->m_ColorEmissive.Set(0,0,0,1);
			phong_data->m_ColorSpecular.Set(0,0,0,1);
			phong_data->m_SpecularPower = 1.0f;

			shared_ptr<effShaderParams> phong_params(new effShaderParams());
			phong_params->SetShaderName(itString("Simple.fx"));
			phong_data->AddToParams(*phong_params);
			o_MatInfo.SetShaderParams(phong_params);
			return true;
		}
	}

	MFnSet set(shader, &status);
	if (status == MS::kSuccess)
	{
		cout << "Don't have surface shader node, using shading group name: " << set.name() << endl;
		o_MatInfo.SetMaterialName( MayaUtil::PrepareName(set.name()).asUTF8() );

		// Default grey
		shared_ptr<effPhongData> phong_data(new effPhongData());
		phong_data->m_ColorDiffuse.Set(1,1,1,1);
		phong_data->m_ColorAmbient.Set(1,1,1,1);
		phong_data->m_ColorEmissive.Set(0,0,0,1);
		phong_data->m_ColorSpecular.Set(0,0,0,1);
		phong_data->m_SpecularPower = 1.0f;

		shared_ptr<effShaderParams> phong_params(new effShaderParams());
		phong_params->SetShaderName(itString("Simple.fx"));
		phong_data->AddToParams(*phong_params);
		o_MatInfo.SetShaderParams(phong_params);
		return true;
	}

	return false;
}

//========================================================================
// Get material data from shader, adding it to the material table, 
// if needed. Returns name for material to use for this shader object, 
// or empty string if there was a problem.
//========================================================================
std::string MaterialUtil::ProcessMaterial(MObject &shader, 
										  mdlMatInfoTable& io_MaterialTable)
{
	shared_ptr<mdlMatInfo> material_info(new mdlMatInfo());
	if (GetMaterialData(shader, material_info->m_Info))
	{
		if (io_MaterialTable.find(material_info->m_Info.GetMaterialName()) == io_MaterialTable.end())
		{
			// If we don't have a material table entry for this name yet, add this one
			io_MaterialTable[material_info->m_Info.GetMaterialName()] = material_info;
		}
		return material_info->m_Info.GetMaterialName();
	}
	return "";
}

//========================================================================
// Return name for material to use for this shader object, or
// empty string if not found.
//========================================================================
std::string MaterialUtil::GetMaterialName(MObject &shader)
{
	MStatus status;
	MObject surface_shader = get_surface_shader(shader, status);
	if (status == MS::kSuccess) 
	{
		MFnDependencyNode shader_node(surface_shader, &status);
		return MayaUtil::PrepareName(shader_node.name()).asUTF8();
	}
	
	// If no shader node, just use shading group name
	MFnSet set(shader, &status);
	if (status == MS::kSuccess)
	{
		return MayaUtil::PrepareName(set.name()).asUTF8();
	}

	return "";
}