/*****************************************************************************
**	chtrExpressionObject.cpp
**
**		A chtrExpressionObject is a supporting class for objects that
**	contain subanimation expressions.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Expressions/chtrExpressionObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/rel/relRelationshipMultiple.hpp"
#include "Graphics/ent/entAnimation.hpp"
#include "Graphics/ent/entAnimKeys.hpp"
#include "Graphics/ent/entEntity.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrExpressionObject::chtrExpressionObject(api3dObjectEntity* i_pObject, const fsLocator& i_ObjectModelFile)
:	m_pObject(i_pObject),
	m_ObjectModelPath(i_ObjectModelFile)
{
	//	remove the filename, so just the path is left.
	m_ObjectModelPath.Pop();

	//DBG_LOG("Expression Object has model path " << m_ObjectModelPath );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrExpressionObject::~chtrExpressionObject()
{
	DeleteExpressions();
}

//--------------------------------------------------------------------
//	Set the object
//--------------------------------------------------------------------
void chtrExpressionObject::SetObject( api3dObjectEntity* i_pObject )
{
	this->m_pObject = i_pObject;
}
void chtrExpressionObject::SetParent( relObject &i_Parent )
{
	// Create new multiple relationship connecting the property
	// objects to the new parent object
	//
	m_ParentRelationship.reset(
		new relRelationshipMultiple<chtrExpressionPropertyObject>("Expressions", i_Parent, m_PropertyObjects));
	i_Parent.AddRelationship(m_ParentRelationship);
	
	// update expressions if already "gathered".
	for (int i = 0; i < m_PropertyObjects.size(); i++)
	{
		m_PropertyObjects[i]->SetParentRelationship(m_ParentRelationship);
	}
}

//--------------------------------------------------------------------
//	Create a slider for a single expression
//--------------------------------------------------------------------
void chtrExpressionObject::AddSingleProperty(const std::string &i_Name,
											 const fsLocator &i_ExpressionAnimFileName)
{
	//std::vector<chtrExpressionPropertyObject*>::const_iterator it =  m_PropertyObjects.find(i_Name);
	//if (it != m_PropertyObjects.end())
	//{
	//	// already exists
	//}

	// Add object for this expression
	chtrSingleExpressionPropertyObject* pEPO = new chtrSingleExpressionPropertyObject(m_pObject, m_ObjectModelPath, i_ExpressionAnimFileName );
	pEPO->GetPropertyName().SetValue(i_Name);
	pEPO->GetPropertyWeight().SetValue(0.0f);
	//pEPO->GetPropertyWeight().SetMinimum(0.0f);
	//pEPO->GetPropertyWeight().SetMaximum(1.0f);
	m_PropertyObjects.push_back(pEPO);

	//
	pEPO->AddExpression( i_Name, i_ExpressionAnimFileName );

	// Register property with a slider
	prtyRangedFloatUIInfo* pRFUII  = new prtyRangedFloatUIInfo(&(pEPO->GetPropertyWeight()), "Expressions", i_Name);
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(40);
	AddProperty( pRFUII );

	// Register callback to update expression when property changes
	(pEPO->GetPropertyWeight()).AddCallback(new prtyCallbackWrapper<chtrExpressionObject>(this, &chtrExpressionObject::SinglePropertyChanged));
	(pEPO->GetPropertyName()).AddCallback(new prtyCallbackWrapper<chtrExpressionObject>(this, &chtrExpressionObject::SinglePropertyChanged));
	//(pEPO->GetPropertyFileName()).AddCallback(new prtyCallbackWrapper<chtrExpressionObject>(this, &chtrExpressionObject::SinglePropertyFileNameChanged));
}

//--------------------------------------------------------------------
//	Pair two expressions in order to control them with one slider
//--------------------------------------------------------------------
void chtrExpressionObject::AddDualProperty(const std::string &i_Name,
										   const fsLocator &i_ExpressionAnimFileNameLeft, 
										   const fsLocator &i_ExpressionAnimFileNameRight)
{
	//std::vector<chtrExpressionPropertyObject*>::const_iterator it =  m_PropertyObjects.find(i_Name);
	//if (it != m_PropertyObjects.end())
	//{
	//	// already exists
	//}

	// Add object for this expression
	chtrDualExpressionPropertyObject* pEPO = new chtrDualExpressionPropertyObject( m_pObject, m_ObjectModelPath, i_ExpressionAnimFileNameLeft, i_ExpressionAnimFileNameRight );
	pEPO->GetPropertyName().SetValue(i_Name);
	pEPO->GetPropertyWeight().SetValue(0.0f);
	//pEPO->GetPropertyWeight().SetMinimum(0.0f);
	//pEPO->GetPropertyWeight().SetMaximum(1.0f);
	m_PropertyObjects.push_back(pEPO);

	//
	pEPO->AddExpression( i_Name, i_ExpressionAnimFileNameLeft );
	pEPO->AddExpression( i_Name, i_ExpressionAnimFileNameRight );

	// Register property with a slider
	prtyRangedFloatUIInfo* pRFUII  = new prtyRangedFloatUIInfo(&(pEPO->GetPropertyWeight()), "Expressions", i_Name);
	pRFUII->SetMinimum(-1.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(40);
	AddProperty( pRFUII );

	// Register callback to update expression when property changes
	(pEPO->GetPropertyWeight()).AddCallback(new prtyCallbackWrapper<chtrExpressionObject>(this, &chtrExpressionObject::DualPropertyChanged));
	(pEPO->GetPropertyName()).AddCallback(new prtyCallbackWrapper<chtrExpressionObject>(this, &chtrExpressionObject::DualPropertyChanged));
	//(pEPO->GetPropertyFileNameRight()).AddCallback(new prtyCallbackWrapper<chtrExpressionObject>(this, &chtrExpressionObject::DualPropertyFileNameChanged));
	//(pEPO->GetPropertyFileNameLeft()).AddCallback(new prtyCallbackWrapper<chtrExpressionObject>(this, &chtrExpressionObject::DualPropertyFileNameChanged));
}

//--------------------------------------------------------------------
//	group four expressions in order to control them with one slider
//--------------------------------------------------------------------
void chtrExpressionObject::AddQuadProperty(const std::string &i_Name,
										   const fsLocator &i_ExpressionAnimFileNameLeft, 
										   const fsLocator &i_ExpressionAnimFileNameRight,
										   const fsLocator &i_ExpressionAnimFileNameUp,
										   const fsLocator &i_ExpressionAnimFileNameDown)
{
	// TODO QUAD - finish this function

	//std::vector<PairProperty>::const_iterator it =  m_PairProperties.find(i_Name);
	//if (it != m_PairProperties.end())
	//{
	//	// already exists
	//}

	//// Add object for this expression
	//chtrQuadExpressionPropertyObject* pEPO = new chtrQuadExpressionPropertyObject( m_pParent, m_pObject, m_ObjectModelPath, i_ExpressionAnimFileNameLeft, i_ExpressionAnimFileNameRight, i_ExpressionAnimFileNameUp, i_ExpressionAnimFileNameDown );
	//pEPO->GetPropertyName().SetValue(i_Name);
	//pEPO->GetPropertyWeight().SetValue(0.0f);
	//pEPO->GetPropertyWeight().SetMinimum(-1.0f);
	//pEPO->GetPropertyWeight().SetMaximum(1.0f);
	//m_PropertyObjects.push_back(pEPO);

	////
	//pEPO->AddExpression( i_Name, i_ExpressionAnimFileNameLeft );
	//pEPO->AddExpression( i_Name, i_ExpressionAnimFileNameRight );
	//pEPO->AddExpression( i_Name, i_ExpressionAnimFileNameUp );
	//pEPO->AddExpression( i_Name, i_ExpressionAnimFileNameDown );

	//// Register property with a slider
	//prtyRangedFloatUIInfo* pRFUII  = new prtyRangedFloatUIInfo(&(pEPO->GetPropertyWeight()), "Expressions", i_Name);
	//pRFUII->SetMinimum(-1.0f);
	//pRFUII->SetMaximum(1.0f);
	//pRFUII->SetDecimalPlaces(2);
	//pRFUII->SetNumTicks(40);
	//AddProperty( pRFUII );

//	// Register callback to update expression when property changes
////	m_PairProperties.push_back( PairProperty(i_ExpressionAnimFileNameLeft, i_ExpressionAnimFileNameRight, &(pEPO->GetPropertyWeight())) );
//	(pEPO->GetPropertyWeight()).AddCallback(new prtyCallbackWrapper<chtrExpressionObject>(this, &chtrExpressionObject::PairPropertyChanged));
//	(pEPO->GetPropertyName()).AddCallback(new prtyCallbackWrapper<chtrExpressionObject>(this, &chtrExpressionObject::PairPropertyChanged));
////	(pEPO->GetPropertyFileName()).AddCallback(new prtyCallbackWrapper<chtrExpressionObject>(this, &chtrExpressionObject::PairPropertyFileNameChanged));
}

//--------------------------------------------------------------------
//	Delete named expression
//--------------------------------------------------------------------
void chtrExpressionObject::DeleteExpression(const std::string& i_Name)
{
	const int num_properties = m_PropertyObjects.size();
	for (int i=0; i<num_properties; i++)
	{
		if (m_PropertyObjects[i]->GetPropertyName().GetValue() == i_Name)
		{
			std::vector<chtrExpressionPropertyObject*>::iterator it = m_PropertyObjects.begin() + i;
			delete *it;
			m_PropertyObjects.erase(it);
			return;
		}
	}
}

//--------------------------------------------------------------------
//	Delete expressions
//--------------------------------------------------------------------
void chtrExpressionObject::DeleteExpressions()
{
	envSTLHelpers::DeleteContainer(m_PropertyObjects);
}

//--------------------------------------------------------------------
//	get expression
//--------------------------------------------------------------------
int chtrExpressionObject::GetExpression(const std::string& i_Name)
{
	const int num_properties = m_PropertyObjects.size();
	for (int i=0; i<num_properties; i++)
	{
		if (m_PropertyObjects[i]->GetPropertyName().GetValue() == i_Name)
		{
			return i;
		}
	}
	return -1;
}

//--------------------------------------------------------------------
// Return number of named morph targets
//--------------------------------------------------------------------
int chtrExpressionObject::GetNumExpressions() const
{
	return m_PropertyObjects.size();
}

//--------------------------------------------------------------------
// Return name of morph target with given index
//--------------------------------------------------------------------
std::string chtrExpressionObject::GetExpressionName(int i_Index) const
{
	return m_PropertyObjects[i_Index]->GetPropertyName().GetValue();
}
void chtrExpressionObject::SetExpressionName(int i_Index, const std::string& i_Name)
{
	m_PropertyObjects[i_Index]->GetPropertyName().SetValue( i_Name );
}

//--------------------------------------------------------------------
// ExpressionWeight - usually number from 0-1 to set influence 
//	of this morph target.
//--------------------------------------------------------------------
float chtrExpressionObject::GetExpressionWeight(int i_Index) const
{
	return m_PropertyObjects[i_Index]->GetPropertyWeight().GetValue();
}
void chtrExpressionObject::SetExpressionWeight(int i_Index, float i_Weight)
{
	m_PropertyObjects[i_Index]->GetPropertyWeight().SetValue(i_Weight);
}

//--------------------------------------------------------------------
// Expression property access
//--------------------------------------------------------------------
prtyFloat& chtrExpressionObject::PropertyExpression(int i_Index)
{
	return (m_PropertyObjects[i_Index]->GetPropertyWeight());
}
const prtyFloat& chtrExpressionObject::GetPropertyExpression(int i_Index) const
{
	return (m_PropertyObjects[i_Index]->GetPropertyWeight());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
chtrExpressionPropertyObject* chtrExpressionObject::GetExpressionUI(int i_Index) const
{	
	DBG_ASSERT(i_Index<m_PropertyObjects.size(), "Control index out of property object range " << i_Index << " < " << m_PropertyObjects.size());
	return m_PropertyObjects[i_Index];
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void chtrExpressionObject::SetData(const chtrExpressionsData &i_Data)
{
	int nDataExpressions = i_Data.size();
	for (int e=0; e < nDataExpressions; e++)
	{
		chtrExpressionPropertyObject* pEPO = GetExpressionProperty( i_Data[e]->m_Name.GetValue() );
		if (pEPO != NULL)
		{
			pEPO->SetData( *(i_Data[e]) );
		}
		else
		{
			//	create the correct EPO based on the data
			//
			chtrSingleExpressionData* pSED = dynamic_cast<chtrSingleExpressionData*>(i_Data[e]);
			if (pSED != NULL)
			{
				AddSingleProperty( pSED->m_Name.GetValue(), pSED->m_FileName.GetValue() );
			}
			else
			{
				chtrDualExpressionData* pPED = dynamic_cast<chtrDualExpressionData*>(i_Data[e]);
				if (pPED != NULL)
				{
					this->AddDualProperty( pPED->m_Name.GetValue(), pPED->m_FileNameLeft.GetValue(), pPED->m_FileNameRight.GetValue() );
				}
				else
				{
					chtrQuadExpressionData* pQED = dynamic_cast<chtrQuadExpressionData*>(i_Data[e]);
					if (pQED != NULL)
					{
						this->AddQuadProperty( pQED->m_Name.GetValue(), pQED->m_FileNameLeft.GetValue(), pQED->m_FileNameRight.GetValue(), pQED->m_FileNameUp.GetValue(), pQED->m_FileNameDown.GetValue() );
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------
// Get to data structure
//--------------------------------------------------------------------
void chtrExpressionObject::GetData(chtrExpressionsData &o_Data)
{
	int nDataExpressions = o_Data.size();
	int nPOs = m_PropertyObjects.size();

	//	clear out the old
	for (int i=0; i < nDataExpressions; ++i)
	{
		delete o_Data[i];
	}
	o_Data.clear();

	//	push on the new
	for (int e=0; e < nPOs; e++)
	{
 		chtrExpressionPropertyObject* pEPO = m_PropertyObjects[e];
 		if (pEPO != NULL)
 		{
			o_Data.push_back( pEPO->CloneOriginalData() );
 			//o_Data.push_back( chtrExpressionData( pEPO->GetPropertyName().GetValue(), pEPO->GetPropertyWeight().GetValue() ) );
 		}
	}
}

//--------------------------------------------------------------------
// CreateChannel
//--------------------------------------------------------------------
tmlnChannelFloat* chtrExpressionObject::CreateChannel(const std::string &i_Name)
{
	int nExpressions = m_PropertyObjects.size();
	for (int e=0; e < nExpressions; e++)
	{
		chtrExpressionPropertyObject* pEPO = m_PropertyObjects[e];
		if (pEPO != NULL)
		{
			if (pEPO->GetPropertyName().GetValue() == i_Name)
			{
				pEPO->CreateChannel();
				return pEPO->GetChannel();
			}
		}
	}

	return NULL;
}

//--------------------------------------------------------------------
//	Gets
//--------------------------------------------------------------------
tmlnChannelFloat* chtrExpressionObject::GetExpressionChannel( const std::string& i_Name )
{
	int nExpressions = m_PropertyObjects.size();

	for (int e=0; e < nExpressions; e++)
	{
		chtrExpressionPropertyObject* pEPO = m_PropertyObjects[e];
		if (pEPO != NULL)
		{
			if (pEPO->GetChannel() != NULL)
			{
				if (pEPO->GetChannel()->GetName() == i_Name)
				{
					return pEPO->GetChannel();
				}
			}
		}
	}

	return NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool chtrExpressionObject::GetNameFromExpressionChannel( tmlnChannelFloat* i_pChannel, std::string& o_Name )
{
	int nExpressions = m_PropertyObjects.size();

	for (int e=0; e < nExpressions; e++)
	{
		chtrExpressionPropertyObject* pEPO = m_PropertyObjects[e];
		if (pEPO != NULL)
		{
			if (pEPO->GetChannel() == i_pChannel)
			{
				o_Name = pEPO->GetPropertyName().GetValue();
				return true;
			}
		}
	}

	return false;
}


//--------------------------------------------------------------------
//	fine the expression with the given name, otherwise return NULL
//--------------------------------------------------------------------
chtrExpressionPropertyObject* chtrExpressionObject::GetExpressionProperty( const std::string& i_Name )
{
	std::vector<chtrExpressionPropertyObject*>::iterator it, end;
	it = m_PropertyObjects.begin();
	end = m_PropertyObjects.end();

	while (it != end)
	{
		if ((*it)->GetPropertyName().GetValue() == i_Name)
		{
			return (*it);
		}

		++it;
	}
	return NULL;
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void chtrExpressionObject::SinglePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	//const int num_properties = m_SingleProperties.size();
	//for (int i=0; i<num_properties; i++)
	//{
	//	if (m_SingleProperties[i].m_pProperty == i_pProperty)
	//	{
	//		// alter blend for subanimation 
	//		if (m_SingleProperties[i].m_Index >= 0)
	//			m_pObject->GetEntity()->SetSubAnimationBlend(m_Expressions[m_SingleProperties[i].m_Index].m_pAnimInstance, 
	//														 m_SingleProperties[i].m_pProperty->GetValue());	
	//		break;
	//	}
	//}
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void chtrExpressionObject::SinglePropertyFileNameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	//const int num_properties = m_SingleProperties.size();
	//for (int i=0; i<num_properties; i++)
	//{
	//	int exp_index = m_SingleProperties[i].m_Index;
	//	if (&(m_PropertyObjects[exp_index]->GetPropertyFileName()) == i_pProperty)
	//	{
	//		DeleteExpressionAnimation( m_Expressions[exp_index] );

	//		prtyFileName* pPrtyFN = dynamic_cast<prtyFileName*>(i_pProperty);
	//		if (pPrtyFN != NULL)
	//		{
	//			try
	//			{
	//				UpdateExpressionAnimation( pPrtyFN->GetValue(), m_Expressions[exp_index]);
	//			}
	//			catch ( fsInvalidLocatorX& i_Ex )
	//			{
	//				return;
	//			}
	//		}
	//		break;
	//	}
	//}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrExpressionObject::DualPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	//const int num_properties = m_PairProperties.size();
	//for (int i=0; i<num_properties; i++)
	//{
	//	if (m_PairProperties[i].m_pProperty == i_pProperty)
	//	{
	//		float weight = m_PairProperties[i].m_pProperty->GetValue();
	//		int left = m_PairProperties[i].m_Left;
	//		int right = m_PairProperties[i].m_Right;
	//		entEntity *pEntity = m_pObject->GetEntity();

	//		if (weight < 0)
	//		{
	//			pEntity->SetSubAnimationBlend(m_Expressions[left].m_pAnimInstance, -weight);	
	//			pEntity->SetSubAnimationBlend(m_Expressions[right].m_pAnimInstance, 0);	
	//		}
	//		else
	//		{
	//			pEntity->SetSubAnimationBlend(m_Expressions[left].m_pAnimInstance, 0);	
	//			pEntity->SetSubAnimationBlend(m_Expressions[right].m_pAnimInstance, weight);	
	//		}

	//		break;
	//	}
	//}
}
