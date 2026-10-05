/*****************************************************************************
**  fxEffectDesc.cpp
**
**      Reads an .effect.json manifest into an fxEffectDesc.
\****************************************************************************/

#include "GraphicsDX11/Fx/fxEffectDesc.hpp"

#include <nlohmann/json.hpp>

#include <cctype>
#include <stdexcept>

namespace
{
	using json = nlohmann::json;

	const char* k_StageKeys[fx_NumStages] = { "vs", "hs", "ds", "gs", "ps", "cs" };

	// Numbers in the manifest are plain JSON numbers; anything else (an
	// expression the converter could not evaluate) is an error.
	void flatten(const json& i_Value, std::vector<double>& o_Out)
	{
		if (i_Value.is_array())
		{
			for (const json& v : i_Value)
				flatten(v, o_Out);
		}
		else if (i_Value.is_boolean())
			o_Out.push_back(i_Value.get<bool>() ? 1.0 : 0.0);
		else if (i_Value.is_number())
			o_Out.push_back(i_Value.get<double>());
		else
			throw std::runtime_error("unsupported default value " + i_Value.dump());
	}

	std::string str(const json& i_Value)
	{
		return i_Value.is_string() ? i_Value.get<std::string>() : i_Value.dump();
	}

	float num(const json& i_Value)
	{
		if (!i_Value.is_number())
			throw std::runtime_error("expected a number, got " + i_Value.dump());
		return i_Value.get<float>();
	}

	fxSamplerDesc parse_sampler(const std::string& i_Name, const json& i_State)
	{
		fxSamplerDesc s;
		s.m_Name = i_Name;
		for (auto it = i_State.begin(); it != i_State.end(); ++it)
		{
			std::string key = it.key();
			for (char& c : key)
				c = (char)tolower((unsigned char)c);
			const json& v = it.value();

			if (key == "filter")				s.m_Filter = str(v);
			else if (key == "addressu")			s.m_Address[0] = str(v);
			else if (key == "addressv")			s.m_Address[1] = str(v);
			else if (key == "addressw")			s.m_Address[2] = str(v);
			else if (key == "miplodbias")		s.m_MipLODBias = num(v);
			else if (key == "maxanisotropy")	s.m_MaxAnisotropy = (int)num(v);
			else if (key == "comparisonfunc")	s.m_ComparisonFunc = str(v);
			else if (key == "minlod")			s.m_MinLOD = num(v);
			else if (key == "maxlod")			s.m_MaxLOD = num(v);
			else if (key == "bordercolor")
			{
				std::vector<double> c;
				flatten(v, c);
				for (size_t i = 0; i < 4 && i < c.size(); i++)
					s.m_BorderColor[i] = (float)c[i];
			}
			else
				throw std::runtime_error("sampler " + i_Name + ": unknown state '" + it.key() + "'");
		}
		return s;
	}
}

//------------------------------------------------------------------------
// Parse()
//------------------------------------------------------------------------
bool fxEffectDesc::Parse(const char* i_Json, fxEffectDesc& o_Desc, std::string& o_Error)
{
	try
	{
		json root = json::parse(i_Json);
		fxEffectDesc desc;
		desc.m_Source = root.value("source", "");
		desc.m_Hlsl = root.value("hlsl", "");

		for (const json& t : root.at("techniques"))
		{
			fxTechniqueDesc tech;
			tech.m_Name = t.at("name").get<std::string>();
			for (const json& p : t.at("passes"))
			{
				fxPassDesc pass;
				pass.m_Name = p.at("name").get<std::string>();
				const json& stages = p.at("stages");
				for (int s = 0; s < fx_NumStages; s++)
				{
					auto it = stages.find(k_StageKeys[s]);
					if (it == stages.end() || it->is_null())
						continue;	// absent or explicitly NULL: no shader on this stage
					pass.m_Stages[s].m_Entry = it->at("entry").get<std::string>();
					pass.m_Stages[s].m_Profile = it->at("profile").get<std::string>();
				}
				tech.m_Passes.push_back(pass);
			}
			desc.m_Techniques.push_back(tech);
		}

		if (root.contains("samplers"))
		{
			const json& samplers = root.at("samplers");
			for (auto it = samplers.begin(); it != samplers.end(); ++it)
			{
				static const json k_Empty = json::object();
				const json& state = it.value().contains("state") ? it.value().at("state") : k_Empty;
				desc.m_Samplers.push_back(parse_sampler(it.key(), state));
			}
		}

		if (root.contains("variables"))
		{
			const json& vars = root.at("variables");
			for (auto it = vars.begin(); it != vars.end(); ++it)
			{
				fxVariableDesc var;
				var.m_Name = it.key();
				var.m_Type = it.value().value("type", "");
				var.m_Semantic = it.value().value("semantic", "");
				if (it.value().contains("default"))
				{
					const json& def = it.value().at("default");
					flatten(def, var.m_Default);
					if (def.is_array() && !def.empty() && def[0].is_array())
						var.m_DefaultComponentsPerElement = (int)def[0].size();
				}
				desc.m_Variables.push_back(var);
			}
		}

		o_Desc = desc;
		return true;
	}
	catch (const std::exception& e)
	{
		o_Error = e.what();
		return false;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
const fxSamplerDesc* fxEffectDesc::FindSampler(const std::string& i_Name) const
{
	for (const fxSamplerDesc& s : m_Samplers)
		if (s.m_Name == i_Name)
			return &s;
	return nullptr;
}

const fxVariableDesc* fxEffectDesc::FindVariable(const std::string& i_Name) const
{
	for (const fxVariableDesc& v : m_Variables)
		if (v.m_Name == i_Name)
			return &v;
	return nullptr;
}
