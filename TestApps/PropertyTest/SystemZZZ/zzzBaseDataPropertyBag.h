//
//		A base data BAG class
//
#ifdef ZZZ_BASEDATAPROPERTYBAG_H
#error zzzBaseDataPropertyBag.hpp multiply included
#endif
#define ZZZ_BASEDATAPROPERTYBAG_H

#ifndef ZZZ_BASEDATA_HPP
#include "zzzBaseData.hpp"
#endif

using namespace XLT::Windows::Forms;


//============================================================================
//============================================================================
__gc class zzzBaseDataBag : public PropertyBag
{
public:
	zzzBaseData& m_Data;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	zzzBaseDataBag(zzzBaseData& i_Data)
	:	m_Data(i_Data)
	{
		SetBagFromData(i_Data);

		this->add_GetValue( new PropertySpecEventHandler(this, OnGetValue) );
		this->add_SetValue( new PropertySpecEventHandler(this, OnSetValue) );
	}

	void SetBagFromData(const zzzBaseData& i_Data)
	{
		this->Properties->Clear();

		Type* newtype;
		PropertySpec* pPSpec;

		newtype = __typeof(float);
		pPSpec = new PropertySpec(S"FOV", newtype, S"Category 1", NULL, __box(i_Data.m_FOV));
		this->Properties->Add(pPSpec);
		pPSpec->set_DefaultValue(__box(i_Data.m_FOV));

		pPSpec = new PropertySpec(S"Near", newtype, S"Category 1", NULL, __box(i_Data.m_Near));
		this->Properties->Add(pPSpec);
		pPSpec->set_DefaultValue(__box(i_Data.m_Near));

		newtype = __typeof(Vector3Value);
		Vector3Value* pV3O = new Vector3Value( i_Data.m_Position.GetX(), i_Data.m_Position.GetY(), i_Data.m_Position.GetZ() );	// LEAK?
		pPSpec = new PropertySpec(S"Position", newtype, S"Category 1", NULL, pV3O,__typeof(TerawattManagedControls::Vector3Editor)->ToString(),__typeof(TerawattManagedControls::Vector3EditorConverter)->ToString());
		//pPSpec = new PropertySpec(S"Position", newtype, S"Category 1", NULL, pV3O, "TerawattManagedControls::Vector3Editor",__typeof(TerawattManagedControls::Vector3EditorConverter));
		pPSpec->set_DefaultValue(pV3O);
		this->Properties->Add(pPSpec);
	}

protected:
	/// <summary>
	/// This member overrides PropertyBag.OnGetValue.
	/// </summary>
	virtual void OnGetValue(PropertySpecEventArgs* e)
	{
		if (e->Property->Name->Equals(S"FOV"))
		{
			e->Value = e->Property->DefaultValue;
			//e->Value = __box(m_Data.m_FOV);
		}
		else if (e->Property->Name->Equals(S"Near"))
		{
			e->Value = e->Property->DefaultValue;
			//e->Value = __box(m_Data.m_Near);
		}
		else if (e->Property->Name->Equals(S"Position"))
		{
			e->Value = dynamic_cast<Vector3Value*>(e->Property->DefaultValue);
			//e->Value = (new Vector3Value(m_Data.m_Position.GetX(), m_Data.m_Position.GetY(), m_Data.m_Position.GetZ()));
		}
	}

	/// <summary>
	/// This member overrides PropertyBag.OnSetValue.
	/// </summary>
	virtual void OnSetValue(PropertySpecEventArgs* e)
	{
		if (e->Property->Name->Equals(S"FOV"))
		{
			e->Property->DefaultValue = e->Value;
	        m_Data.m_FOV	= *dynamic_cast<__box float*>(e->Value);
		}
		else if (e->Property->Name->Equals(S"Near"))
		{
			e->Property->DefaultValue = e->Value;
			m_Data.m_Near	= *dynamic_cast<__box float*>(e->Value);
		}
		else if (e->Property->Name->Equals(S"Position"))
		{
			e->Property->DefaultValue = e->Value;
			Vector3Value* pV3O = dynamic_cast<Vector3Value*>(e->Value);
			m_Data.m_Position.Set(pV3O->m_X, pV3O->m_Y, pV3O->m_Z );
		}
	}

private:
	void OnGetValue(Object* sender, PropertySpecEventArgs* e)
	{
		OnGetValue(e);
	}

	void OnSetValue(Object* sender, PropertySpecEventArgs* e)
	{
		OnSetValue(e);
	}

};

