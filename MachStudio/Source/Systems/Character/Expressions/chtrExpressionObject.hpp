/*****************************************************************************
**  chtrExpressionObject.hpp
**
**      A chtrExpressionObject is a supporting class for objects that
**	contain subanimation expressions.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_EXPRESSIONOBJECT_HPP
#error chtrExpressionObject.hpp multiply included
#endif
#define CHTR_EXPRESSIONOBJECT_HPP

#ifndef CHTR_EXPRESSIONPROPERTYOBJECT_HPP
#include "Systems/Character/Expressions/chtrExpressionPropertyObject.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif


//============================================================================
//============================================================================
class fsLocator;
class prtyFloat;
class relObject;

//============================================================================
//============================================================================
class chtrExpressionObject : public prtyObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chtrExpressionObject(api3dObjectEntity* i_pObject, const fsLocator& i_ObjectModelFile);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~chtrExpressionObject();

		//--------------------------------------------------------------------
		//	Set the object
		//--------------------------------------------------------------------
		void SetObject( api3dObjectEntity* i_pObject );
		void SetParent( relObject &i_Parent );

		//--------------------------------------------------------------------
		//	Create a slider for a single expression
		//--------------------------------------------------------------------
		void AddSingleProperty(const std::string &i_Name,
								const fsLocator &i_ExpressionAnimFileName);

		//--------------------------------------------------------------------
		//	Dual two expressions in order to control them with one slider
		//--------------------------------------------------------------------
		void AddDualProperty(const std::string &i_Name,
							 const fsLocator &i_ExpressionAnimFileNameLeft, 
							 const fsLocator &i_ExpressionAnimFileNameRight);

		//--------------------------------------------------------------------
		//	group Four expressions in order to control them with one slider
		//--------------------------------------------------------------------
		void AddQuadProperty(const std::string &i_Name,
							 const fsLocator &i_ExpressionAnimFileNameLeft, 
							 const fsLocator &i_ExpressionAnimFileNameRight, 
							 const fsLocator &i_ExpressionAnimFileNameUp, 
							 const fsLocator &i_ExpressionAnimFileNameDown);

		//--------------------------------------------------------------------
		//	Delete named expression
		//--------------------------------------------------------------------
		void DeleteExpression(const std::string& i_Name);

		//--------------------------------------------------------------------
		//	Delete expressions
		//--------------------------------------------------------------------
		void DeleteExpressions();

		//--------------------------------------------------------------------
		//	get expression
		//--------------------------------------------------------------------
		int GetExpression(const std::string& i_Name);

		//--------------------------------------------------------------------
		// Return number of expressions
		//--------------------------------------------------------------------
		int GetNumExpressions() const;
				
		//--------------------------------------------------------------------
		// Return name of expression
		//--------------------------------------------------------------------
		std::string GetExpressionName(int i_Index) const;
		void SetExpressionName(int i_Index, const std::string& i_Name);

		//--------------------------------------------------------------------
		// ExpressionWeight - usually number from 0-1 to set influence 
		//	of this morph target.
		//--------------------------------------------------------------------
		float GetExpressionWeight(int i_Index) const;
		void SetExpressionWeight(int i_Index, float i_Weight);

		//--------------------------------------------------------------------
		// Expression property access
		//--------------------------------------------------------------------
		prtyFloat& PropertyExpression(int i_Index);
		const prtyFloat& GetPropertyExpression(int i_Index) const;

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		chtrExpressionPropertyObject* GetExpressionUI(int i_Index) const;

public:
		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetData(const chtrExpressionsData &i_Data);
		void GetData(chtrExpressionsData &o_Data);

		//--------------------------------------------------------------------
		//	Set Directory - for getting animation list
		//--------------------------------------------------------------------
		void SetDirectory(const fsLocator &i_Directory);

		//	Channels
		//

		//--------------------------------------------------------------------
		// CreateChannel
		//--------------------------------------------------------------------
		tmlnChannelFloat* CreateChannel(const std::string &i_Name);

		//--------------------------------------------------------------------
		//	Gets
		//--------------------------------------------------------------------
		tmlnChannelFloat* GetExpressionChannel( const std::string& i_Name );
		bool GetNameFromExpressionChannel( tmlnChannelFloat* i_pChannel, std::string& o_Name );

	private:
		//--------------------------------------------------------------------
		//	fine the expression with the given name, otherwise return NULL
		//--------------------------------------------------------------------
		chtrExpressionPropertyObject* GetExpressionProperty( const std::string& i_Name );

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void SinglePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void SinglePropertyFileNameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void DualPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

	private:
		fsLocator m_ObjectModelPath;
		api3dObjectEntity* m_pObject;
		shared_ptr<relRelationship> m_ParentRelationship;

		// Name and weight for each expression is in property data
		std::vector<chtrExpressionPropertyObject*> m_PropertyObjects;
};
