//
//		A base data TABLE class
//
#ifdef XXX_BASEDATAPROPERTYTABLE_H
#error xxxBaseDataPropertyTable.hpp multiply included
#endif
#define XXX_BASEDATAPROPERTYTABLE_H

#ifndef XXX_BASEDATA_HPP
#include "xxxBaseData.hpp"
#endif


//============================================================================
//============================================================================
using namespace XLT::Windows::Forms;
using namespace TerawattManagedControls;


//============================================================================
//============================================================================
__gc class xxxBaseDataTable : public PropertyTable
{
public:
	xxxBaseData& m_Data;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	xxxBaseDataTable(xxxBaseData& i_Data)
	:	m_Data(i_Data)
	{
		SetTableFromData(i_Data);
	}

	void SetTableFromData(const xxxBaseData& i_Data)
	{
		this->Properties->Clear();

		Type* newtype;
		PropertySpec* pPSpec;
		newtype = __typeof(System::Boolean);
		pPSpec = new PropertySpec(S"Editor Visible", newtype, NULL, NULL, __box(i_Data.m_bEditorVisible));
		this->Properties->Add(pPSpec);
		this->SetObjectValue(S"Editor Visible", __box(i_Data.m_bEditorVisible));

		newtype = __typeof(float);
		pPSpec = new PropertySpec(S"FOV", newtype, S"Category 1", NULL, __box(i_Data.m_FOV));
		this->Properties->Add(pPSpec);
		this->SetObjectValue(S"FOV", __box(i_Data.m_FOV));

		pPSpec = new PropertySpec(S"Tilt", newtype, S"Category 1", NULL, __box(i_Data.m_Tilt));
		this->Properties->Add(pPSpec);
		this->SetObjectValue(S"Tilt", __box(i_Data.m_Tilt));

		pPSpec = new PropertySpec(S"Near", newtype, S"Category 1", NULL, __box(i_Data.m_Near));
		this->Properties->Add(pPSpec);
		this->SetObjectValue(S"Near", __box(i_Data.m_Near));

		pPSpec = new PropertySpec(S"Far", newtype, S"Category 1", NULL, __box(i_Data.m_Far));
		this->Properties->Add(pPSpec);
		this->SetObjectValue(S"Far", __box(i_Data.m_Far));
	}

protected:
	/// <summary>
	/// This member overrides PropertyBag.OnGetValue.
	/// </summary>
	virtual void OnGetValue(PropertySpecEventArgs* e)
	{
		PropertyTable::OnGetValue(e);

		//if (e->Property->Name->Equals(S"Editor Visible"))
		//{
		//	m_Data.m_bEditorVisible = *dynamic_cast<__box bool*>(this->GetObjectValue(S"Editor Visible"));
		//}
		//else if (e->Property->Name->Equals(S"FOV"))
		//{
		//	m_Data.m_FOV	= *dynamic_cast<__box float*>(this->GetObjectValue(S"FOV"));
		//}
		//else if (e->Property->Name->Equals(S"Tilt"))
		//{
		//	m_Data.m_Tilt	= *dynamic_cast<__box float*>(this->GetObjectValue(S"Tilt"));
		//}
		//else if (e->Property->Name->Equals(S"Near"))
		//{
		//	m_Data.m_Near	= *dynamic_cast<__box float*>(this->GetObjectValue(S"Near"));
		//}
		//else if (e->Property->Name->Equals(S"Far"))
		//{
		//	m_Data.m_Far	= *dynamic_cast<__box float*>(this->GetObjectValue(S"Far"));
		//}
	}

	/// <summary>
	/// This member overrides PropertyBag.OnSetValue.
	/// </summary>
	virtual void OnSetValue(PropertySpecEventArgs* e)
	{
		PropertyTable::OnSetValue(e);

		if (e->Property->Name->Equals(S"Editor Visible"))
		{
			m_Data.m_bEditorVisible = *dynamic_cast<__box bool*>(this->GetObjectValue(S"Editor Visible"));
		}
		else if (e->Property->Name->Equals(S"FOV"))
		{
	        m_Data.m_FOV	= *dynamic_cast<__box float*>(this->GetObjectValue(S"FOV"));
		}
		else if (e->Property->Name->Equals(S"Tilt"))
		{
	        m_Data.m_Tilt	= *dynamic_cast<__box float*>(this->GetObjectValue(S"Tilt"));
		}
		else if (e->Property->Name->Equals(S"Near"))
		{
			m_Data.m_Near	= *dynamic_cast<__box float*>(this->GetObjectValue(S"Near"));
		}
		else if (e->Property->Name->Equals(S"Far"))
		{
			m_Data.m_Far	= *dynamic_cast<__box float*>(this->GetObjectValue(S"Far"));
		}
	}
};

