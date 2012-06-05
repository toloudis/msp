/********************************************************************************************\
**  chtrExpressionPropertyObject.hpp
**
**	Selectable property object representing a sub animation expression
**
**  StudioGPU
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef CHTR_EXPRESSIONPROPERTYOBJECT_HPP
#error chtrExpressionPropertyObject.hpp multiply included
#endif
#define CHTR_EXPRESSIONPROPERTYOBJECT_HPP


#ifndef CMM_SELECTABLEPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmSelectablePropertyObject.hpp"
#endif 
#ifndef CHTR_EXPRESSIONDATA_HPP
#include "Systems/Character/Data/chtrExpressionData.hpp"
#endif
#ifndef TMLN_CHANNELFLOAT_HPP
#include "Support/tmln/tmlnChannelFloat.hpp"
#endif

#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class api3dObjectEntity;
class entAnimKeys;
class entAnimation;
class anFrameAnimInstance;


//============================================================================
//============================================================================
class chtrExpressionPropertyObject : public cmmSelectablePropertyObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrExpressionPropertyObject(api3dObjectEntity* i_pObject, const fsLocator& i_ModelPath);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~chtrExpressionPropertyObject();

	//------------------------------------------------------------------------
	// Quick access to name given in constructor
	//------------------------------------------------------------------------
	virtual inline const std::string& GetName() const = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void SetData(const chtrExpressionData &i_Data) = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual chtrExpressionData* CloneOriginalData() = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual inline prtyFloat& GetPropertyWeight() = 0;
	virtual inline prtyText& GetPropertyName() = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetChannel( tmlnChannelFloat * i_pChannel );
	tmlnChannelFloat * GetChannel();

	//------------------------------------------------------------------------
	//	Create a channel
	//------------------------------------------------------------------------
	virtual void CreateChannel();
	virtual void CreateChannel(const std::string& i_Name, prtyFloat& i_Weight);


//============================================================================
// sel3dObject - virtual function overrides
//============================================================================

	//--------------------------------------------------------------------
	// Set Parent pointer to use when creating selectable property objects
	//--------------------------------------------------------------------
	//virtual void SetParent(sel3dObject* i_pParent);

	//------------------------------------------------------------------------
	// Get parent object of this object in order to define relationships
	//	between icons and their affected objects.
	//------------------------------------------------------------------------
	//virtual sel3dObject* GetParentObject() const;

	//------------------------------------------------------------------------
	// Get the name of the object for display when selected
	//------------------------------------------------------------------------
	virtual std::string GetDisplayName() const;

	//--------------------------------------------------------------------
	//	AddExpression loads sub-animation as expression that can then
	//	be blended with an alpha from 0-1
	//--------------------------------------------------------------------
	void AddExpression(const std::string &i_Name,
					   const fsLocator& i_AnimFileName);

protected:
	// Keeps track of animation data for expressions
	struct ExpressionAnim 
	{
		ExpressionAnim(anFrameAnimInstance* i_pAnimInstance, 
					   entAnimation* i_pAnimation, 
					   entAnimKeys* i_pAnimKeys)
		: m_pAnimInstance(i_pAnimInstance), 
		  m_pAnimation(i_pAnimation), 
		  m_pAnimKeys(i_pAnimKeys) {}

		anFrameAnimInstance* m_pAnimInstance;
		entAnimation* m_pAnimation;
		entAnimKeys* m_pAnimKeys;
	};

protected:
	//------------------------------------------------------------------------
	//	ConfigureProperties - setup UIInfos and callbacks
	//------------------------------------------------------------------------
	virtual void ConfigureProperties() {};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void DeleteExpressionAnimation( ExpressionAnim& i_Expression );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void UpdateExpressionAnimation(	const fsLocator& i_AnimFileName, 
											ExpressionAnim& o_Expression );

protected:
	//sel3dObject* m_pParent;
	api3dObjectEntity* m_pObject;
	fsLocator m_ObjectModelPath;
	tmlnChannelFloat * m_pChannel;	// TODO - probably shouldn't have this here

	// Not the same length as m_Properties, some properties are groups
	// of expressions
	std::vector<ExpressionAnim> m_Expressions;
};


//
// TODO - move out single and multi objects
//


//============================================================================
//============================================================================
class chtrSingleExpressionPropertyObject : public chtrExpressionPropertyObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrSingleExpressionPropertyObject(api3dObjectEntity* i_pObject, const fsLocator& i_ModelPath, const fsLocator &i_ExpressionAnimFileName);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~chtrSingleExpressionPropertyObject();

	//------------------------------------------------------------------------
	// Access to properties
	//------------------------------------------------------------------------
	inline prtyFloat& GetPropertyWeight();
	inline prtyText& GetPropertyName();
	inline prtyFilePath& GetPropertyFileName();

	//------------------------------------------------------------------------
	// Quick access to name given in constructor
	//------------------------------------------------------------------------
	inline virtual const std::string& GetName() const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetData(const chtrExpressionData &i_Data);

	//------------------------------------------------------------------------
	//	Get access to the data
	//------------------------------------------------------------------------
	const chtrSingleExpressionData& GetData();
	chtrSingleExpressionData& Data();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual chtrExpressionData* CloneOriginalData();

	//------------------------------------------------------------------------
	//	Create a channel
	//------------------------------------------------------------------------
	virtual void CreateChannel();

private:
	//------------------------------------------------------------------------
	//	ConfigureProperties - setup UIInfos and callbacks
	//------------------------------------------------------------------------
	virtual void ConfigureProperties();

private:
	//------------------------------------------------------------------------
	// Property callbacks
	//------------------------------------------------------------------------
	void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void WeightChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void FileNameChanged(prtyProperty *i_pProperty, bool i_bDirty);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeleteExpressionAnimation( ExpressionAnim& i_Expression );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateExpressionAnimation(	const fsLocator& i_AnimFileName, 
									ExpressionAnim& o_Expression );

private:
	chtrSingleExpressionData m_Data;
};


//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline prtyFloat& chtrSingleExpressionPropertyObject::GetPropertyWeight() 
{ 
	return m_Data.m_Weight; 
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline prtyText& chtrSingleExpressionPropertyObject::GetPropertyName() 
{ 
	return m_Data.m_Name; 
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline prtyFilePath& chtrSingleExpressionPropertyObject::GetPropertyFileName() 
{ 
	return m_Data.m_FileName;
}

//------------------------------------------------------------------------
// Quick access to name given in constructor
//------------------------------------------------------------------------
inline const std::string& chtrSingleExpressionPropertyObject::GetName() const
{
	return m_Data.m_Name.GetValue();
}




//
// TODO - move out single and multi objects
//


//============================================================================
//============================================================================
class chtrDualExpressionPropertyObject : public chtrExpressionPropertyObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	chtrDualExpressionPropertyObject(api3dObjectEntity* i_pObject, 
									 const fsLocator& i_ModelPath, 
									 const fsLocator &i_ExpressionAnimFileNameLeft, 
									 const fsLocator &i_ExpressionAnimFileNameRight );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~chtrDualExpressionPropertyObject();

	//------------------------------------------------------------------------
	// Access to properties
	//------------------------------------------------------------------------
	inline prtyFloat& GetPropertyWeight();
	inline prtyText& GetPropertyName();
	inline prtyFilePath& GetPropertyFileNameRight();
	inline prtyFilePath& GetPropertyFileNameLeft();

	//------------------------------------------------------------------------
	// Quick access to name given in constructor
	//------------------------------------------------------------------------
	inline virtual const std::string& GetName() const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetData(const chtrExpressionData &i_Data);

	//------------------------------------------------------------------------
	//	Get access to the data
	//------------------------------------------------------------------------
	const chtrDualExpressionData& GetData();
	chtrDualExpressionData& Data();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual chtrExpressionData* CloneOriginalData();

	//------------------------------------------------------------------------
	//	Create a channel
	//------------------------------------------------------------------------
	virtual void CreateChannel();

private:
	//------------------------------------------------------------------------
	//	ConfigureProperties - setup UIInfos and callbacks
	//------------------------------------------------------------------------
	virtual void ConfigureProperties();

private:
	//------------------------------------------------------------------------
	// Property callbacks
	//------------------------------------------------------------------------
	void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void WeightChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void FileNameLeftChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void FileNameRightChanged(prtyProperty *i_pProperty, bool i_bDirty);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeleteExpressionAnimation( ExpressionAnim& i_Expression );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateExpressionAnimation(	const fsLocator& i_AnimFileName, 
									ExpressionAnim& o_Expression );

private:
	chtrDualExpressionData m_Data;
};


//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline prtyFloat& chtrDualExpressionPropertyObject::GetPropertyWeight() 
{ 
	return m_Data.m_Weight; 
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline prtyText& chtrDualExpressionPropertyObject::GetPropertyName() 
{ 
	return m_Data.m_Name; 
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline prtyFilePath& chtrDualExpressionPropertyObject::GetPropertyFileNameLeft() 
{ 
	return m_Data.m_FileNameLeft;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
inline prtyFilePath& chtrDualExpressionPropertyObject::GetPropertyFileNameRight() 
{ 
	return m_Data.m_FileNameRight;
}

//------------------------------------------------------------------------
// Quick access to name given in constructor
//------------------------------------------------------------------------
inline const std::string& chtrDualExpressionPropertyObject::GetName() const
{
	return m_Data.m_Name.GetValue();
}


// TODO QUAD - create the quad expressionpropertyobject

