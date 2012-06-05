/****************************************************************************\
**  chtrExpressionPropertyObject.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Expressions/chtrExpressionPropertyObject.hpp"

#include "Support/fsys/fsysFileList.hpp"
#include "Systems/Character/GUI/chtrAnimList.hpp"
#include "Systems/Character/GUI/chtrDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Graphics/ent/entAnimation.hpp"
#include "Graphics/ent/entAnimKeys.hpp"
#include "Graphics/ent/entEntity.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
chtrExpressionPropertyObject::chtrExpressionPropertyObject(pick3dPickObject* i_pParent, api3dObjectEntity* i_pObject, const fsLocator& i_ModelPath)
:	m_pParent(i_pParent),
	m_pObject(i_pObject),
	m_ObjectModelPath(i_ModelPath),
	m_pChannel(NULL)
{
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrExpressionPropertyObject::~chtrExpressionPropertyObject()
{
	const int nExpressions = m_Expressions.size();
	for (int e=0; e<nExpressions; e++)
	{
		DeleteExpressionAnimation( m_Expressions[e] );
	}
	m_Expressions.clear();
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void chtrExpressionPropertyObject::SetChannel( tmlnChannelRangedFloat * i_pChannel )
{
	m_pChannel = i_pChannel;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
tmlnChannelRangedFloat * chtrExpressionPropertyObject::GetChannel()
{
	return m_pChannel;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void chtrExpressionPropertyObject::CreateChannel(const std::string& i_Name, prtyFloat& i_Weight)
{
	if (m_pChannel != NULL)
	{
		DBG_ERROR("Creating a channel for an expression but there already is one");
		delete m_pChannel;
	}

	m_pChannel = new tmlnChannelRangedFloatProperty( i_Name.c_str(), i_Weight );
}

//------------------------------------------------------------------------
//	Create a channel
//------------------------------------------------------------------------
void chtrExpressionPropertyObject::CreateChannel()
{
	// only children use this
}



//============================================================================
// pick3dPickObject - virtual function overrides
//============================================================================

//--------------------------------------------------------------------
// Set Parent pointer to use when creating selectable property objects
//--------------------------------------------------------------------
void chtrExpressionPropertyObject::SetParent(pick3dPickObject* i_pParent)
{
	m_pParent = i_pParent;
}

//------------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//------------------------------------------------------------------------
//virtual 
pick3dPickObject* chtrExpressionPropertyObject::GetParentObject() const
{
	return m_pParent;
}

//------------------------------------------------------------------------
// Get the name of the object for display when selected
//------------------------------------------------------------------------
std::string chtrExpressionPropertyObject::GetPick3dName() const
{
	return this->GetName();
}

//--------------------------------------------------------------------
//	AddExpression loads sub-animation as expression that can then
//	be blended with an alpha from 0-1
//--------------------------------------------------------------------
void chtrExpressionPropertyObject::AddExpression(const std::string &i_Name,
												 const itString& i_AnimFileName)
{
	//	load the expression
	ExpressionAnim expression(NULL, NULL, NULL);
	try
	{
		UpdateExpressionAnimation( i_AnimFileName, expression );
	}
	catch ( fsInvalidLocatorX& /*i_Ex*/ )
	{
		return;
	}
	m_Expressions.push_back(expression);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrExpressionPropertyObject::DeleteExpressionAnimation( ExpressionAnim& i_Expression )
{
	//entSubAnimation pointers remain owned by the entity/object
	//delete i_Expression.m_pAnimInstance;

	// animation pointer itself needs to be deleted
	delete i_Expression.m_pAnimation;
	i_Expression.m_pAnimation = NULL;

	// keys need to be deleted here
	delete i_Expression.m_pAnimKeys;
	i_Expression.m_pAnimKeys = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrExpressionPropertyObject::UpdateExpressionAnimation(	const itString& i_AnimFileName,
																ExpressionAnim& o_Expression )
{
	if (i_AnimFileName.GetLength() > 0)
	{
		fsLocator animfile = this->m_ObjectModelPath;
		animfile.Push( i_AnimFileName );

		//DBG_LOG("Adding Expression (" << i_AnimFileName << ")";

		entAnimKeys* anim_keys = NULL;
		try
		{
			anim_keys = entImport::LoadAnimKeys( animfile );
		}
		catch ( fsInvalidLocatorX& i_Ex )
		{
			DBG_ERROR("Can't find the expression " << animfile);
			throw fsInvalidLocatorX(i_Ex);	// send it up.
			return;
		}

		entAnimation* animation = entImport::CreateAnimation(*anim_keys);

		const float start_time = 0.0f;
		const bool bPreserve = true;
		entSubAnimation *sub_anim = m_pObject->GetEntity()->AddSubAnimation(animation, start_time, bPreserve);
		
		m_pObject->GetEntity()->SetSubAnimationBlend(sub_anim, 0.0f);	// Start with 0.0 weight for expressions
	
		o_Expression.m_pAnimation = animation;
		o_Expression.m_pAnimInstance = sub_anim;
		o_Expression.m_pAnimKeys = anim_keys;
	}
	else
	{
		o_Expression.m_pAnimation = NULL;
		o_Expression.m_pAnimInstance = NULL;
		o_Expression.m_pAnimKeys = NULL;
	}
}


//
//	Single Expression Property Object
//


//------------------------------------------------------------------------
//------------------------------------------------------------------------
chtrSingleExpressionPropertyObject::chtrSingleExpressionPropertyObject(pick3dPickObject* i_pParent, api3dObjectEntity* i_pObject, const fsLocator& i_ModelPath, const itString &i_ExpressionAnimFileName)
:	chtrExpressionPropertyObject( i_pParent, i_pObject, i_ModelPath )
{
	this->m_Data.m_FileName.SetValue( i_ExpressionAnimFileName );

	ConfigureProperties();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrSingleExpressionPropertyObject::~chtrSingleExpressionPropertyObject()
{
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void chtrSingleExpressionPropertyObject::SetData(const chtrExpressionData &i_Data)
{
	const chtrSingleExpressionData* pSEData = dynamic_cast<const chtrSingleExpressionData*>(&i_Data);
	if (pSEData != NULL)
	{
		m_Data = *pSEData;
		GetChannel()->SetOriginalValue(pSEData->m_Weight.GetValue());
	}
}

//------------------------------------------------------------------------
//	Get access to the data
//------------------------------------------------------------------------
const chtrSingleExpressionData& chtrSingleExpressionPropertyObject::GetData()
{
	return this->m_Data;
}
chtrSingleExpressionData& chtrSingleExpressionPropertyObject::Data()
{
	return this->m_Data;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
chtrExpressionData* chtrSingleExpressionPropertyObject::CloneOriginalData()
{
	chtrExpressionData* pED = this->m_Data.Clone();
	pED->m_Weight.SetValue( this->GetChannel()->GetOriginalValue() );
	return pED;
}

//------------------------------------------------------------------------
//	Create a channel
//------------------------------------------------------------------------
void chtrSingleExpressionPropertyObject::CreateChannel()
{
	chtrExpressionPropertyObject::CreateChannel( m_Data.m_Name.GetValue().c_str(), m_Data.m_Weight );
}

//------------------------------------------------------------------------
//	ConfigureProperties - setup UIInfos and callbacks
//------------------------------------------------------------------------
void chtrSingleExpressionPropertyObject::ConfigureProperties()
{
	prtyTextBoxUIInfo* pTBUII = NULL;
	pTBUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Expression", "Name for the Expression");
	AddProperty(pTBUII);

	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_Weight), "Expression", "Weight of Expression");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(200);
	AddProperty( pRFUII );

	prtyComboBoxUIInfo* pFPUII = NULL;
	pFPUII = new prtyComboBoxUIInfo(&(m_Data.m_FileName), "Expression", "Animation File");

	// fill in the control
	if (m_pParent != NULL)
	{
		fsysFileList fileList;
		chtrAnimList::BuildFileList(fileList, m_ObjectModelPath);

		for (int i=0 ; i < fileList.Size(); ++i)
		{
			std::string fname;
			fname = itStringUtil::GetStdString( fileList.GetFilename(i) );
			pFPUII->AddItem( fname );
		}
	}

	AddProperty( pFPUII );

	//	Add the callbacks
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<chtrSingleExpressionPropertyObject>(this, &chtrSingleExpressionPropertyObject::NameChanged));
	m_Data.m_Weight.AddCallback(new prtyCallbackWrapper<chtrSingleExpressionPropertyObject>(this, &chtrSingleExpressionPropertyObject::WeightChanged));
	m_Data.m_FileName.AddCallback(new prtyCallbackWrapper<chtrSingleExpressionPropertyObject>(this, &chtrSingleExpressionPropertyObject::FileNameChanged));
}

//----------------------------------------------------------------------------
// property callbacks
//----------------------------------------------------------------------------
void chtrSingleExpressionPropertyObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_Data.m_Name.GetValue();

	if (m_pChannel != NULL)
	{
		m_pChannel->SetName( m_Data.m_Name.GetValue().c_str() );
	}
	
	chtrDialogUtil::UpdateListDialog();
 	//chtrObject* pSO = dynamic_cast<chtrObject*>(m_pParent);
 	//chtrExpressionsDialogUtil::UpdateExpressions( m_pParent );
}

void chtrSingleExpressionPropertyObject::WeightChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	prtyFloat* pPrtyF = dynamic_cast<prtyFloat*>(i_pProperty);
	if (pPrtyF != NULL)
	{
		m_pObject->GetEntity()->SetSubAnimationBlend(m_Expressions[0].m_pAnimInstance, 
													 pPrtyF->GetValue());
	}
}

void chtrSingleExpressionPropertyObject::FileNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (m_Expressions.size() > 0)
		DeleteExpressionAnimation( m_Expressions[0] );

	prtyFileName* pPrtyFN = dynamic_cast<prtyFileName*>(i_pProperty);
	if (pPrtyFN != NULL)
	{
		try
		{
			UpdateExpressionAnimation( pPrtyFN->GetValue(), m_Expressions[0]);
		}
		catch ( fsInvalidLocatorX& /*i_Ex*/ )
		{
			return;
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrSingleExpressionPropertyObject::DeleteExpressionAnimation( ExpressionAnim& i_Expression )
{
	//entSubAnimation pointers remain owned by the entity/object
	//delete i_Expression.m_pAnimInstance;

	m_pObject->GetEntity()->RemoveSubAnimation(i_Expression.m_pAnimInstance);
	//m_pObject->GetEntity()->ClearSubAnimations();

	// animation pointer itself needs to be deleted
	delete i_Expression.m_pAnimation;
	i_Expression.m_pAnimation = NULL;

	// keys need to be deleted here
	delete i_Expression.m_pAnimKeys;
	i_Expression.m_pAnimKeys = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrSingleExpressionPropertyObject::UpdateExpressionAnimation(	const itString& i_AnimFileName,
																	ExpressionAnim& o_Expression )
{
	if (i_AnimFileName.GetLength() > 0)
	{
		fsLocator animfile = this->m_ObjectModelPath;
		animfile.Push( i_AnimFileName );

		//DBG_LOG("Adding Expression (" << i_AnimFileName << ")";

		entAnimKeys* anim_keys = NULL;
		try
		{
			anim_keys = entImport::LoadAnimKeys( animfile );
		}
		catch ( fsInvalidLocatorX& i_Ex )
		{
			DBG_ERROR("Can't find the expression " << animfile);
			throw fsInvalidLocatorX(i_Ex);	// send it up.
			return;
		}

		entAnimation* animation = entImport::CreateAnimation(*anim_keys);

		const float start_time = 0.0f;
		const bool bPreserve = true;
		entSubAnimation *sub_anim = m_pObject->GetEntity()->AddSubAnimation(animation, start_time, bPreserve);
		m_pObject->GetEntity()->SetSubAnimationBlend(sub_anim, 0.0f);	// Start with 0.0 weight for expressions
	
		o_Expression.m_pAnimation = animation;
		o_Expression.m_pAnimInstance = sub_anim;
		o_Expression.m_pAnimKeys = anim_keys;
	}
	else
	{
		o_Expression.m_pAnimation = NULL;
		o_Expression.m_pAnimInstance = NULL;
		o_Expression.m_pAnimKeys = NULL;
	}
}




//
//	Dual Expression Property Object
//


//------------------------------------------------------------------------
//------------------------------------------------------------------------
chtrDualExpressionPropertyObject::chtrDualExpressionPropertyObject(pick3dPickObject* i_pParent, api3dObjectEntity* i_pObject, const fsLocator& i_ModelPath, const itString &i_ExpressionAnimFileNameLeft, const itString &i_ExpressionAnimFileNameRight )
:	chtrExpressionPropertyObject( i_pParent, i_pObject, i_ModelPath )
{
	this->m_Data.m_FileNameLeft.SetValue( i_ExpressionAnimFileNameLeft );
	this->m_Data.m_FileNameRight.SetValue( i_ExpressionAnimFileNameRight );

	ConfigureProperties();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrDualExpressionPropertyObject::~chtrDualExpressionPropertyObject()
{
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void chtrDualExpressionPropertyObject::SetData(const chtrExpressionData &i_Data)
{
	const chtrDualExpressionData* pMEData = dynamic_cast<const chtrDualExpressionData*>(&i_Data);
	if (pMEData != NULL)
	{
		m_Data = *pMEData;
		GetChannel()->SetOriginalValue(pMEData->m_Weight.GetValue());
	}
}

//------------------------------------------------------------------------
//	Get access to the data
//------------------------------------------------------------------------
const chtrDualExpressionData& chtrDualExpressionPropertyObject::GetData()
{
	return this->m_Data;
}
chtrDualExpressionData& chtrDualExpressionPropertyObject::Data()
{
	return this->m_Data;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
chtrExpressionData* chtrDualExpressionPropertyObject::CloneOriginalData()
{
	chtrExpressionData* pED = this->m_Data.Clone();
	pED->m_Weight.SetValue( this->GetChannel()->GetOriginalValue() );
	return pED;
}

//------------------------------------------------------------------------
//	Create a channel
//------------------------------------------------------------------------
void chtrDualExpressionPropertyObject::CreateChannel()
{
	chtrExpressionPropertyObject::CreateChannel( m_Data.m_Name.GetValue().c_str(), m_Data.m_Weight );
}

//------------------------------------------------------------------------
//	ConfigureProperties - setup UIInfos and callbacks
//------------------------------------------------------------------------
void chtrDualExpressionPropertyObject::ConfigureProperties()
{
	prtyTextBoxUIInfo* pTBUII = NULL;
	pTBUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Expression", "Name for the Expression");
	AddProperty(pTBUII);

	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_Weight), "Expression", "Weight of Expression");
	pRFUII->SetMinimum(-1.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(200);
	AddProperty( pRFUII );

	prtyComboBoxUIInfo* pFPUII_1 = NULL;
	prtyComboBoxUIInfo* pFPUII_2 = NULL;
	pFPUII_1 = new prtyComboBoxUIInfo(&(m_Data.m_FileNameLeft), "Expression", "Animation File 1");
	pFPUII_2 = new prtyComboBoxUIInfo(&(m_Data.m_FileNameRight), "Expression", "Animation File 2");

	// fill in the control
	if (m_pParent != NULL)
	{
		fsysFileList fileList;
		chtrAnimList::BuildFileList(fileList, m_ObjectModelPath);

		for (int i=0 ; i < fileList.Size(); ++i)
		{
			std::string fname;
			fname = itStringUtil::GetStdString( fileList.GetFilename(i) );
			pFPUII_1->AddItem( fname );
			pFPUII_2->AddItem( fname );
		}
	}

	AddProperty( pFPUII_1 );
	AddProperty( pFPUII_2 );

	//	Add the callbacks
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<chtrDualExpressionPropertyObject>(this, &chtrDualExpressionPropertyObject::NameChanged));
	m_Data.m_Weight.AddCallback(new prtyCallbackWrapper<chtrDualExpressionPropertyObject>(this, &chtrDualExpressionPropertyObject::WeightChanged));
	m_Data.m_FileNameRight.AddCallback(new prtyCallbackWrapper<chtrDualExpressionPropertyObject>(this, &chtrDualExpressionPropertyObject::FileNameRightChanged));
	m_Data.m_FileNameLeft.AddCallback(new prtyCallbackWrapper<chtrDualExpressionPropertyObject>(this, &chtrDualExpressionPropertyObject::FileNameLeftChanged));
}

//----------------------------------------------------------------------------
// property callbacks
//----------------------------------------------------------------------------
void chtrDualExpressionPropertyObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_Data.m_Name.GetValue();

	if (m_pChannel != NULL)
	{
		m_pChannel->SetName( m_Data.m_Name.GetValue().c_str() );
	}
	
	chtrDialogUtil::UpdateListDialog();
 	//chtrObject* pSO = dynamic_cast<chtrObject*>(m_pParent);
 	//chtrExpressionsDialogUtil::UpdateExpressions( m_pParent );
}

void chtrDualExpressionPropertyObject::WeightChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	prtyFloat* pPrtyF = dynamic_cast<prtyFloat*>(i_pProperty);
	if (pPrtyF != NULL)
	{
		float weight = pPrtyF->GetValue();
		const int left = 0;
		const int right = 1;
		entEntity* pEntity = m_pObject->GetEntity();
		if (weight < 0)
		{
			pEntity->SetSubAnimationBlend(m_Expressions[left].m_pAnimInstance, -weight);	
			pEntity->SetSubAnimationBlend(m_Expressions[right].m_pAnimInstance, 0);	
		}
		else
		{
			pEntity->SetSubAnimationBlend(m_Expressions[left].m_pAnimInstance, 0);	
			pEntity->SetSubAnimationBlend(m_Expressions[right].m_pAnimInstance, weight);	
		}
	}
}

void chtrDualExpressionPropertyObject::FileNameRightChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	DeleteExpressionAnimation( m_Expressions[1] );

	prtyFileName* pPrtyFN = dynamic_cast<prtyFileName*>(i_pProperty);
	if (pPrtyFN != NULL)
	{
		try
		{
			UpdateExpressionAnimation( pPrtyFN->GetValue(), m_Expressions[1]);
		}
		catch ( fsInvalidLocatorX& /*i_Ex*/ )
		{
			return;
		}
	}
}

void chtrDualExpressionPropertyObject::FileNameLeftChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	DeleteExpressionAnimation( m_Expressions[0] );

	prtyFileName* pPrtyFN = dynamic_cast<prtyFileName*>(i_pProperty);
	if (pPrtyFN != NULL)
	{
		try
		{
			UpdateExpressionAnimation( pPrtyFN->GetValue(), m_Expressions[0]);
		}
		catch ( fsInvalidLocatorX& /*i_Ex*/ )
		{
			return;
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrDualExpressionPropertyObject::DeleteExpressionAnimation( ExpressionAnim& i_Expression )
{
	//entSubAnimation pointers remain owned by the entity/object
	//delete i_Expression.m_pAnimInstance;

	//m_pObject->GetEntity()->RemoveSubAnimation(i_Expression.m_pAnimation);
	m_pObject->GetEntity()->ClearSubAnimations();

	// animation pointer itself needs to be deleted
	delete i_Expression.m_pAnimation;
	i_Expression.m_pAnimation = NULL;

	// keys need to be deleted here
	delete i_Expression.m_pAnimKeys;
	i_Expression.m_pAnimKeys = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrDualExpressionPropertyObject::UpdateExpressionAnimation(	const itString& i_AnimFileName,
																		ExpressionAnim& o_Expression )
{
	if (i_AnimFileName.GetLength() > 0)
	{
		fsLocator animfile = this->m_ObjectModelPath;
		animfile.Push( i_AnimFileName );

		//	debug only
		//std::string animfilestr;
		//fsFileUtil::LocatorToANSIFilename( animfile, animfilestr );
		//DBG_LOG2("Adding Expression (%s) (%s)", i_Name.c_str(), animfilestr.c_str());

		entAnimKeys* anim_keys = NULL;
		try
		{
			anim_keys = entImport::LoadAnimKeys( animfile );
		}
		catch ( fsInvalidLocatorX& i_Ex )
		{
			DBG_ERROR("Can't find the expression " << animfile);
			throw fsInvalidLocatorX(i_Ex);	// send it up.
			return;
		}

		entAnimation* animation = entImport::CreateAnimation(*anim_keys);

		const float start_time = 0.0f;
		const bool bPreserve = false;
		entSubAnimation *sub_anim = m_pObject->GetEntity()->AddSubAnimation(animation, start_time, bPreserve);
		m_pObject->GetEntity()->SetSubAnimationBlend(sub_anim, 0.0f);	// Start with 0.0 weight for expressions
	
		o_Expression.m_pAnimation = animation;
		o_Expression.m_pAnimInstance = sub_anim;
		o_Expression.m_pAnimKeys = anim_keys;
	}
	else
	{
		o_Expression.m_pAnimation = NULL;
		o_Expression.m_pAnimInstance = NULL;
		o_Expression.m_pAnimKeys = NULL;
	}
}

// TODO QUAD - create the quad expressionpropertyobject

//, const itString &i_ExpressionAnimFileNameLeft, const itString &i_ExpressionAnimFileNameRight
//	this->m_Data.m_FileNameLeft.SetValue( i_ExpressionAnimFileNameLeft );
//	this->m_Data.m_FileNameRight.SetValue( i_ExpressionAnimFileNameRight );
