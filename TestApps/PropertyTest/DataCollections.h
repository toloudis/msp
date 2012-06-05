#ifndef XXX_BASEDATA_HPP
#include "xxxBaseData.hpp"
#endif


//============================================================================
//============================================================================
using namespace System;						// attributes
using namespace System::Drawing;			// fonts
//using System::Collections;
using namespace System::ComponentModel;		// ReadOnlyAttribute
using namespace XLT::Windows::Forms;
using namespace TerawattManagedControls;


//============================================================================
//============================================================================
namespace PropertyTest
{
	public __gc class DataCollections
	{
//		public static XLT::Windows::Forms::PropertyBag bag2;
	public: static TerawattManagedControls::PropertyTable* bag3;
	public: static TerawattManagedControls::PropertyTable* bag4;

	public: static void Init()
		{
//			// Create the second property bag and add some properties.
//			bag2 = new PropertyBag();
//			bag2::GetValue += new PropertySpecEventHandler(this::bag2_GetValue);
//			bag2::SetValue += new PropertySpecEventHandler(this::bag2_SetValue);
//			bag2::Properties::Add(new PropertySpec("Fruit", typeof(Fruit), NULL, NULL, Fruit::Banana));
//			bag2::Properties::Add(new PropertySpec("Typeface", typeof(Font), "Another Category", NULL,
//				new Font("Tahoma", 8::25f)));
//			bag2::Properties::Add(new PropertySpec("Some Boolean", "System::Boolean", "Some Category", NULL,
//				false));

			//NOTE: need fully qualified name
			//
			//To make your program robust and being able to always get the Type back, you should be using the type's AssemblyFullyQualifiedName to get the type back. The easiest way to know a the AssemblyFullQualifiedName is to call t.AssemblyFullyQualifiedName.

			//For example, System.Drawing.Font  type’s AFQN is:
			//System.Drawing.Font, System.Drawing, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a

			//The rule behind it is:  t.FullName + “,” + asm.FullName = t.AFQN

			//Type::GetXLT.Windows.Forms.Fruit
			
			// This time, create a property table::  It uses a Hashtable to store
			// values, so we don't need to wire GetValue and SetValue events.
			bag3 = new PropertyTable();
			Type* newtype;
			newtype = __typeof(XLT::Windows::Forms::Fruit);
			bag3->Properties->Add(new PropertySpec(S"Fruit", newtype, NULL, NULL, __box(Fruit::Orange)));
			//newtype = Type::GetType(S"System.Drawing.Image, System.Drawing, Version=1.0.3300.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a");
			newtype = __typeof(System::Drawing::Image);
			bag3->Properties->Add(new PropertySpec(S"Picture", newtype, S"Some Category", S"This is a sample description."));
			//newtype = Type::GetType(S"System.Drawing.Font, System.Drawing, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a");
			newtype = __typeof(System::Drawing::Font);
			bag3->Properties->Add(new PropertySpec(S"Typeface", newtype, S"Another Category", NULL,
				new System::Drawing::Font("Tahoma", 8.25f)));
			//newtype = Type::GetType(S"System.Boolean");
			newtype = __typeof(System::Boolean);
			bag3->Properties->Add(new PropertySpec(S"Some Boolean", newtype, S"Some Category", NULL,
				false));
			newtype = Type::GetType(S"System.Int64");
			bag3->Properties->Add(new PropertySpec(S"Number", newtype, NULL, S"A big number:", __box(1234567890L)));

			// Create a property that uses additional attributes.
			//newtype = Type::GetType(S"System.String");
			newtype = __typeof(System::String);
			PropertySpec* ps = new PropertySpec(S"Can't Touch This", newtype, NULL,	S"This property is read-only.", S"Some Default String");
			//ps->Attributes = new Attribute;//[];// { ReadOnlyAttribute::Yes };
			bag3->Properties->Add(ps);

			// Assign values to the properties above.
			bag3->SetObjectValue(S"Fruit", __box(Fruit::Apple));
			bag3->SetObjectValue(S"Picture", NULL);
			bag3->SetObjectValue(S"Typeface", new Font(S"Times New Roman", 12.0f));
			bag3->SetObjectValue(S"Some Boolean", __box(true));
			bag3->SetObjectValue(S"Number", __box(1234567890L));
			bag3->SetObjectValue(S"Can't Touch This", S"Some Default String");

			// This time, create a property table->  It uses a Hashtable to store
			// values, so we don't need to wire GetValue and SetValue events.
			bag4 = new PropertyTable();
			newtype = __typeof(XLT::Windows::Forms::Fruit);
			bag4->Properties->Add(new PropertySpec(S"Fruit", newtype, NULL, NULL, __box(Fruit::Peach)));
			newtype = Type::GetType(S"System.Boolean");
			bag4->Properties->Add(new PropertySpec(S"Some Boolean", newtype, S"Category 1", NULL, false));
			newtype = Type::GetType(S"System.Drawing.Image, System.Drawing, Version=1.0.3300.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a");
			bag4->Properties->Add(new PropertySpec(S"Picture", newtype, S"Category 2", S"This is a sample description."));
			newtype = Type::GetType(S"System.Int64");
			bag4->Properties->Add(new PropertySpec(S"Number", newtype, NULL, S"A big number:", __box(1234567890L)));
			newtype = Type::GetType(S"System.Drawing.Font, System.Drawing, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a");
			bag4->Properties->Add(new PropertySpec(S"Typeface", newtype, S"Category 2", NULL,
				new System::Drawing::Font("Tahoma", 8.25f)));

			// Create a property that uses additional attributes.
			newtype = Type::GetType(S"System.String");
			ps = new PropertySpec(S"Touch This", newtype, NULL,	S"This property is not read-only.", S"Some Default String");
			//ps->Attributes = new Attribute;//[];// { ReadOnlyAttribute::Yes };
			bag4->Properties->Add(ps);

			// Assign values to the properties above.
			bag4->SetObjectValue(S"Fruit", __box(Fruit::Pear));
			bag4->SetObjectValue(S"Picture", NULL);
			bag4->SetObjectValue(S"Typeface", new Font(S"Arial", 12.0f));
			bag4->SetObjectValue(S"Some Boolean", __box(false));
			bag4->SetObjectValue(S"Number", __box(98765L));
			bag4->SetObjectValue(S"Can't Touch This", S"new default string");
		}
	};

}