#pragma once

#ifndef PTCL_OPERATIONS_HPP
#include "ptclOperations.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif


//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


//============================================================================
//	forward references
//============================================================================
ref class tmaDialogMemory;
class prtParticleGeneratorTemplate;


//============================================================================
//
//============================================================================
namespace ParticleStudio
{
	/// <summary>
	/// Summary for ParticleDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ParticleDialog : public System::Windows::Forms::Form
	{
	public:
		static ParticleDialog ^FormInstance = nullptr;

		ParticleDialog();

		void UpdateData()
		{
		}

	protected:
		~ParticleDialog()
		{
			// clear instance
			if (ParticleDialog::FormInstance == this)
				ParticleDialog::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	private: tmaDialogMemory^ m_pMemory;	// Dialog memory remembers size, location, visiblity of dialog 
	private: bool m_bDisableNotify;			// Turns off notify callbacks when setting up form
	private: bool m_bOurChange;				// Turns off setting of data fields when we make the change

	private: System::Windows::Forms::TabControl ^  tabControl_particledata;

	private: System::Windows::Forms::TabPage ^  tabPage_particlestarttexture;
	private: System::Windows::Forms::TabPage ^  tabPage_particletype_generic;
	private: System::Windows::Forms::TabPage ^  tabPage_particletype_cone;
	private: System::Windows::Forms::TabPage ^  tabPage_particletype_spiral;
	private: System::Windows::Forms::TabPage ^  tabPage_generator;

	//	main menu
	private: System::Windows::Forms::Button ^  button_save;
	private: System::Windows::Forms::Button ^  button_saveas;
	private: System::Windows::Forms::Button ^  button_open;

	//	tabPage - Particle Start Texture
	private: System::Windows::Forms::GroupBox ^  groupBox_texture_scaling;
	private: System::Windows::Forms::RadioButton ^  radioButton_texture_scaling_linear;
	private: System::Windows::Forms::RadioButton ^  radioButton_texture_scaling_exponential;
	private: System::Windows::Forms::GroupBox ^  groupBox_texture;
	private: TerawattManagedControls::FileChooser ^  fileChooser_texture;
	private: System::Windows::Forms::GroupBox ^  groupBox_texture_alphablending;
	private: System::Windows::Forms::RadioButton ^  radioButton_texture_alphablending_add;
	private: System::Windows::Forms::RadioButton ^  radioButton_texture_alphablending_multiply;
	private: System::Windows::Forms::GroupBox ^  groupBox_texture_UVA;
	private: System::Windows::Forms::RadioButton ^  radioButton_texture_UVA_random;
	private: System::Windows::Forms::RadioButton ^  radioButton_texture_UVA_Lifetime;
	private: System::Windows::Forms::GroupBox ^  groupBox_texture_alphalevels;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_texture_alphalevel_start;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_texture_alphalevel_middle;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_texture_alphalevel_end;
	private: System::Windows::Forms::Label ^  label_texture_alphalevel_start;
	private: System::Windows::Forms::Label ^  label_texture_alphalevel_middle;
	private: System::Windows::Forms::Label ^  label_texture_alphalevel_end;
	private: System::Windows::Forms::Label ^  label_texture_alphalevel_middlepercentstart;
	private: System::Windows::Forms::Label ^  label_texture_alphalevel_middlepercentend;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_texture_alphalevel_middlepercentend;
	private: TerawattManagedControls::RangedFloat ^  rangedFloat_texture_alphalevel_middlepercentstart;


	//	tabPage - Particle Generic
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_particlescale_start;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particlescale_start;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particlescalecoefficient;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particlescale;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_particlescalecoefficient;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_particlestartangle_min;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particlestartangle_max;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particlestartangle_min;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_particlestartangle_max;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particlestartangle;

	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_particleangularvelocity_min;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particleangularvelocity_max;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particleangularvelocity_min;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_particleangularvelocity_max;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particleangularvelocity;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_particleangularacceleration_min;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particleangularacceleration_max;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particleangularacceleration_min;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_particleangularacceleration_max;
	private: System::Windows::Forms::Label ^  label_particledata_generic_particleangularacceleration;
	private: System::Windows::Forms::GroupBox ^  groupBox_particledata_generic_particle;

	//	tabPage - Particle Cone
	private: System::Windows::Forms::GroupBox ^  groupBox_particledata_cone;
	private: System::Windows::Forms::Label ^  label_particledata_cone_acceleration;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_cone_speed_min;
	private: System::Windows::Forms::Label ^  label_particledata_cone_speed_max;
	private: System::Windows::Forms::Label ^  label_particledata_cone_speed_min;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_cone_speed_max;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_cone_coneangle;
	private: System::Windows::Forms::Label ^  label_particledata_cone_coneangle;
	private: System::Windows::Forms::Label ^  label_particledata_cone_speed;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_particledata_cone_acceleration;

	//	tabPage - Particle Spiral
	private: System::Windows::Forms::GroupBox ^  groupBox_particledata_spiral;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_particledata_spiral_emitdirection;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_emitdirection;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_spiral_speed_min;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_speed_max;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_speed_min;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_spiral_speed_max;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_spiral_rotationradius;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_rotationradius;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_speed;
	private: TerawattManagedControls::Vector3Edit ^  vector3Edit_particledata_spiral_acceleration;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_acceleration;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_spiral_rotationangle_min;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_rotationangle_max;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_rotationangle_min;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_spiral_rotationangle_max;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_rotationangle;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_spiral_rotationangular_min;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_rotationangular_max;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_rotationangular_min;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_spiral_rotationangular_max;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_rotationangular;
	private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_spiral_radiusscalerate;
	private: System::Windows::Forms::Label ^  label_particledata_spiral_radiusscalerate;
private: System::Windows::Forms::GroupBox ^  groupBox_emitter;
private: TerawattManagedControls::FloatEdit ^  floatEdit_emitterscale;
private: System::Windows::Forms::Label ^  label_emitterscale;
private: System::Windows::Forms::GroupBox ^  groupBox_geometry;
private: TerawattManagedControls::FileChooser ^  fileChooser_geometry;
private: System::Windows::Forms::GroupBox ^  groupBox_particledata_generic_particles;
private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_maxparticles;
private: System::Windows::Forms::Label ^  label_particledata_generic_maxparticles;
private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_particlelifetime_min;
private: System::Windows::Forms::Label ^  label_particledata_generic_particlelifetime_max;
private: System::Windows::Forms::Label ^  label_particledata_generic_particlelifetime_min;
private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_particlelifetime_max;
private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_particlerate;
private: System::Windows::Forms::Label ^  label_particledata_generic_particlerate;
private: System::Windows::Forms::Label ^  label_particledata_generic_particlelifetime;
private: System::Windows::Forms::GroupBox ^  groupBox_simulation;
private: System::Windows::Forms::Label ^  label_seconds;
private: TerawattManagedControls::FloatEdit ^  floatEdit_particledata_generic_presimtime;
private: System::Windows::Forms::Label ^  label_presimtime;
private: System::Windows::Forms::GroupBox ^  groupBox_assettype;
private: System::Windows::Forms::RadioButton ^  radioButton_assettype_geometry;
private: System::Windows::Forms::RadioButton ^  radioButton_assettype_1texture;
private: System::Windows::Forms::CheckedListBox ^  checkedListBox_emittertype;
private: System::Windows::Forms::CheckedListBox ^  checkedListBox_generatortype;

private: System::Windows::Forms::GroupBox ^  groupBox_texture_info;
private: TerawattManagedControls::FloatEdit ^  floatEdit_texture_starttime;
private: System::Windows::Forms::Label ^  label_texture_starttime;
private: System::Windows::Forms::TextBox ^  textBox_texture_name;
private: TerawattManagedControls::FloatEdit ^  floatEdit_texture_endtime;
private: System::Windows::Forms::Label ^  label_texture_endtime;
private: System::Windows::Forms::Label ^  label_texture_name;
private: System::Windows::Forms::Button ^  button_texture_movedown;
private: System::Windows::Forms::Button ^  button_texture_moveup;
private: System::Windows::Forms::Button ^  button_texture_del;
private: System::Windows::Forms::Button ^  button_texture_add;
private: System::Windows::Forms::NumericUpDown ^  numericUpDown_texture_framerate;
private: System::Windows::Forms::Label ^  label_texture_framerate;
private: System::Windows::Forms::CheckBox ^  checkBox_texture_looping;
private: System::Windows::Forms::NumericUpDown ^  numericUpDown_texture_frames;
private: System::Windows::Forms::Label ^  label_texture_frames;
private: System::Windows::Forms::ListBox ^  listBox_textures;
private: System::Windows::Forms::Label ^  label_texture_timeinfo;
private: System::Windows::Forms::TabPage^  tabPage_streaking;
private: System::Windows::Forms::GroupBox^  groupBox_streaking;
private: System::Windows::Forms::CheckBox^  checkBox_streaking;
private: TerawattManagedControls::RangedFloat^  rangedFloat_streak_fade;
private: System::Windows::Forms::Label^  label3;
private: TerawattManagedControls::RangedFloat^  rangedFloat_streak_taper;
private: System::Windows::Forms::Label^  label2;
private: TerawattManagedControls::RangedFloat^  rangedFloat_streak_length;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::RadioButton^  radioButton_texture_UVA_TUV;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl_particledata = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_generator = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_simulation = (gcnew System::Windows::Forms::GroupBox());
			this->label_seconds = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_generic_presimtime = (gcnew TerawattManagedControls::FloatEdit());
			this->label_presimtime = (gcnew System::Windows::Forms::Label());
			this->groupBox_particledata_generic_particles = (gcnew System::Windows::Forms::GroupBox());
			this->checkedListBox_generatortype = (gcnew System::Windows::Forms::CheckedListBox());
			this->floatEdit_particledata_generic_maxparticles = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_generic_maxparticles = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_generic_particlelifetime_min = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_generic_particlelifetime_max = (gcnew System::Windows::Forms::Label());
			this->label_particledata_generic_particlelifetime_min = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_generic_particlelifetime_max = (gcnew TerawattManagedControls::FloatEdit());
			this->floatEdit_particledata_generic_particlerate = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_generic_particlerate = (gcnew System::Windows::Forms::Label());
			this->label_particledata_generic_particlelifetime = (gcnew System::Windows::Forms::Label());
			this->groupBox_emitter = (gcnew System::Windows::Forms::GroupBox());
			this->checkedListBox_emittertype = (gcnew System::Windows::Forms::CheckedListBox());
			this->label_emitterscale = (gcnew System::Windows::Forms::Label());
			this->floatEdit_emitterscale = (gcnew TerawattManagedControls::FloatEdit());
			this->tabPage_particlestarttexture = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_texture_info = (gcnew System::Windows::Forms::GroupBox());
			this->label_texture_timeinfo = (gcnew System::Windows::Forms::Label());
			this->floatEdit_texture_starttime = (gcnew TerawattManagedControls::FloatEdit());
			this->label_texture_starttime = (gcnew System::Windows::Forms::Label());
			this->textBox_texture_name = (gcnew System::Windows::Forms::TextBox());
			this->floatEdit_texture_endtime = (gcnew TerawattManagedControls::FloatEdit());
			this->label_texture_endtime = (gcnew System::Windows::Forms::Label());
			this->label_texture_name = (gcnew System::Windows::Forms::Label());
			this->button_texture_movedown = (gcnew System::Windows::Forms::Button());
			this->button_texture_moveup = (gcnew System::Windows::Forms::Button());
			this->button_texture_del = (gcnew System::Windows::Forms::Button());
			this->button_texture_add = (gcnew System::Windows::Forms::Button());
			this->listBox_textures = (gcnew System::Windows::Forms::ListBox());
			this->groupBox_texture_alphalevels = (gcnew System::Windows::Forms::GroupBox());
			this->rangedFloat_texture_alphalevel_middlepercentend = (gcnew TerawattManagedControls::RangedFloat());
			this->label_texture_alphalevel_middlepercentend = (gcnew System::Windows::Forms::Label());
			this->rangedFloat_texture_alphalevel_end = (gcnew TerawattManagedControls::RangedFloat());
			this->rangedFloat_texture_alphalevel_middlepercentstart = (gcnew TerawattManagedControls::RangedFloat());
			this->rangedFloat_texture_alphalevel_middle = (gcnew TerawattManagedControls::RangedFloat());
			this->rangedFloat_texture_alphalevel_start = (gcnew TerawattManagedControls::RangedFloat());
			this->label_texture_alphalevel_middlepercentstart = (gcnew System::Windows::Forms::Label());
			this->label_texture_alphalevel_end = (gcnew System::Windows::Forms::Label());
			this->label_texture_alphalevel_middle = (gcnew System::Windows::Forms::Label());
			this->label_texture_alphalevel_start = (gcnew System::Windows::Forms::Label());
			this->groupBox_texture_scaling = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton_texture_scaling_exponential = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_texture_scaling_linear = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox_texture = (gcnew System::Windows::Forms::GroupBox());
			this->numericUpDown_texture_framerate = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_texture_framerate = (gcnew System::Windows::Forms::Label());
			this->checkBox_texture_looping = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDown_texture_frames = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_texture_frames = (gcnew System::Windows::Forms::Label());
			this->fileChooser_texture = (gcnew TerawattManagedControls::FileChooser());
			this->groupBox_texture_alphablending = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton_texture_alphablending_multiply = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_texture_alphablending_add = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox_texture_UVA = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton_texture_UVA_random = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_texture_UVA_Lifetime = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_texture_UVA_TUV = (gcnew System::Windows::Forms::RadioButton());
			this->tabPage_streaking = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_streaking = (gcnew System::Windows::Forms::GroupBox());
			this->rangedFloat_streak_fade = (gcnew TerawattManagedControls::RangedFloat());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->rangedFloat_streak_taper = (gcnew TerawattManagedControls::RangedFloat());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->rangedFloat_streak_length = (gcnew TerawattManagedControls::RangedFloat());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->checkBox_streaking = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage_particletype_generic = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_assettype = (gcnew System::Windows::Forms::GroupBox());
			this->radioButton_assettype_geometry = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton_assettype_1texture = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox_geometry = (gcnew System::Windows::Forms::GroupBox());
			this->fileChooser_geometry = (gcnew TerawattManagedControls::FileChooser());
			this->groupBox_particledata_generic_particle = (gcnew System::Windows::Forms::GroupBox());
			this->floatEdit_particledata_generic_particleangularacceleration_min = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_generic_particleangularacceleration_max = (gcnew System::Windows::Forms::Label());
			this->label_particledata_generic_particleangularacceleration_min = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_generic_particleangularacceleration_max = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_generic_particleangularacceleration = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_generic_particleangularvelocity_min = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_generic_particleangularvelocity_max = (gcnew System::Windows::Forms::Label());
			this->label_particledata_generic_particleangularvelocity_min = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_generic_particleangularvelocity_max = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_generic_particleangularvelocity = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_generic_particlestartangle_min = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_generic_particlestartangle_max = (gcnew System::Windows::Forms::Label());
			this->label_particledata_generic_particlestartangle_min = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_generic_particlestartangle_max = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_generic_particlestartangle = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_generic_particlescale_start = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_generic_particlescale_start = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_generic_particlescalecoefficient = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_generic_particlescalecoefficient = (gcnew System::Windows::Forms::Label());
			this->label_particledata_generic_particlescale = (gcnew System::Windows::Forms::Label());
			this->tabPage_particletype_cone = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_particledata_cone = (gcnew System::Windows::Forms::GroupBox());
			this->vector3Edit_particledata_cone_acceleration = (gcnew TerawattManagedControls::Vector3Edit());
			this->label_particledata_cone_acceleration = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_cone_speed_min = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_cone_speed_max = (gcnew System::Windows::Forms::Label());
			this->label_particledata_cone_speed_min = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_cone_speed_max = (gcnew TerawattManagedControls::FloatEdit());
			this->floatEdit_particledata_cone_coneangle = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_cone_coneangle = (gcnew System::Windows::Forms::Label());
			this->label_particledata_cone_speed = (gcnew System::Windows::Forms::Label());
			this->tabPage_particletype_spiral = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_particledata_spiral = (gcnew System::Windows::Forms::GroupBox());
			this->floatEdit_particledata_spiral_radiusscalerate = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_spiral_radiusscalerate = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_spiral_rotationangular_min = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_spiral_rotationangular_max = (gcnew System::Windows::Forms::Label());
			this->label_particledata_spiral_rotationangular_min = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_spiral_rotationangular_max = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_spiral_rotationangular = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_spiral_rotationangle_min = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_spiral_rotationangle_max = (gcnew System::Windows::Forms::Label());
			this->label_particledata_spiral_rotationangle_min = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_spiral_rotationangle_max = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_spiral_rotationangle = (gcnew System::Windows::Forms::Label());
			this->vector3Edit_particledata_spiral_acceleration = (gcnew TerawattManagedControls::Vector3Edit());
			this->label_particledata_spiral_acceleration = (gcnew System::Windows::Forms::Label());
			this->vector3Edit_particledata_spiral_emitdirection = (gcnew TerawattManagedControls::Vector3Edit());
			this->label_particledata_spiral_emitdirection = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_spiral_speed_min = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_spiral_speed_max = (gcnew System::Windows::Forms::Label());
			this->label_particledata_spiral_speed_min = (gcnew System::Windows::Forms::Label());
			this->floatEdit_particledata_spiral_speed_max = (gcnew TerawattManagedControls::FloatEdit());
			this->floatEdit_particledata_spiral_rotationradius = (gcnew TerawattManagedControls::FloatEdit());
			this->label_particledata_spiral_rotationradius = (gcnew System::Windows::Forms::Label());
			this->label_particledata_spiral_speed = (gcnew System::Windows::Forms::Label());
			this->button_save = (gcnew System::Windows::Forms::Button());
			this->button_saveas = (gcnew System::Windows::Forms::Button());
			this->button_open = (gcnew System::Windows::Forms::Button());
			this->tabControl_particledata->SuspendLayout();
			this->tabPage_generator->SuspendLayout();
			this->groupBox_simulation->SuspendLayout();
			this->groupBox_particledata_generic_particles->SuspendLayout();
			this->groupBox_emitter->SuspendLayout();
			this->tabPage_particlestarttexture->SuspendLayout();
			this->groupBox_texture_info->SuspendLayout();
			this->groupBox_texture_alphalevels->SuspendLayout();
			this->groupBox_texture_scaling->SuspendLayout();
			this->groupBox_texture->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_texture_framerate))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_texture_frames))->BeginInit();
			this->groupBox_texture_alphablending->SuspendLayout();
			this->groupBox_texture_UVA->SuspendLayout();
			this->tabPage_streaking->SuspendLayout();
			this->groupBox_streaking->SuspendLayout();
			this->tabPage_particletype_generic->SuspendLayout();
			this->groupBox_assettype->SuspendLayout();
			this->groupBox_geometry->SuspendLayout();
			this->groupBox_particledata_generic_particle->SuspendLayout();
			this->tabPage_particletype_cone->SuspendLayout();
			this->groupBox_particledata_cone->SuspendLayout();
			this->tabPage_particletype_spiral->SuspendLayout();
			this->groupBox_particledata_spiral->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_particledata
			// 
			this->tabControl_particledata->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl_particledata->Controls->Add(this->tabPage_generator);
			this->tabControl_particledata->Controls->Add(this->tabPage_particlestarttexture);
			this->tabControl_particledata->Controls->Add(this->tabPage_streaking);
			this->tabControl_particledata->Controls->Add(this->tabPage_particletype_generic);
			this->tabControl_particledata->Controls->Add(this->tabPage_particletype_cone);
			this->tabControl_particledata->Controls->Add(this->tabPage_particletype_spiral);
			this->tabControl_particledata->Location = System::Drawing::Point(8, 8);
			this->tabControl_particledata->Multiline = true;
			this->tabControl_particledata->Name = L"tabControl_particledata";
			this->tabControl_particledata->SelectedIndex = 0;
			this->tabControl_particledata->Size = System::Drawing::Size(458, 646);
			this->tabControl_particledata->TabIndex = 0;
			// 
			// tabPage_generator
			// 
			this->tabPage_generator->Controls->Add(this->groupBox_simulation);
			this->tabPage_generator->Controls->Add(this->groupBox_particledata_generic_particles);
			this->tabPage_generator->Controls->Add(this->groupBox_emitter);
			this->tabPage_generator->Location = System::Drawing::Point(4, 22);
			this->tabPage_generator->Name = L"tabPage_generator";
			this->tabPage_generator->Size = System::Drawing::Size(450, 620);
			this->tabPage_generator->TabIndex = 0;
			this->tabPage_generator->Text = L"Generator / Emitter";
			this->tabPage_generator->UseVisualStyleBackColor = true;
			// 
			// groupBox_simulation
			// 
			this->groupBox_simulation->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_simulation->Controls->Add(this->label_seconds);
			this->groupBox_simulation->Controls->Add(this->floatEdit_particledata_generic_presimtime);
			this->groupBox_simulation->Controls->Add(this->label_presimtime);
			this->groupBox_simulation->Location = System::Drawing::Point(8, 296);
			this->groupBox_simulation->Name = L"groupBox_simulation";
			this->groupBox_simulation->Size = System::Drawing::Size(426, 64);
			this->groupBox_simulation->TabIndex = 12;
			this->groupBox_simulation->TabStop = false;
			this->groupBox_simulation->Text = L"Simulation";
			// 
			// label_seconds
			// 
			this->label_seconds->Location = System::Drawing::Point(200, 28);
			this->label_seconds->Name = L"label_seconds";
			this->label_seconds->Size = System::Drawing::Size(48, 16);
			this->label_seconds->TabIndex = 7;
			this->label_seconds->Text = L"seconds";
			this->label_seconds->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_generic_presimtime
			// 
			this->floatEdit_particledata_generic_presimtime->Location = System::Drawing::Point(112, 24);
			this->floatEdit_particledata_generic_presimtime->Name = L"floatEdit_particledata_generic_presimtime";
			this->floatEdit_particledata_generic_presimtime->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_presimtime->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_presimtime->TabIndex = 6;
			this->floatEdit_particledata_generic_presimtime->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_presimtime_ValueChanged);
			// 
			// label_presimtime
			// 
			this->label_presimtime->Location = System::Drawing::Point(16, 29);
			this->label_presimtime->Name = L"label_presimtime";
			this->label_presimtime->Size = System::Drawing::Size(88, 14);
			this->label_presimtime->TabIndex = 5;
			this->label_presimtime->Text = L"Pre-Sim Time";
			this->label_presimtime->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox_particledata_generic_particles
			// 
			this->groupBox_particledata_generic_particles->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_particledata_generic_particles->Controls->Add(this->checkedListBox_generatortype);
			this->groupBox_particledata_generic_particles->Controls->Add(this->floatEdit_particledata_generic_maxparticles);
			this->groupBox_particledata_generic_particles->Controls->Add(this->label_particledata_generic_maxparticles);
			this->groupBox_particledata_generic_particles->Controls->Add(this->floatEdit_particledata_generic_particlelifetime_min);
			this->groupBox_particledata_generic_particles->Controls->Add(this->label_particledata_generic_particlelifetime_max);
			this->groupBox_particledata_generic_particles->Controls->Add(this->label_particledata_generic_particlelifetime_min);
			this->groupBox_particledata_generic_particles->Controls->Add(this->floatEdit_particledata_generic_particlelifetime_max);
			this->groupBox_particledata_generic_particles->Controls->Add(this->floatEdit_particledata_generic_particlerate);
			this->groupBox_particledata_generic_particles->Controls->Add(this->label_particledata_generic_particlerate);
			this->groupBox_particledata_generic_particles->Controls->Add(this->label_particledata_generic_particlelifetime);
			this->groupBox_particledata_generic_particles->Location = System::Drawing::Point(8, 8);
			this->groupBox_particledata_generic_particles->Name = L"groupBox_particledata_generic_particles";
			this->groupBox_particledata_generic_particles->Size = System::Drawing::Size(426, 136);
			this->groupBox_particledata_generic_particles->TabIndex = 4;
			this->groupBox_particledata_generic_particles->TabStop = false;
			this->groupBox_particledata_generic_particles->Text = L"Particle Generator";
			// 
			// checkedListBox_generatortype
			// 
			this->checkedListBox_generatortype->CheckOnClick = true;
			this->checkedListBox_generatortype->Location = System::Drawing::Point(8, 16);
			this->checkedListBox_generatortype->Name = L"checkedListBox_generatortype";
			this->checkedListBox_generatortype->Size = System::Drawing::Size(144, 109);
			this->checkedListBox_generatortype->TabIndex = 10;
			this->checkedListBox_generatortype->ThreeDCheckBoxes = true;
			this->checkedListBox_generatortype->SelectedIndexChanged += gcnew System::EventHandler(this, &ParticleDialog::checkedListBox_generatortype_SelectedIndexChanged);
			// 
			// floatEdit_particledata_generic_maxparticles
			// 
			this->floatEdit_particledata_generic_maxparticles->Location = System::Drawing::Point(264, 48);
			this->floatEdit_particledata_generic_maxparticles->Name = L"floatEdit_particledata_generic_maxparticles";
			this->floatEdit_particledata_generic_maxparticles->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_maxparticles->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_maxparticles->TabIndex = 3;
			this->floatEdit_particledata_generic_maxparticles->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_maxparticles_ValueChanged);
			// 
			// label_particledata_generic_maxparticles
			// 
			this->label_particledata_generic_maxparticles->Location = System::Drawing::Point(168, 52);
			this->label_particledata_generic_maxparticles->Name = L"label_particledata_generic_maxparticles";
			this->label_particledata_generic_maxparticles->Size = System::Drawing::Size(88, 16);
			this->label_particledata_generic_maxparticles->TabIndex = 9;
			this->label_particledata_generic_maxparticles->Text = L"Max Particles";
			this->label_particledata_generic_maxparticles->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_generic_particlelifetime_min
			// 
			this->floatEdit_particledata_generic_particlelifetime_min->Location = System::Drawing::Point(264, 72);
			this->floatEdit_particledata_generic_particlelifetime_min->Name = L"floatEdit_particledata_generic_particlelifetime_min";
			this->floatEdit_particledata_generic_particlelifetime_min->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_particlelifetime_min->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_particlelifetime_min->TabIndex = 0;
			this->floatEdit_particledata_generic_particlelifetime_min->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_particlelifetime_min_ValueChanged);
			// 
			// label_particledata_generic_particlelifetime_max
			// 
			this->label_particledata_generic_particlelifetime_max->Location = System::Drawing::Point(352, 100);
			this->label_particledata_generic_particlelifetime_max->Name = L"label_particledata_generic_particlelifetime_max";
			this->label_particledata_generic_particlelifetime_max->Size = System::Drawing::Size(40, 16);
			this->label_particledata_generic_particlelifetime_max->TabIndex = 6;
			this->label_particledata_generic_particlelifetime_max->Text = L"max";
			this->label_particledata_generic_particlelifetime_max->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_generic_particlelifetime_min
			// 
			this->label_particledata_generic_particlelifetime_min->Location = System::Drawing::Point(352, 76);
			this->label_particledata_generic_particlelifetime_min->Name = L"label_particledata_generic_particlelifetime_min";
			this->label_particledata_generic_particlelifetime_min->Size = System::Drawing::Size(40, 16);
			this->label_particledata_generic_particlelifetime_min->TabIndex = 5;
			this->label_particledata_generic_particlelifetime_min->Text = L"min";
			this->label_particledata_generic_particlelifetime_min->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_generic_particlelifetime_max
			// 
			this->floatEdit_particledata_generic_particlelifetime_max->Location = System::Drawing::Point(264, 96);
			this->floatEdit_particledata_generic_particlelifetime_max->Name = L"floatEdit_particledata_generic_particlelifetime_max";
			this->floatEdit_particledata_generic_particlelifetime_max->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_particlelifetime_max->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_particlelifetime_max->TabIndex = 1;
			this->floatEdit_particledata_generic_particlelifetime_max->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_particlelifetime_max_ValueChanged);
			// 
			// floatEdit_particledata_generic_particlerate
			// 
			this->floatEdit_particledata_generic_particlerate->Location = System::Drawing::Point(264, 24);
			this->floatEdit_particledata_generic_particlerate->Name = L"floatEdit_particledata_generic_particlerate";
			this->floatEdit_particledata_generic_particlerate->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_particlerate->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_particlerate->TabIndex = 2;
			this->floatEdit_particledata_generic_particlerate->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_particlerate_ValueChanged);
			// 
			// label_particledata_generic_particlerate
			// 
			this->label_particledata_generic_particlerate->Location = System::Drawing::Point(168, 29);
			this->label_particledata_generic_particlerate->Name = L"label_particledata_generic_particlerate";
			this->label_particledata_generic_particlerate->Size = System::Drawing::Size(88, 14);
			this->label_particledata_generic_particlerate->TabIndex = 3;
			this->label_particledata_generic_particlerate->Text = L"Particle Rate";
			this->label_particledata_generic_particlerate->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_generic_particlelifetime
			// 
			this->label_particledata_generic_particlelifetime->Location = System::Drawing::Point(168, 77);
			this->label_particledata_generic_particlelifetime->Name = L"label_particledata_generic_particlelifetime";
			this->label_particledata_generic_particlelifetime->Size = System::Drawing::Size(88, 14);
			this->label_particledata_generic_particlelifetime->TabIndex = 1;
			this->label_particledata_generic_particlelifetime->Text = L"Particle Lifetime";
			this->label_particledata_generic_particlelifetime->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox_emitter
			// 
			this->groupBox_emitter->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_emitter->Controls->Add(this->checkedListBox_emittertype);
			this->groupBox_emitter->Controls->Add(this->label_emitterscale);
			this->groupBox_emitter->Controls->Add(this->floatEdit_emitterscale);
			this->groupBox_emitter->Location = System::Drawing::Point(8, 152);
			this->groupBox_emitter->Name = L"groupBox_emitter";
			this->groupBox_emitter->Size = System::Drawing::Size(426, 136);
			this->groupBox_emitter->TabIndex = 2;
			this->groupBox_emitter->TabStop = false;
			this->groupBox_emitter->Text = L"Particle Emitter";
			// 
			// checkedListBox_emittertype
			// 
			this->checkedListBox_emittertype->CheckOnClick = true;
			this->checkedListBox_emittertype->Location = System::Drawing::Point(8, 16);
			this->checkedListBox_emittertype->Name = L"checkedListBox_emittertype";
			this->checkedListBox_emittertype->Size = System::Drawing::Size(144, 109);
			this->checkedListBox_emittertype->TabIndex = 7;
			this->checkedListBox_emittertype->ThreeDCheckBoxes = true;
			this->checkedListBox_emittertype->SelectedIndexChanged += gcnew System::EventHandler(this, &ParticleDialog::checkedListBox_emittertype_SelectedIndexChanged);
			// 
			// label_emitterscale
			// 
			this->label_emitterscale->Location = System::Drawing::Point(168, 24);
			this->label_emitterscale->Name = L"label_emitterscale";
			this->label_emitterscale->Size = System::Drawing::Size(72, 20);
			this->label_emitterscale->TabIndex = 6;
			this->label_emitterscale->Text = L"Emitter Scale";
			this->label_emitterscale->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_emitterscale
			// 
			this->floatEdit_emitterscale->Location = System::Drawing::Point(264, 24);
			this->floatEdit_emitterscale->Name = L"floatEdit_emitterscale";
			this->floatEdit_emitterscale->Precision = static_cast<System::Int16>(2);
			this->floatEdit_emitterscale->Size = System::Drawing::Size(56, 24);
			this->floatEdit_emitterscale->TabIndex = 5;
			this->floatEdit_emitterscale->Value = 1;
			this->floatEdit_emitterscale->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_emitterscale_ValueChanged);
			// 
			// tabPage_particlestarttexture
			// 
			this->tabPage_particlestarttexture->Controls->Add(this->groupBox_texture_info);
			this->tabPage_particlestarttexture->Controls->Add(this->button_texture_movedown);
			this->tabPage_particlestarttexture->Controls->Add(this->button_texture_moveup);
			this->tabPage_particlestarttexture->Controls->Add(this->button_texture_del);
			this->tabPage_particlestarttexture->Controls->Add(this->button_texture_add);
			this->tabPage_particlestarttexture->Controls->Add(this->listBox_textures);
			this->tabPage_particlestarttexture->Controls->Add(this->groupBox_texture_alphalevels);
			this->tabPage_particlestarttexture->Controls->Add(this->groupBox_texture_scaling);
			this->tabPage_particlestarttexture->Controls->Add(this->groupBox_texture);
			this->tabPage_particlestarttexture->Controls->Add(this->groupBox_texture_alphablending);
			this->tabPage_particlestarttexture->Controls->Add(this->groupBox_texture_UVA);
			this->tabPage_particlestarttexture->Location = System::Drawing::Point(4, 22);
			this->tabPage_particlestarttexture->Name = L"tabPage_particlestarttexture";
			this->tabPage_particlestarttexture->Size = System::Drawing::Size(450, 620);
			this->tabPage_particlestarttexture->TabIndex = 1;
			this->tabPage_particlestarttexture->Text = L"Textures";
			this->tabPage_particlestarttexture->UseVisualStyleBackColor = true;
			// 
			// groupBox_texture_info
			// 
			this->groupBox_texture_info->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_texture_info->Controls->Add(this->label_texture_timeinfo);
			this->groupBox_texture_info->Controls->Add(this->floatEdit_texture_starttime);
			this->groupBox_texture_info->Controls->Add(this->label_texture_starttime);
			this->groupBox_texture_info->Controls->Add(this->textBox_texture_name);
			this->groupBox_texture_info->Controls->Add(this->floatEdit_texture_endtime);
			this->groupBox_texture_info->Controls->Add(this->label_texture_endtime);
			this->groupBox_texture_info->Controls->Add(this->label_texture_name);
			this->groupBox_texture_info->Location = System::Drawing::Point(8, 131);
			this->groupBox_texture_info->Name = L"groupBox_texture_info";
			this->groupBox_texture_info->Size = System::Drawing::Size(434, 72);
			this->groupBox_texture_info->TabIndex = 17;
			this->groupBox_texture_info->TabStop = false;
			this->groupBox_texture_info->Text = L"Texture Info";
			// 
			// label_texture_timeinfo
			// 
			this->label_texture_timeinfo->Location = System::Drawing::Point(308, 37);
			this->label_texture_timeinfo->Name = L"label_texture_timeinfo";
			this->label_texture_timeinfo->Size = System::Drawing::Size(100, 24);
			this->label_texture_timeinfo->TabIndex = 17;
			this->label_texture_timeinfo->Text = L"Set start and end to 0 for infinite";
			// 
			// floatEdit_texture_starttime
			// 
			this->floatEdit_texture_starttime->Enabled = false;
			this->floatEdit_texture_starttime->Location = System::Drawing::Point(72, 41);
			this->floatEdit_texture_starttime->Name = L"floatEdit_texture_starttime";
			this->floatEdit_texture_starttime->Precision = static_cast<System::Int16>(2);
			this->floatEdit_texture_starttime->Size = System::Drawing::Size(72, 21);
			this->floatEdit_texture_starttime->TabIndex = 13;
			this->floatEdit_texture_starttime->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_texture_starttime_ValueChanged);
			// 
			// label_texture_starttime
			// 
			this->label_texture_starttime->Location = System::Drawing::Point(8, 41);
			this->label_texture_starttime->Name = L"label_texture_starttime";
			this->label_texture_starttime->Size = System::Drawing::Size(64, 23);
			this->label_texture_starttime->TabIndex = 16;
			this->label_texture_starttime->Text = L"Start Time";
			this->label_texture_starttime->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox_texture_name
			// 
			this->textBox_texture_name->Enabled = false;
			this->textBox_texture_name->Location = System::Drawing::Point(72, 16);
			this->textBox_texture_name->Name = L"textBox_texture_name";
			this->textBox_texture_name->Size = System::Drawing::Size(168, 20);
			this->textBox_texture_name->TabIndex = 12;
			// 
			// floatEdit_texture_endtime
			// 
			this->floatEdit_texture_endtime->Enabled = false;
			this->floatEdit_texture_endtime->Location = System::Drawing::Point(224, 41);
			this->floatEdit_texture_endtime->Name = L"floatEdit_texture_endtime";
			this->floatEdit_texture_endtime->Precision = static_cast<System::Int16>(2);
			this->floatEdit_texture_endtime->Size = System::Drawing::Size(72, 21);
			this->floatEdit_texture_endtime->TabIndex = 14;
			this->floatEdit_texture_endtime->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_texture_endtime_ValueChanged);
			// 
			// label_texture_endtime
			// 
			this->label_texture_endtime->Location = System::Drawing::Point(160, 41);
			this->label_texture_endtime->Name = L"label_texture_endtime";
			this->label_texture_endtime->Size = System::Drawing::Size(56, 23);
			this->label_texture_endtime->TabIndex = 15;
			this->label_texture_endtime->Text = L"End Time";
			this->label_texture_endtime->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_texture_name
			// 
			this->label_texture_name->Location = System::Drawing::Point(8, 16);
			this->label_texture_name->Name = L"label_texture_name";
			this->label_texture_name->Size = System::Drawing::Size(48, 23);
			this->label_texture_name->TabIndex = 11;
			this->label_texture_name->Text = L"Name";
			this->label_texture_name->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// button_texture_movedown
			// 
			this->button_texture_movedown->Enabled = false;
			this->button_texture_movedown->Location = System::Drawing::Point(280, 101);
			this->button_texture_movedown->Name = L"button_texture_movedown";
			this->button_texture_movedown->Size = System::Drawing::Size(40, 23);
			this->button_texture_movedown->TabIndex = 10;
			this->button_texture_movedown->Text = L"down";
			// 
			// button_texture_moveup
			// 
			this->button_texture_moveup->Enabled = false;
			this->button_texture_moveup->Location = System::Drawing::Point(232, 101);
			this->button_texture_moveup->Name = L"button_texture_moveup";
			this->button_texture_moveup->Size = System::Drawing::Size(40, 23);
			this->button_texture_moveup->TabIndex = 9;
			this->button_texture_moveup->Text = L"up";
			// 
			// button_texture_del
			// 
			this->button_texture_del->Enabled = false;
			this->button_texture_del->Location = System::Drawing::Point(152, 101);
			this->button_texture_del->Name = L"button_texture_del";
			this->button_texture_del->Size = System::Drawing::Size(40, 23);
			this->button_texture_del->TabIndex = 7;
			this->button_texture_del->Text = L"del";
			// 
			// button_texture_add
			// 
			this->button_texture_add->Enabled = false;
			this->button_texture_add->Location = System::Drawing::Point(104, 101);
			this->button_texture_add->Name = L"button_texture_add";
			this->button_texture_add->Size = System::Drawing::Size(40, 23);
			this->button_texture_add->TabIndex = 6;
			this->button_texture_add->Text = L"add";
			// 
			// listBox_textures
			// 
			this->listBox_textures->Enabled = false;
			this->listBox_textures->Location = System::Drawing::Point(8, 10);
			this->listBox_textures->Name = L"listBox_textures";
			this->listBox_textures->Size = System::Drawing::Size(416, 82);
			this->listBox_textures->TabIndex = 5;
			// 
			// groupBox_texture_alphalevels
			// 
			this->groupBox_texture_alphalevels->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_texture_alphalevels->Controls->Add(this->rangedFloat_texture_alphalevel_middlepercentend);
			this->groupBox_texture_alphalevels->Controls->Add(this->label_texture_alphalevel_middlepercentend);
			this->groupBox_texture_alphalevels->Controls->Add(this->rangedFloat_texture_alphalevel_end);
			this->groupBox_texture_alphalevels->Controls->Add(this->rangedFloat_texture_alphalevel_middlepercentstart);
			this->groupBox_texture_alphalevels->Controls->Add(this->rangedFloat_texture_alphalevel_middle);
			this->groupBox_texture_alphalevels->Controls->Add(this->rangedFloat_texture_alphalevel_start);
			this->groupBox_texture_alphalevels->Controls->Add(this->label_texture_alphalevel_middlepercentstart);
			this->groupBox_texture_alphalevels->Controls->Add(this->label_texture_alphalevel_end);
			this->groupBox_texture_alphalevels->Controls->Add(this->label_texture_alphalevel_middle);
			this->groupBox_texture_alphalevels->Controls->Add(this->label_texture_alphalevel_start);
			this->groupBox_texture_alphalevels->Location = System::Drawing::Point(8, 413);
			this->groupBox_texture_alphalevels->Name = L"groupBox_texture_alphalevels";
			this->groupBox_texture_alphalevels->Size = System::Drawing::Size(434, 192);
			this->groupBox_texture_alphalevels->TabIndex = 4;
			this->groupBox_texture_alphalevels->TabStop = false;
			this->groupBox_texture_alphalevels->Text = L"Alpha Levels";
			// 
			// rangedFloat_texture_alphalevel_middlepercentend
			// 
			this->rangedFloat_texture_alphalevel_middlepercentend->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->rangedFloat_texture_alphalevel_middlepercentend->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat_texture_alphalevel_middlepercentend->Location = System::Drawing::Point(96, 160);
			this->rangedFloat_texture_alphalevel_middlepercentend->Name = L"rangedFloat_texture_alphalevel_middlepercentend";
			this->rangedFloat_texture_alphalevel_middlepercentend->NumTicks = static_cast<System::Int16>(100);
			this->rangedFloat_texture_alphalevel_middlepercentend->Precision = static_cast<System::Int16>(2);
			this->rangedFloat_texture_alphalevel_middlepercentend->ShowValue = true;
			this->rangedFloat_texture_alphalevel_middlepercentend->Size = System::Drawing::Size(322, 24);
			this->rangedFloat_texture_alphalevel_middlepercentend->TabIndex = 12;
			this->rangedFloat_texture_alphalevel_middlepercentend->Value = 0.5;
			this->rangedFloat_texture_alphalevel_middlepercentend->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::rangedFloat_texture_alphalevel_middlepercentend_ValueChanged);
			// 
			// label_texture_alphalevel_middlepercentend
			// 
			this->label_texture_alphalevel_middlepercentend->Location = System::Drawing::Point(16, 160);
			this->label_texture_alphalevel_middlepercentend->Name = L"label_texture_alphalevel_middlepercentend";
			this->label_texture_alphalevel_middlepercentend->Size = System::Drawing::Size(80, 24);
			this->label_texture_alphalevel_middlepercentend->TabIndex = 11;
			this->label_texture_alphalevel_middlepercentend->Text = L"Middle % End";
			this->label_texture_alphalevel_middlepercentend->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// rangedFloat_texture_alphalevel_end
			// 
			this->rangedFloat_texture_alphalevel_end->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->rangedFloat_texture_alphalevel_end->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat_texture_alphalevel_end->Location = System::Drawing::Point(96, 92);
			this->rangedFloat_texture_alphalevel_end->Name = L"rangedFloat_texture_alphalevel_end";
			this->rangedFloat_texture_alphalevel_end->NumTicks = static_cast<System::Int16>(100);
			this->rangedFloat_texture_alphalevel_end->Precision = static_cast<System::Int16>(2);
			this->rangedFloat_texture_alphalevel_end->ShowValue = true;
			this->rangedFloat_texture_alphalevel_end->Size = System::Drawing::Size(322, 24);
			this->rangedFloat_texture_alphalevel_end->TabIndex = 9;
			this->rangedFloat_texture_alphalevel_end->Value = 1;
			this->rangedFloat_texture_alphalevel_end->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::rangedFloat_texture_alphalevel_end_ValueChanged);
			// 
			// rangedFloat_texture_alphalevel_middlepercentstart
			// 
			this->rangedFloat_texture_alphalevel_middlepercentstart->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->rangedFloat_texture_alphalevel_middlepercentstart->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat_texture_alphalevel_middlepercentstart->Location = System::Drawing::Point(96, 126);
			this->rangedFloat_texture_alphalevel_middlepercentstart->Name = L"rangedFloat_texture_alphalevel_middlepercentstart";
			this->rangedFloat_texture_alphalevel_middlepercentstart->NumTicks = static_cast<System::Int16>(100);
			this->rangedFloat_texture_alphalevel_middlepercentstart->Precision = static_cast<System::Int16>(2);
			this->rangedFloat_texture_alphalevel_middlepercentstart->ShowValue = true;
			this->rangedFloat_texture_alphalevel_middlepercentstart->Size = System::Drawing::Size(322, 24);
			this->rangedFloat_texture_alphalevel_middlepercentstart->TabIndex = 10;
			this->rangedFloat_texture_alphalevel_middlepercentstart->Value = 0.5;
			this->rangedFloat_texture_alphalevel_middlepercentstart->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::rangedFloat_texture_alphalevel_middlepercentstart_ValueChanged);
			// 
			// rangedFloat_texture_alphalevel_middle
			// 
			this->rangedFloat_texture_alphalevel_middle->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->rangedFloat_texture_alphalevel_middle->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat_texture_alphalevel_middle->Location = System::Drawing::Point(96, 58);
			this->rangedFloat_texture_alphalevel_middle->Name = L"rangedFloat_texture_alphalevel_middle";
			this->rangedFloat_texture_alphalevel_middle->NumTicks = static_cast<System::Int16>(100);
			this->rangedFloat_texture_alphalevel_middle->Precision = static_cast<System::Int16>(2);
			this->rangedFloat_texture_alphalevel_middle->ShowValue = true;
			this->rangedFloat_texture_alphalevel_middle->Size = System::Drawing::Size(322, 24);
			this->rangedFloat_texture_alphalevel_middle->TabIndex = 8;
			this->rangedFloat_texture_alphalevel_middle->Value = 1;
			this->rangedFloat_texture_alphalevel_middle->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::rangedFloat_texture_alphalevel_middle_ValueChanged);
			// 
			// rangedFloat_texture_alphalevel_start
			// 
			this->rangedFloat_texture_alphalevel_start->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->rangedFloat_texture_alphalevel_start->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat_texture_alphalevel_start->Location = System::Drawing::Point(96, 24);
			this->rangedFloat_texture_alphalevel_start->Name = L"rangedFloat_texture_alphalevel_start";
			this->rangedFloat_texture_alphalevel_start->NumTicks = static_cast<System::Int16>(100);
			this->rangedFloat_texture_alphalevel_start->Precision = static_cast<System::Int16>(2);
			this->rangedFloat_texture_alphalevel_start->ShowValue = true;
			this->rangedFloat_texture_alphalevel_start->Size = System::Drawing::Size(322, 24);
			this->rangedFloat_texture_alphalevel_start->TabIndex = 7;
			this->rangedFloat_texture_alphalevel_start->Value = 1;
			this->rangedFloat_texture_alphalevel_start->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::rangedFloat_texture_alphalevel_start_ValueChanged);
			// 
			// label_texture_alphalevel_middlepercentstart
			// 
			this->label_texture_alphalevel_middlepercentstart->Location = System::Drawing::Point(16, 126);
			this->label_texture_alphalevel_middlepercentstart->Name = L"label_texture_alphalevel_middlepercentstart";
			this->label_texture_alphalevel_middlepercentstart->Size = System::Drawing::Size(80, 24);
			this->label_texture_alphalevel_middlepercentstart->TabIndex = 7;
			this->label_texture_alphalevel_middlepercentstart->Text = L"Middle % Start";
			this->label_texture_alphalevel_middlepercentstart->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_texture_alphalevel_end
			// 
			this->label_texture_alphalevel_end->Location = System::Drawing::Point(16, 92);
			this->label_texture_alphalevel_end->Name = L"label_texture_alphalevel_end";
			this->label_texture_alphalevel_end->Size = System::Drawing::Size(32, 24);
			this->label_texture_alphalevel_end->TabIndex = 5;
			this->label_texture_alphalevel_end->Text = L"End";
			this->label_texture_alphalevel_end->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_texture_alphalevel_middle
			// 
			this->label_texture_alphalevel_middle->Location = System::Drawing::Point(16, 58);
			this->label_texture_alphalevel_middle->Name = L"label_texture_alphalevel_middle";
			this->label_texture_alphalevel_middle->Size = System::Drawing::Size(40, 24);
			this->label_texture_alphalevel_middle->TabIndex = 3;
			this->label_texture_alphalevel_middle->Text = L"Middle";
			this->label_texture_alphalevel_middle->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_texture_alphalevel_start
			// 
			this->label_texture_alphalevel_start->Location = System::Drawing::Point(16, 24);
			this->label_texture_alphalevel_start->Name = L"label_texture_alphalevel_start";
			this->label_texture_alphalevel_start->Size = System::Drawing::Size(32, 24);
			this->label_texture_alphalevel_start->TabIndex = 1;
			this->label_texture_alphalevel_start->Text = L"Start";
			this->label_texture_alphalevel_start->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox_texture_scaling
			// 
			this->groupBox_texture_scaling->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_texture_scaling->Controls->Add(this->radioButton_texture_scaling_exponential);
			this->groupBox_texture_scaling->Controls->Add(this->radioButton_texture_scaling_linear);
			this->groupBox_texture_scaling->Location = System::Drawing::Point(8, 292);
			this->groupBox_texture_scaling->Name = L"groupBox_texture_scaling";
			this->groupBox_texture_scaling->Size = System::Drawing::Size(434, 40);
			this->groupBox_texture_scaling->TabIndex = 1;
			this->groupBox_texture_scaling->TabStop = false;
			this->groupBox_texture_scaling->Text = L"Scaling";
			// 
			// radioButton_texture_scaling_exponential
			// 
			this->radioButton_texture_scaling_exponential->Checked = true;
			this->radioButton_texture_scaling_exponential->Location = System::Drawing::Point(224, 14);
			this->radioButton_texture_scaling_exponential->Name = L"radioButton_texture_scaling_exponential";
			this->radioButton_texture_scaling_exponential->Size = System::Drawing::Size(104, 18);
			this->radioButton_texture_scaling_exponential->TabIndex = 2;
			this->radioButton_texture_scaling_exponential->TabStop = true;
			this->radioButton_texture_scaling_exponential->Text = L"Exponential";
			this->radioButton_texture_scaling_exponential->CheckedChanged += gcnew System::EventHandler(this, &ParticleDialog::radioButton_texture_scaling_exponential_CheckedChanged);
			// 
			// radioButton_texture_scaling_linear
			// 
			this->radioButton_texture_scaling_linear->Location = System::Drawing::Point(64, 14);
			this->radioButton_texture_scaling_linear->Name = L"radioButton_texture_scaling_linear";
			this->radioButton_texture_scaling_linear->Size = System::Drawing::Size(104, 18);
			this->radioButton_texture_scaling_linear->TabIndex = 0;
			this->radioButton_texture_scaling_linear->TabStop = true;
			this->radioButton_texture_scaling_linear->Text = L"Linear";
			this->radioButton_texture_scaling_linear->CheckedChanged += gcnew System::EventHandler(this, &ParticleDialog::radioButton_texture_scaling_linear_CheckedChanged);
			// 
			// groupBox_texture
			// 
			this->groupBox_texture->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_texture->Controls->Add(this->numericUpDown_texture_framerate);
			this->groupBox_texture->Controls->Add(this->label_texture_framerate);
			this->groupBox_texture->Controls->Add(this->checkBox_texture_looping);
			this->groupBox_texture->Controls->Add(this->numericUpDown_texture_frames);
			this->groupBox_texture->Controls->Add(this->label_texture_frames);
			this->groupBox_texture->Controls->Add(this->fileChooser_texture);
			this->groupBox_texture->Location = System::Drawing::Point(8, 204);
			this->groupBox_texture->Name = L"groupBox_texture";
			this->groupBox_texture->Size = System::Drawing::Size(434, 88);
			this->groupBox_texture->TabIndex = 0;
			this->groupBox_texture->TabStop = false;
			this->groupBox_texture->Text = L"Texture";
			// 
			// numericUpDown_texture_framerate
			// 
			this->numericUpDown_texture_framerate->Enabled = false;
			this->numericUpDown_texture_framerate->Location = System::Drawing::Point(224, 57);
			this->numericUpDown_texture_framerate->Name = L"numericUpDown_texture_framerate";
			this->numericUpDown_texture_framerate->Size = System::Drawing::Size(48, 20);
			this->numericUpDown_texture_framerate->TabIndex = 5;
			this->numericUpDown_texture_framerate->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::numericUpDown_texture_framerate_ValueChanged);
			// 
			// label_texture_framerate
			// 
			this->label_texture_framerate->Location = System::Drawing::Point(152, 56);
			this->label_texture_framerate->Name = L"label_texture_framerate";
			this->label_texture_framerate->Size = System::Drawing::Size(64, 23);
			this->label_texture_framerate->TabIndex = 4;
			this->label_texture_framerate->Text = L"Frame Rate";
			this->label_texture_framerate->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// checkBox_texture_looping
			// 
			this->checkBox_texture_looping->Enabled = false;
			this->checkBox_texture_looping->Location = System::Drawing::Point(312, 56);
			this->checkBox_texture_looping->Name = L"checkBox_texture_looping";
			this->checkBox_texture_looping->Size = System::Drawing::Size(64, 23);
			this->checkBox_texture_looping->TabIndex = 3;
			this->checkBox_texture_looping->Text = L"Looping";
			this->checkBox_texture_looping->CheckedChanged += gcnew System::EventHandler(this, &ParticleDialog::checkBox_texture_looping_CheckedChanged);
			// 
			// numericUpDown_texture_frames
			// 
			this->numericUpDown_texture_frames->Enabled = false;
			this->numericUpDown_texture_frames->Location = System::Drawing::Point(80, 57);
			this->numericUpDown_texture_frames->Name = L"numericUpDown_texture_frames";
			this->numericUpDown_texture_frames->Size = System::Drawing::Size(48, 20);
			this->numericUpDown_texture_frames->TabIndex = 2;
			this->numericUpDown_texture_frames->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::numericUpDown_texture_frames_ValueChanged);
			// 
			// label_texture_frames
			// 
			this->label_texture_frames->Location = System::Drawing::Point(24, 56);
			this->label_texture_frames->Name = L"label_texture_frames";
			this->label_texture_frames->Size = System::Drawing::Size(48, 23);
			this->label_texture_frames->TabIndex = 1;
			this->label_texture_frames->Text = L"Frames";
			this->label_texture_frames->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// fileChooser_texture
			// 
			this->fileChooser_texture->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->fileChooser_texture->Location = System::Drawing::Point(16, 19);
			this->fileChooser_texture->Name = L"fileChooser_texture";
			this->fileChooser_texture->Size = System::Drawing::Size(402, 24);
			this->fileChooser_texture->TabIndex = 0;
			this->fileChooser_texture->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::fileChooser_texture_ValueChanged);
			// 
			// groupBox_texture_alphablending
			// 
			this->groupBox_texture_alphablending->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_texture_alphablending->Controls->Add(this->radioButton_texture_alphablending_multiply);
			this->groupBox_texture_alphablending->Controls->Add(this->radioButton_texture_alphablending_add);
			this->groupBox_texture_alphablending->Location = System::Drawing::Point(8, 332);
			this->groupBox_texture_alphablending->Name = L"groupBox_texture_alphablending";
			this->groupBox_texture_alphablending->Size = System::Drawing::Size(434, 40);
			this->groupBox_texture_alphablending->TabIndex = 2;
			this->groupBox_texture_alphablending->TabStop = false;
			this->groupBox_texture_alphablending->Text = L"Alpha Blending";
			// 
			// radioButton_texture_alphablending_multiply
			// 
			this->radioButton_texture_alphablending_multiply->Checked = true;
			this->radioButton_texture_alphablending_multiply->Location = System::Drawing::Point(224, 16);
			this->radioButton_texture_alphablending_multiply->Name = L"radioButton_texture_alphablending_multiply";
			this->radioButton_texture_alphablending_multiply->Size = System::Drawing::Size(104, 18);
			this->radioButton_texture_alphablending_multiply->TabIndex = 4;
			this->radioButton_texture_alphablending_multiply->TabStop = true;
			this->radioButton_texture_alphablending_multiply->Text = L"Multiply";
			this->radioButton_texture_alphablending_multiply->CheckedChanged += gcnew System::EventHandler(this, &ParticleDialog::radioButton_texture_alphablending_multiply_CheckedChanged);
			// 
			// radioButton_texture_alphablending_add
			// 
			this->radioButton_texture_alphablending_add->Location = System::Drawing::Point(64, 16);
			this->radioButton_texture_alphablending_add->Name = L"radioButton_texture_alphablending_add";
			this->radioButton_texture_alphablending_add->Size = System::Drawing::Size(104, 18);
			this->radioButton_texture_alphablending_add->TabIndex = 3;
			this->radioButton_texture_alphablending_add->TabStop = true;
			this->radioButton_texture_alphablending_add->Text = L"Add";
			this->radioButton_texture_alphablending_add->CheckedChanged += gcnew System::EventHandler(this, &ParticleDialog::radioButton_texture_alphablending_add_CheckedChanged);
			// 
			// groupBox_texture_UVA
			// 
			this->groupBox_texture_UVA->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_texture_UVA->Controls->Add(this->radioButton_texture_UVA_TUV);
			this->groupBox_texture_UVA->Controls->Add(this->radioButton_texture_UVA_random);
			this->groupBox_texture_UVA->Controls->Add(this->radioButton_texture_UVA_Lifetime);
			this->groupBox_texture_UVA->Location = System::Drawing::Point(8, 372);
			this->groupBox_texture_UVA->Name = L"groupBox_texture_UVA";
			this->groupBox_texture_UVA->Size = System::Drawing::Size(434, 40);
			this->groupBox_texture_UVA->TabIndex = 3;
			this->groupBox_texture_UVA->TabStop = false;
			this->groupBox_texture_UVA->Text = L"UVA";
			// 
			// radioButton_texture_UVA_TUV
			// 
			this->radioButton_texture_UVA_TUV->Location = System::Drawing::Point(224, 16);
			this->radioButton_texture_UVA_TUV->Name = L"radioButton_texture_UVA_TUV";
			this->radioButton_texture_UVA_TUV->Size = System::Drawing::Size(104, 18);
			this->radioButton_texture_UVA_TUV->TabIndex = 7;
			this->radioButton_texture_UVA_TUV->TabStop = true;
			this->radioButton_texture_UVA_TUV->Text = L"TUV framerate";
			this->radioButton_texture_UVA_TUV->CheckedChanged += gcnew System::EventHandler(this, &ParticleDialog::radioButton_texture_UVA_TUV_CheckedChanged);
			// 
			// radioButton_texture_UVA_random
			// 
			this->radioButton_texture_UVA_random->Location = System::Drawing::Point(143, 16);
			this->radioButton_texture_UVA_random->Name = L"radioButton_texture_UVA_random";
			this->radioButton_texture_UVA_random->Size = System::Drawing::Size(75, 18);
			this->radioButton_texture_UVA_random->TabIndex = 6;
			this->radioButton_texture_UVA_random->TabStop = true;
			this->radioButton_texture_UVA_random->Text = L"Random";
			this->radioButton_texture_UVA_random->CheckedChanged += gcnew System::EventHandler(this, &ParticleDialog::radioButton_texture_UVA_random_CheckedChanged);
			// 
			// radioButton_texture_UVA_Lifetime
			// 
			this->radioButton_texture_UVA_Lifetime->Location = System::Drawing::Point(64, 16);
			this->radioButton_texture_UVA_Lifetime->Name = L"radioButton_texture_UVA_Lifetime";
			this->radioButton_texture_UVA_Lifetime->Size = System::Drawing::Size(104, 18);
			this->radioButton_texture_UVA_Lifetime->TabIndex = 5;
			this->radioButton_texture_UVA_Lifetime->TabStop = true;
			this->radioButton_texture_UVA_Lifetime->Text = L"Lifetime";
			this->radioButton_texture_UVA_Lifetime->CheckedChanged += gcnew System::EventHandler(this, &ParticleDialog::radioButton_texture_UVA_Lifetime_CheckedChanged);
			// 
			// tabPage_streaking
			// 
			this->tabPage_streaking->Controls->Add(this->groupBox_streaking);
			this->tabPage_streaking->Location = System::Drawing::Point(4, 22);
			this->tabPage_streaking->Name = L"tabPage_streaking";
			this->tabPage_streaking->Size = System::Drawing::Size(450, 620);
			this->tabPage_streaking->TabIndex = 9;
			this->tabPage_streaking->Text = L"Streaking";
			this->tabPage_streaking->UseVisualStyleBackColor = true;
			// 
			// groupBox_streaking
			// 
			this->groupBox_streaking->Controls->Add(this->rangedFloat_streak_fade);
			this->groupBox_streaking->Controls->Add(this->label3);
			this->groupBox_streaking->Controls->Add(this->rangedFloat_streak_taper);
			this->groupBox_streaking->Controls->Add(this->label2);
			this->groupBox_streaking->Controls->Add(this->rangedFloat_streak_length);
			this->groupBox_streaking->Controls->Add(this->label1);
			this->groupBox_streaking->Controls->Add(this->checkBox_streaking);
			this->groupBox_streaking->Location = System::Drawing::Point(14, 13);
			this->groupBox_streaking->Name = L"groupBox_streaking";
			this->groupBox_streaking->Size = System::Drawing::Size(420, 176);
			this->groupBox_streaking->TabIndex = 1;
			this->groupBox_streaking->TabStop = false;
			this->groupBox_streaking->Text = L"Streaking";
			// 
			// rangedFloat_streak_fade
			// 
			this->rangedFloat_streak_fade->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->rangedFloat_streak_fade->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat_streak_fade->Location = System::Drawing::Point(83, 126);
			this->rangedFloat_streak_fade->Name = L"rangedFloat_streak_fade";
			this->rangedFloat_streak_fade->NumTicks = static_cast<System::Int16>(100);
			this->rangedFloat_streak_fade->Precision = static_cast<System::Int16>(2);
			this->rangedFloat_streak_fade->ShowValue = true;
			this->rangedFloat_streak_fade->Size = System::Drawing::Size(322, 24);
			this->rangedFloat_streak_fade->TabIndex = 13;
			this->rangedFloat_streak_fade->Value = 1;
			this->rangedFloat_streak_fade->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::rangedFloat_streak_fade_ValueChanged);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(3, 126);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(53, 24);
			this->label3->TabIndex = 12;
			this->label3->Text = L"Fade";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// rangedFloat_streak_taper
			// 
			this->rangedFloat_streak_taper->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->rangedFloat_streak_taper->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat_streak_taper->Location = System::Drawing::Point(83, 85);
			this->rangedFloat_streak_taper->Name = L"rangedFloat_streak_taper";
			this->rangedFloat_streak_taper->NumTicks = static_cast<System::Int16>(100);
			this->rangedFloat_streak_taper->Precision = static_cast<System::Int16>(2);
			this->rangedFloat_streak_taper->ShowValue = true;
			this->rangedFloat_streak_taper->Size = System::Drawing::Size(322, 24);
			this->rangedFloat_streak_taper->TabIndex = 11;
			this->rangedFloat_streak_taper->Value = 1;
			this->rangedFloat_streak_taper->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::rangedFloat_streak_taper_ValueChanged);
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(3, 85);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(53, 24);
			this->label2->TabIndex = 10;
			this->label2->Text = L"Taper";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// rangedFloat_streak_length
			// 
			this->rangedFloat_streak_length->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->rangedFloat_streak_length->Exponent = static_cast<System::Int16>(1);
			this->rangedFloat_streak_length->Location = System::Drawing::Point(83, 49);
			this->rangedFloat_streak_length->Name = L"rangedFloat_streak_length";
			this->rangedFloat_streak_length->NumTicks = static_cast<System::Int16>(100);
			this->rangedFloat_streak_length->Precision = static_cast<System::Int16>(2);
			this->rangedFloat_streak_length->ShowValue = true;
			this->rangedFloat_streak_length->Size = System::Drawing::Size(322, 24);
			this->rangedFloat_streak_length->TabIndex = 9;
			this->rangedFloat_streak_length->Value = 1;
			this->rangedFloat_streak_length->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::rangedFloat_streak_length_ValueChanged);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(3, 49);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(53, 24);
			this->label1->TabIndex = 8;
			this->label1->Text = L"Length";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// checkBox_streaking
			// 
			this->checkBox_streaking->AutoSize = true;
			this->checkBox_streaking->Location = System::Drawing::Point(6, 19);
			this->checkBox_streaking->Name = L"checkBox_streaking";
			this->checkBox_streaking->Size = System::Drawing::Size(154, 17);
			this->checkBox_streaking->TabIndex = 0;
			this->checkBox_streaking->Text = L"Enable streaking of  motion";
			this->checkBox_streaking->UseVisualStyleBackColor = true;
			this->checkBox_streaking->CheckedChanged += gcnew System::EventHandler(this, &ParticleDialog::checkBox_streaking_CheckedChanged);
			// 
			// tabPage_particletype_generic
			// 
			this->tabPage_particletype_generic->Controls->Add(this->groupBox_assettype);
			this->tabPage_particletype_generic->Controls->Add(this->groupBox_geometry);
			this->tabPage_particletype_generic->Controls->Add(this->groupBox_particledata_generic_particle);
			this->tabPage_particletype_generic->Location = System::Drawing::Point(4, 22);
			this->tabPage_particletype_generic->Name = L"tabPage_particletype_generic";
			this->tabPage_particletype_generic->Size = System::Drawing::Size(450, 620);
			this->tabPage_particletype_generic->TabIndex = 8;
			this->tabPage_particletype_generic->Text = L"Particle";
			this->tabPage_particletype_generic->UseVisualStyleBackColor = true;
			// 
			// groupBox_assettype
			// 
			this->groupBox_assettype->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_assettype->Controls->Add(this->radioButton_assettype_geometry);
			this->groupBox_assettype->Controls->Add(this->radioButton_assettype_1texture);
			this->groupBox_assettype->Location = System::Drawing::Point(8, 8);
			this->groupBox_assettype->Name = L"groupBox_assettype";
			this->groupBox_assettype->Size = System::Drawing::Size(426, 48);
			this->groupBox_assettype->TabIndex = 13;
			this->groupBox_assettype->TabStop = false;
			this->groupBox_assettype->Text = L"Particle Asset Type";
			// 
			// radioButton_assettype_geometry
			// 
			this->radioButton_assettype_geometry->Enabled = false;
			this->radioButton_assettype_geometry->Location = System::Drawing::Point(160, 18);
			this->radioButton_assettype_geometry->Name = L"radioButton_assettype_geometry";
			this->radioButton_assettype_geometry->Size = System::Drawing::Size(104, 24);
			this->radioButton_assettype_geometry->TabIndex = 1;
			this->radioButton_assettype_geometry->Text = L"Geometry";
			this->radioButton_assettype_geometry->CheckedChanged += gcnew System::EventHandler(this, &ParticleDialog::radioButton_assettype_geometry_CheckedChanged);
			// 
			// radioButton_assettype_1texture
			// 
			this->radioButton_assettype_1texture->Checked = true;
			this->radioButton_assettype_1texture->Location = System::Drawing::Point(16, 18);
			this->radioButton_assettype_1texture->Name = L"radioButton_assettype_1texture";
			this->radioButton_assettype_1texture->Size = System::Drawing::Size(104, 24);
			this->radioButton_assettype_1texture->TabIndex = 0;
			this->radioButton_assettype_1texture->TabStop = true;
			this->radioButton_assettype_1texture->Text = L"Single Texture";
			this->radioButton_assettype_1texture->CheckedChanged += gcnew System::EventHandler(this, &ParticleDialog::radioButton_assettype_1texture_CheckedChanged);
			// 
			// groupBox_geometry
			// 
			this->groupBox_geometry->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_geometry->Controls->Add(this->fileChooser_geometry);
			this->groupBox_geometry->Location = System::Drawing::Point(8, 64);
			this->groupBox_geometry->Name = L"groupBox_geometry";
			this->groupBox_geometry->Size = System::Drawing::Size(426, 64);
			this->groupBox_geometry->TabIndex = 12;
			this->groupBox_geometry->TabStop = false;
			this->groupBox_geometry->Text = L"Geometry";
			// 
			// fileChooser_geometry
			// 
			this->fileChooser_geometry->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->fileChooser_geometry->Location = System::Drawing::Point(16, 24);
			this->fileChooser_geometry->Name = L"fileChooser_geometry";
			this->fileChooser_geometry->Size = System::Drawing::Size(394, 24);
			this->fileChooser_geometry->TabIndex = 0;
			this->fileChooser_geometry->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::fileChooser_geometry_ValueChanged);
			// 
			// groupBox_particledata_generic_particle
			// 
			this->groupBox_particledata_generic_particle->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_particledata_generic_particle->Controls->Add(this->floatEdit_particledata_generic_particleangularacceleration_min);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particleangularacceleration_max);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particleangularacceleration_min);
			this->groupBox_particledata_generic_particle->Controls->Add(this->floatEdit_particledata_generic_particleangularacceleration_max);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particleangularacceleration);
			this->groupBox_particledata_generic_particle->Controls->Add(this->floatEdit_particledata_generic_particleangularvelocity_min);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particleangularvelocity_max);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particleangularvelocity_min);
			this->groupBox_particledata_generic_particle->Controls->Add(this->floatEdit_particledata_generic_particleangularvelocity_max);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particleangularvelocity);
			this->groupBox_particledata_generic_particle->Controls->Add(this->floatEdit_particledata_generic_particlestartangle_min);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particlestartangle_max);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particlestartangle_min);
			this->groupBox_particledata_generic_particle->Controls->Add(this->floatEdit_particledata_generic_particlestartangle_max);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particlestartangle);
			this->groupBox_particledata_generic_particle->Controls->Add(this->floatEdit_particledata_generic_particlescale_start);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particlescale_start);
			this->groupBox_particledata_generic_particle->Controls->Add(this->floatEdit_particledata_generic_particlescalecoefficient);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particlescalecoefficient);
			this->groupBox_particledata_generic_particle->Controls->Add(this->label_particledata_generic_particlescale);
			this->groupBox_particledata_generic_particle->Location = System::Drawing::Point(8, 136);
			this->groupBox_particledata_generic_particle->Name = L"groupBox_particledata_generic_particle";
			this->groupBox_particledata_generic_particle->Size = System::Drawing::Size(426, 160);
			this->groupBox_particledata_generic_particle->TabIndex = 10;
			this->groupBox_particledata_generic_particle->TabStop = false;
			this->groupBox_particledata_generic_particle->Text = L"Parameters";
			// 
			// floatEdit_particledata_generic_particleangularacceleration_min
			// 
			this->floatEdit_particledata_generic_particleangularacceleration_min->Location = System::Drawing::Point(112, 120);
			this->floatEdit_particledata_generic_particleangularacceleration_min->Name = L"floatEdit_particledata_generic_particleangularacceleration_min";
			this->floatEdit_particledata_generic_particleangularacceleration_min->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_particleangularacceleration_min->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_particleangularacceleration_min->TabIndex = 10;
			this->floatEdit_particledata_generic_particleangularacceleration_min->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_particleangularacceleration_min_ValueChanged);
			// 
			// label_particledata_generic_particleangularacceleration_max
			// 
			this->label_particledata_generic_particleangularacceleration_max->Location = System::Drawing::Point(336, 124);
			this->label_particledata_generic_particleangularacceleration_max->Name = L"label_particledata_generic_particleangularacceleration_max";
			this->label_particledata_generic_particleangularacceleration_max->Size = System::Drawing::Size(40, 16);
			this->label_particledata_generic_particleangularacceleration_max->TabIndex = 21;
			this->label_particledata_generic_particleangularacceleration_max->Text = L"max";
			this->label_particledata_generic_particleangularacceleration_max->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_generic_particleangularacceleration_min
			// 
			this->label_particledata_generic_particleangularacceleration_min->Location = System::Drawing::Point(200, 124);
			this->label_particledata_generic_particleangularacceleration_min->Name = L"label_particledata_generic_particleangularacceleration_min";
			this->label_particledata_generic_particleangularacceleration_min->Size = System::Drawing::Size(40, 16);
			this->label_particledata_generic_particleangularacceleration_min->TabIndex = 20;
			this->label_particledata_generic_particleangularacceleration_min->Text = L"min";
			this->label_particledata_generic_particleangularacceleration_min->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_generic_particleangularacceleration_max
			// 
			this->floatEdit_particledata_generic_particleangularacceleration_max->Location = System::Drawing::Point(248, 120);
			this->floatEdit_particledata_generic_particleangularacceleration_max->Name = L"floatEdit_particledata_generic_particleangularacceleration_max";
			this->floatEdit_particledata_generic_particleangularacceleration_max->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_particleangularacceleration_max->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_particleangularacceleration_max->TabIndex = 11;
			this->floatEdit_particledata_generic_particleangularacceleration_max->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_particleangularacceleration_max_ValueChanged);
			// 
			// label_particledata_generic_particleangularacceleration
			// 
			this->label_particledata_generic_particleangularacceleration->Location = System::Drawing::Point(16, 125);
			this->label_particledata_generic_particleangularacceleration->Name = L"label_particledata_generic_particleangularacceleration";
			this->label_particledata_generic_particleangularacceleration->Size = System::Drawing::Size(88, 14);
			this->label_particledata_generic_particleangularacceleration->TabIndex = 18;
			this->label_particledata_generic_particleangularacceleration->Text = L"Angular Accel.";
			this->label_particledata_generic_particleangularacceleration->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_generic_particleangularvelocity_min
			// 
			this->floatEdit_particledata_generic_particleangularvelocity_min->Location = System::Drawing::Point(112, 96);
			this->floatEdit_particledata_generic_particleangularvelocity_min->Name = L"floatEdit_particledata_generic_particleangularvelocity_min";
			this->floatEdit_particledata_generic_particleangularvelocity_min->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_particleangularvelocity_min->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_particleangularvelocity_min->TabIndex = 8;
			this->floatEdit_particledata_generic_particleangularvelocity_min->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_particleangularvelocity_min_ValueChanged);
			// 
			// label_particledata_generic_particleangularvelocity_max
			// 
			this->label_particledata_generic_particleangularvelocity_max->Location = System::Drawing::Point(336, 100);
			this->label_particledata_generic_particleangularvelocity_max->Name = L"label_particledata_generic_particleangularvelocity_max";
			this->label_particledata_generic_particleangularvelocity_max->Size = System::Drawing::Size(40, 16);
			this->label_particledata_generic_particleangularvelocity_max->TabIndex = 16;
			this->label_particledata_generic_particleangularvelocity_max->Text = L"max";
			this->label_particledata_generic_particleangularvelocity_max->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_generic_particleangularvelocity_min
			// 
			this->label_particledata_generic_particleangularvelocity_min->Location = System::Drawing::Point(200, 100);
			this->label_particledata_generic_particleangularvelocity_min->Name = L"label_particledata_generic_particleangularvelocity_min";
			this->label_particledata_generic_particleangularvelocity_min->Size = System::Drawing::Size(40, 16);
			this->label_particledata_generic_particleangularvelocity_min->TabIndex = 15;
			this->label_particledata_generic_particleangularvelocity_min->Text = L"min";
			this->label_particledata_generic_particleangularvelocity_min->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_generic_particleangularvelocity_max
			// 
			this->floatEdit_particledata_generic_particleangularvelocity_max->Location = System::Drawing::Point(248, 96);
			this->floatEdit_particledata_generic_particleangularvelocity_max->Name = L"floatEdit_particledata_generic_particleangularvelocity_max";
			this->floatEdit_particledata_generic_particleangularvelocity_max->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_particleangularvelocity_max->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_particleangularvelocity_max->TabIndex = 9;
			this->floatEdit_particledata_generic_particleangularvelocity_max->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_particleangularvelocity_max_ValueChanged);
			// 
			// label_particledata_generic_particleangularvelocity
			// 
			this->label_particledata_generic_particleangularvelocity->Location = System::Drawing::Point(16, 101);
			this->label_particledata_generic_particleangularvelocity->Name = L"label_particledata_generic_particleangularvelocity";
			this->label_particledata_generic_particleangularvelocity->Size = System::Drawing::Size(88, 14);
			this->label_particledata_generic_particleangularvelocity->TabIndex = 13;
			this->label_particledata_generic_particleangularvelocity->Text = L"Angular Velocity";
			this->label_particledata_generic_particleangularvelocity->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_generic_particlestartangle_min
			// 
			this->floatEdit_particledata_generic_particlestartangle_min->Location = System::Drawing::Point(112, 72);
			this->floatEdit_particledata_generic_particlestartangle_min->Name = L"floatEdit_particledata_generic_particlestartangle_min";
			this->floatEdit_particledata_generic_particlestartangle_min->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_particlestartangle_min->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_particlestartangle_min->TabIndex = 6;
			this->floatEdit_particledata_generic_particlestartangle_min->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_particlestartangle_min_ValueChanged);
			// 
			// label_particledata_generic_particlestartangle_max
			// 
			this->label_particledata_generic_particlestartangle_max->Location = System::Drawing::Point(336, 76);
			this->label_particledata_generic_particlestartangle_max->Name = L"label_particledata_generic_particlestartangle_max";
			this->label_particledata_generic_particlestartangle_max->Size = System::Drawing::Size(40, 16);
			this->label_particledata_generic_particlestartangle_max->TabIndex = 11;
			this->label_particledata_generic_particlestartangle_max->Text = L"max";
			this->label_particledata_generic_particlestartangle_max->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_generic_particlestartangle_min
			// 
			this->label_particledata_generic_particlestartangle_min->Location = System::Drawing::Point(200, 76);
			this->label_particledata_generic_particlestartangle_min->Name = L"label_particledata_generic_particlestartangle_min";
			this->label_particledata_generic_particlestartangle_min->Size = System::Drawing::Size(40, 16);
			this->label_particledata_generic_particlestartangle_min->TabIndex = 10;
			this->label_particledata_generic_particlestartangle_min->Text = L"min";
			this->label_particledata_generic_particlestartangle_min->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_generic_particlestartangle_max
			// 
			this->floatEdit_particledata_generic_particlestartangle_max->Location = System::Drawing::Point(248, 72);
			this->floatEdit_particledata_generic_particlestartangle_max->Name = L"floatEdit_particledata_generic_particlestartangle_max";
			this->floatEdit_particledata_generic_particlestartangle_max->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_particlestartangle_max->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_particlestartangle_max->TabIndex = 7;
			this->floatEdit_particledata_generic_particlestartangle_max->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_particlestartangle_max_ValueChanged);
			// 
			// label_particledata_generic_particlestartangle
			// 
			this->label_particledata_generic_particlestartangle->Location = System::Drawing::Point(16, 77);
			this->label_particledata_generic_particlestartangle->Name = L"label_particledata_generic_particlestartangle";
			this->label_particledata_generic_particlestartangle->Size = System::Drawing::Size(88, 14);
			this->label_particledata_generic_particlestartangle->TabIndex = 8;
			this->label_particledata_generic_particlestartangle->Text = L"Start Angle";
			this->label_particledata_generic_particlestartangle->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_generic_particlescale_start
			// 
			this->floatEdit_particledata_generic_particlescale_start->Location = System::Drawing::Point(112, 24);
			this->floatEdit_particledata_generic_particlescale_start->Name = L"floatEdit_particledata_generic_particlescale_start";
			this->floatEdit_particledata_generic_particlescale_start->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_particlescale_start->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_particlescale_start->TabIndex = 4;
			this->floatEdit_particledata_generic_particlescale_start->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_particlescale_start_ValueChanged);
			// 
			// label_particledata_generic_particlescale_start
			// 
			this->label_particledata_generic_particlescale_start->Location = System::Drawing::Point(200, 28);
			this->label_particledata_generic_particlescale_start->Name = L"label_particledata_generic_particlescale_start";
			this->label_particledata_generic_particlescale_start->Size = System::Drawing::Size(40, 16);
			this->label_particledata_generic_particlescale_start->TabIndex = 5;
			this->label_particledata_generic_particlescale_start->Text = L"start";
			this->label_particledata_generic_particlescale_start->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_generic_particlescalecoefficient
			// 
			this->floatEdit_particledata_generic_particlescalecoefficient->Location = System::Drawing::Point(112, 48);
			this->floatEdit_particledata_generic_particlescalecoefficient->Name = L"floatEdit_particledata_generic_particlescalecoefficient";
			this->floatEdit_particledata_generic_particlescalecoefficient->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_generic_particlescalecoefficient->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_generic_particlescalecoefficient->TabIndex = 5;
			this->floatEdit_particledata_generic_particlescalecoefficient->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_generic_particlescalecoefficient_ValueChanged);
			// 
			// label_particledata_generic_particlescalecoefficient
			// 
			this->label_particledata_generic_particlescalecoefficient->Location = System::Drawing::Point(16, 53);
			this->label_particledata_generic_particlescalecoefficient->Name = L"label_particledata_generic_particlescalecoefficient";
			this->label_particledata_generic_particlescalecoefficient->Size = System::Drawing::Size(88, 14);
			this->label_particledata_generic_particlescalecoefficient->TabIndex = 3;
			this->label_particledata_generic_particlescalecoefficient->Text = L"Scale Coeff.";
			this->label_particledata_generic_particlescalecoefficient->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_generic_particlescale
			// 
			this->label_particledata_generic_particlescale->Location = System::Drawing::Point(16, 29);
			this->label_particledata_generic_particlescale->Name = L"label_particledata_generic_particlescale";
			this->label_particledata_generic_particlescale->Size = System::Drawing::Size(88, 14);
			this->label_particledata_generic_particlescale->TabIndex = 1;
			this->label_particledata_generic_particlescale->Text = L"Scale";
			this->label_particledata_generic_particlescale->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tabPage_particletype_cone
			// 
			this->tabPage_particletype_cone->Controls->Add(this->groupBox_particledata_cone);
			this->tabPage_particletype_cone->Location = System::Drawing::Point(4, 22);
			this->tabPage_particletype_cone->Name = L"tabPage_particletype_cone";
			this->tabPage_particletype_cone->Size = System::Drawing::Size(450, 620);
			this->tabPage_particletype_cone->TabIndex = 4;
			this->tabPage_particletype_cone->Text = L"Cone";
			this->tabPage_particletype_cone->UseVisualStyleBackColor = true;
			// 
			// groupBox_particledata_cone
			// 
			this->groupBox_particledata_cone->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_particledata_cone->Controls->Add(this->vector3Edit_particledata_cone_acceleration);
			this->groupBox_particledata_cone->Controls->Add(this->label_particledata_cone_acceleration);
			this->groupBox_particledata_cone->Controls->Add(this->floatEdit_particledata_cone_speed_min);
			this->groupBox_particledata_cone->Controls->Add(this->label_particledata_cone_speed_max);
			this->groupBox_particledata_cone->Controls->Add(this->label_particledata_cone_speed_min);
			this->groupBox_particledata_cone->Controls->Add(this->floatEdit_particledata_cone_speed_max);
			this->groupBox_particledata_cone->Controls->Add(this->floatEdit_particledata_cone_coneangle);
			this->groupBox_particledata_cone->Controls->Add(this->label_particledata_cone_coneangle);
			this->groupBox_particledata_cone->Controls->Add(this->label_particledata_cone_speed);
			this->groupBox_particledata_cone->Location = System::Drawing::Point(12, 8);
			this->groupBox_particledata_cone->Name = L"groupBox_particledata_cone";
			this->groupBox_particledata_cone->Size = System::Drawing::Size(426, 128);
			this->groupBox_particledata_cone->TabIndex = 4;
			this->groupBox_particledata_cone->TabStop = false;
			this->groupBox_particledata_cone->Text = L"Cone Particle Data";
			// 
			// vector3Edit_particledata_cone_acceleration
			// 
			this->vector3Edit_particledata_cone_acceleration->Location = System::Drawing::Point(112, 88);
			this->vector3Edit_particledata_cone_acceleration->Name = L"vector3Edit_particledata_cone_acceleration";
			this->vector3Edit_particledata_cone_acceleration->Precision = static_cast<System::Int16>(2);
			this->vector3Edit_particledata_cone_acceleration->Size = System::Drawing::Size(288, 24);
			this->vector3Edit_particledata_cone_acceleration->TabIndex = 3;
			this->vector3Edit_particledata_cone_acceleration->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::vector3Edit_particledata_cone_acceleration_ValueChanged);
			// 
			// label_particledata_cone_acceleration
			// 
			this->label_particledata_cone_acceleration->Location = System::Drawing::Point(16, 93);
			this->label_particledata_cone_acceleration->Name = L"label_particledata_cone_acceleration";
			this->label_particledata_cone_acceleration->Size = System::Drawing::Size(88, 14);
			this->label_particledata_cone_acceleration->TabIndex = 9;
			this->label_particledata_cone_acceleration->Text = L"Acceleration";
			this->label_particledata_cone_acceleration->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_cone_speed_min
			// 
			this->floatEdit_particledata_cone_speed_min->Location = System::Drawing::Point(112, 56);
			this->floatEdit_particledata_cone_speed_min->Name = L"floatEdit_particledata_cone_speed_min";
			this->floatEdit_particledata_cone_speed_min->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_cone_speed_min->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_cone_speed_min->TabIndex = 1;
			this->floatEdit_particledata_cone_speed_min->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_cone_speed_min_ValueChanged);
			// 
			// label_particledata_cone_speed_max
			// 
			this->label_particledata_cone_speed_max->Location = System::Drawing::Point(336, 56);
			this->label_particledata_cone_speed_max->Name = L"label_particledata_cone_speed_max";
			this->label_particledata_cone_speed_max->Size = System::Drawing::Size(40, 16);
			this->label_particledata_cone_speed_max->TabIndex = 6;
			this->label_particledata_cone_speed_max->Text = L"max";
			this->label_particledata_cone_speed_max->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_cone_speed_min
			// 
			this->label_particledata_cone_speed_min->Location = System::Drawing::Point(200, 56);
			this->label_particledata_cone_speed_min->Name = L"label_particledata_cone_speed_min";
			this->label_particledata_cone_speed_min->Size = System::Drawing::Size(40, 16);
			this->label_particledata_cone_speed_min->TabIndex = 5;
			this->label_particledata_cone_speed_min->Text = L"min";
			this->label_particledata_cone_speed_min->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_cone_speed_max
			// 
			this->floatEdit_particledata_cone_speed_max->Location = System::Drawing::Point(248, 56);
			this->floatEdit_particledata_cone_speed_max->Name = L"floatEdit_particledata_cone_speed_max";
			this->floatEdit_particledata_cone_speed_max->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_cone_speed_max->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_cone_speed_max->TabIndex = 2;
			this->floatEdit_particledata_cone_speed_max->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_cone_speed_max_ValueChanged);
			// 
			// floatEdit_particledata_cone_coneangle
			// 
			this->floatEdit_particledata_cone_coneangle->Location = System::Drawing::Point(112, 24);
			this->floatEdit_particledata_cone_coneangle->Name = L"floatEdit_particledata_cone_coneangle";
			this->floatEdit_particledata_cone_coneangle->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_cone_coneangle->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_cone_coneangle->TabIndex = 0;
			this->floatEdit_particledata_cone_coneangle->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_cone_coneangle_ValueChanged);
			// 
			// label_particledata_cone_coneangle
			// 
			this->label_particledata_cone_coneangle->Location = System::Drawing::Point(16, 29);
			this->label_particledata_cone_coneangle->Name = L"label_particledata_cone_coneangle";
			this->label_particledata_cone_coneangle->Size = System::Drawing::Size(88, 14);
			this->label_particledata_cone_coneangle->TabIndex = 3;
			this->label_particledata_cone_coneangle->Text = L"Cone Angle";
			this->label_particledata_cone_coneangle->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_cone_speed
			// 
			this->label_particledata_cone_speed->Location = System::Drawing::Point(16, 56);
			this->label_particledata_cone_speed->Name = L"label_particledata_cone_speed";
			this->label_particledata_cone_speed->Size = System::Drawing::Size(88, 14);
			this->label_particledata_cone_speed->TabIndex = 1;
			this->label_particledata_cone_speed->Text = L"Speed";
			this->label_particledata_cone_speed->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tabPage_particletype_spiral
			// 
			this->tabPage_particletype_spiral->Controls->Add(this->groupBox_particledata_spiral);
			this->tabPage_particletype_spiral->Location = System::Drawing::Point(4, 22);
			this->tabPage_particletype_spiral->Name = L"tabPage_particletype_spiral";
			this->tabPage_particletype_spiral->Size = System::Drawing::Size(450, 620);
			this->tabPage_particletype_spiral->TabIndex = 5;
			this->tabPage_particletype_spiral->Text = L"Spiral";
			this->tabPage_particletype_spiral->UseVisualStyleBackColor = true;
			// 
			// groupBox_particledata_spiral
			// 
			this->groupBox_particledata_spiral->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox_particledata_spiral->Controls->Add(this->floatEdit_particledata_spiral_radiusscalerate);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_radiusscalerate);
			this->groupBox_particledata_spiral->Controls->Add(this->floatEdit_particledata_spiral_rotationangular_min);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_rotationangular_max);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_rotationangular_min);
			this->groupBox_particledata_spiral->Controls->Add(this->floatEdit_particledata_spiral_rotationangular_max);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_rotationangular);
			this->groupBox_particledata_spiral->Controls->Add(this->floatEdit_particledata_spiral_rotationangle_min);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_rotationangle_max);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_rotationangle_min);
			this->groupBox_particledata_spiral->Controls->Add(this->floatEdit_particledata_spiral_rotationangle_max);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_rotationangle);
			this->groupBox_particledata_spiral->Controls->Add(this->vector3Edit_particledata_spiral_acceleration);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_acceleration);
			this->groupBox_particledata_spiral->Controls->Add(this->vector3Edit_particledata_spiral_emitdirection);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_emitdirection);
			this->groupBox_particledata_spiral->Controls->Add(this->floatEdit_particledata_spiral_speed_min);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_speed_max);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_speed_min);
			this->groupBox_particledata_spiral->Controls->Add(this->floatEdit_particledata_spiral_speed_max);
			this->groupBox_particledata_spiral->Controls->Add(this->floatEdit_particledata_spiral_rotationradius);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_rotationradius);
			this->groupBox_particledata_spiral->Controls->Add(this->label_particledata_spiral_speed);
			this->groupBox_particledata_spiral->Location = System::Drawing::Point(16, 8);
			this->groupBox_particledata_spiral->Name = L"groupBox_particledata_spiral";
			this->groupBox_particledata_spiral->Size = System::Drawing::Size(418, 208);
			this->groupBox_particledata_spiral->TabIndex = 5;
			this->groupBox_particledata_spiral->TabStop = false;
			this->groupBox_particledata_spiral->Text = L"Spiral Particle Data";
			// 
			// floatEdit_particledata_spiral_radiusscalerate
			// 
			this->floatEdit_particledata_spiral_radiusscalerate->Location = System::Drawing::Point(112, 176);
			this->floatEdit_particledata_spiral_radiusscalerate->Name = L"floatEdit_particledata_spiral_radiusscalerate";
			this->floatEdit_particledata_spiral_radiusscalerate->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_spiral_radiusscalerate->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_spiral_radiusscalerate->TabIndex = 9;
			this->floatEdit_particledata_spiral_radiusscalerate->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_spiral_radiusscalerate_ValueChanged);
			// 
			// label_particledata_spiral_radiusscalerate
			// 
			this->label_particledata_spiral_radiusscalerate->Location = System::Drawing::Point(16, 181);
			this->label_particledata_spiral_radiusscalerate->Name = L"label_particledata_spiral_radiusscalerate";
			this->label_particledata_spiral_radiusscalerate->Size = System::Drawing::Size(88, 14);
			this->label_particledata_spiral_radiusscalerate->TabIndex = 24;
			this->label_particledata_spiral_radiusscalerate->Text = L"Rad. Scale Rate";
			this->label_particledata_spiral_radiusscalerate->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_spiral_rotationangular_min
			// 
			this->floatEdit_particledata_spiral_rotationangular_min->Location = System::Drawing::Point(112, 128);
			this->floatEdit_particledata_spiral_rotationangular_min->Name = L"floatEdit_particledata_spiral_rotationangular_min";
			this->floatEdit_particledata_spiral_rotationangular_min->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_spiral_rotationangular_min->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_spiral_rotationangular_min->TabIndex = 6;
			this->floatEdit_particledata_spiral_rotationangular_min->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_spiral_rotationangular_min_ValueChanged);
			// 
			// label_particledata_spiral_rotationangular_max
			// 
			this->label_particledata_spiral_rotationangular_max->Location = System::Drawing::Point(336, 132);
			this->label_particledata_spiral_rotationangular_max->Name = L"label_particledata_spiral_rotationangular_max";
			this->label_particledata_spiral_rotationangular_max->Size = System::Drawing::Size(40, 16);
			this->label_particledata_spiral_rotationangular_max->TabIndex = 21;
			this->label_particledata_spiral_rotationangular_max->Text = L"max";
			this->label_particledata_spiral_rotationangular_max->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_spiral_rotationangular_min
			// 
			this->label_particledata_spiral_rotationangular_min->Location = System::Drawing::Point(200, 132);
			this->label_particledata_spiral_rotationangular_min->Name = L"label_particledata_spiral_rotationangular_min";
			this->label_particledata_spiral_rotationangular_min->Size = System::Drawing::Size(40, 16);
			this->label_particledata_spiral_rotationangular_min->TabIndex = 20;
			this->label_particledata_spiral_rotationangular_min->Text = L"min";
			this->label_particledata_spiral_rotationangular_min->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_spiral_rotationangular_max
			// 
			this->floatEdit_particledata_spiral_rotationangular_max->Location = System::Drawing::Point(248, 128);
			this->floatEdit_particledata_spiral_rotationangular_max->Name = L"floatEdit_particledata_spiral_rotationangular_max";
			this->floatEdit_particledata_spiral_rotationangular_max->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_spiral_rotationangular_max->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_spiral_rotationangular_max->TabIndex = 7;
			this->floatEdit_particledata_spiral_rotationangular_max->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_spiral_rotationangular_max_ValueChanged);
			// 
			// label_particledata_spiral_rotationangular
			// 
			this->label_particledata_spiral_rotationangular->Location = System::Drawing::Point(16, 133);
			this->label_particledata_spiral_rotationangular->Name = L"label_particledata_spiral_rotationangular";
			this->label_particledata_spiral_rotationangular->Size = System::Drawing::Size(88, 14);
			this->label_particledata_spiral_rotationangular->TabIndex = 18;
			this->label_particledata_spiral_rotationangular->Text = L"Rot. Angular";
			this->label_particledata_spiral_rotationangular->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_spiral_rotationangle_min
			// 
			this->floatEdit_particledata_spiral_rotationangle_min->Location = System::Drawing::Point(112, 104);
			this->floatEdit_particledata_spiral_rotationangle_min->Name = L"floatEdit_particledata_spiral_rotationangle_min";
			this->floatEdit_particledata_spiral_rotationangle_min->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_spiral_rotationangle_min->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_spiral_rotationangle_min->TabIndex = 4;
			this->floatEdit_particledata_spiral_rotationangle_min->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_spiral_rotationangle_min_ValueChanged);
			// 
			// label_particledata_spiral_rotationangle_max
			// 
			this->label_particledata_spiral_rotationangle_max->Location = System::Drawing::Point(336, 108);
			this->label_particledata_spiral_rotationangle_max->Name = L"label_particledata_spiral_rotationangle_max";
			this->label_particledata_spiral_rotationangle_max->Size = System::Drawing::Size(40, 16);
			this->label_particledata_spiral_rotationangle_max->TabIndex = 16;
			this->label_particledata_spiral_rotationangle_max->Text = L"max";
			this->label_particledata_spiral_rotationangle_max->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_spiral_rotationangle_min
			// 
			this->label_particledata_spiral_rotationangle_min->Location = System::Drawing::Point(200, 108);
			this->label_particledata_spiral_rotationangle_min->Name = L"label_particledata_spiral_rotationangle_min";
			this->label_particledata_spiral_rotationangle_min->Size = System::Drawing::Size(40, 16);
			this->label_particledata_spiral_rotationangle_min->TabIndex = 15;
			this->label_particledata_spiral_rotationangle_min->Text = L"min";
			this->label_particledata_spiral_rotationangle_min->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_spiral_rotationangle_max
			// 
			this->floatEdit_particledata_spiral_rotationangle_max->Location = System::Drawing::Point(248, 104);
			this->floatEdit_particledata_spiral_rotationangle_max->Name = L"floatEdit_particledata_spiral_rotationangle_max";
			this->floatEdit_particledata_spiral_rotationangle_max->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_spiral_rotationangle_max->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_spiral_rotationangle_max->TabIndex = 5;
			this->floatEdit_particledata_spiral_rotationangle_max->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_spiral_rotationangle_max_ValueChanged);
			// 
			// label_particledata_spiral_rotationangle
			// 
			this->label_particledata_spiral_rotationangle->Location = System::Drawing::Point(16, 109);
			this->label_particledata_spiral_rotationangle->Name = L"label_particledata_spiral_rotationangle";
			this->label_particledata_spiral_rotationangle->Size = System::Drawing::Size(88, 14);
			this->label_particledata_spiral_rotationangle->TabIndex = 13;
			this->label_particledata_spiral_rotationangle->Text = L"Rotation Angle";
			this->label_particledata_spiral_rotationangle->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// vector3Edit_particledata_spiral_acceleration
			// 
			this->vector3Edit_particledata_spiral_acceleration->Location = System::Drawing::Point(112, 72);
			this->vector3Edit_particledata_spiral_acceleration->Name = L"vector3Edit_particledata_spiral_acceleration";
			this->vector3Edit_particledata_spiral_acceleration->Precision = static_cast<System::Int16>(2);
			this->vector3Edit_particledata_spiral_acceleration->Size = System::Drawing::Size(288, 24);
			this->vector3Edit_particledata_spiral_acceleration->TabIndex = 3;
			this->vector3Edit_particledata_spiral_acceleration->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::vector3Edit_particledata_spiral_acceleration_ValueChanged);
			// 
			// label_particledata_spiral_acceleration
			// 
			this->label_particledata_spiral_acceleration->Location = System::Drawing::Point(16, 77);
			this->label_particledata_spiral_acceleration->Name = L"label_particledata_spiral_acceleration";
			this->label_particledata_spiral_acceleration->Size = System::Drawing::Size(88, 14);
			this->label_particledata_spiral_acceleration->TabIndex = 11;
			this->label_particledata_spiral_acceleration->Text = L"Acceleration";
			this->label_particledata_spiral_acceleration->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// vector3Edit_particledata_spiral_emitdirection
			// 
			this->vector3Edit_particledata_spiral_emitdirection->Location = System::Drawing::Point(112, 48);
			this->vector3Edit_particledata_spiral_emitdirection->Name = L"vector3Edit_particledata_spiral_emitdirection";
			this->vector3Edit_particledata_spiral_emitdirection->Precision = static_cast<System::Int16>(2);
			this->vector3Edit_particledata_spiral_emitdirection->Size = System::Drawing::Size(288, 24);
			this->vector3Edit_particledata_spiral_emitdirection->TabIndex = 2;
			this->vector3Edit_particledata_spiral_emitdirection->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::vector3Edit_particledata_spiral_emitdirection_ValueChanged);
			// 
			// label_particledata_spiral_emitdirection
			// 
			this->label_particledata_spiral_emitdirection->Location = System::Drawing::Point(16, 53);
			this->label_particledata_spiral_emitdirection->Name = L"label_particledata_spiral_emitdirection";
			this->label_particledata_spiral_emitdirection->Size = System::Drawing::Size(88, 14);
			this->label_particledata_spiral_emitdirection->TabIndex = 9;
			this->label_particledata_spiral_emitdirection->Text = L"Emit Direction";
			this->label_particledata_spiral_emitdirection->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_spiral_speed_min
			// 
			this->floatEdit_particledata_spiral_speed_min->Location = System::Drawing::Point(112, 24);
			this->floatEdit_particledata_spiral_speed_min->Name = L"floatEdit_particledata_spiral_speed_min";
			this->floatEdit_particledata_spiral_speed_min->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_spiral_speed_min->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_spiral_speed_min->TabIndex = 0;
			this->floatEdit_particledata_spiral_speed_min->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_spiral_speed_min_ValueChanged);
			// 
			// label_particledata_spiral_speed_max
			// 
			this->label_particledata_spiral_speed_max->Location = System::Drawing::Point(336, 24);
			this->label_particledata_spiral_speed_max->Name = L"label_particledata_spiral_speed_max";
			this->label_particledata_spiral_speed_max->Size = System::Drawing::Size(40, 16);
			this->label_particledata_spiral_speed_max->TabIndex = 6;
			this->label_particledata_spiral_speed_max->Text = L"max";
			this->label_particledata_spiral_speed_max->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_spiral_speed_min
			// 
			this->label_particledata_spiral_speed_min->Location = System::Drawing::Point(200, 24);
			this->label_particledata_spiral_speed_min->Name = L"label_particledata_spiral_speed_min";
			this->label_particledata_spiral_speed_min->Size = System::Drawing::Size(40, 16);
			this->label_particledata_spiral_speed_min->TabIndex = 5;
			this->label_particledata_spiral_speed_min->Text = L"min";
			this->label_particledata_spiral_speed_min->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// floatEdit_particledata_spiral_speed_max
			// 
			this->floatEdit_particledata_spiral_speed_max->Location = System::Drawing::Point(248, 24);
			this->floatEdit_particledata_spiral_speed_max->Name = L"floatEdit_particledata_spiral_speed_max";
			this->floatEdit_particledata_spiral_speed_max->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_spiral_speed_max->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_spiral_speed_max->TabIndex = 1;
			this->floatEdit_particledata_spiral_speed_max->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_spiral_speed_max_ValueChanged);
			// 
			// floatEdit_particledata_spiral_rotationradius
			// 
			this->floatEdit_particledata_spiral_rotationradius->Location = System::Drawing::Point(112, 152);
			this->floatEdit_particledata_spiral_rotationradius->Name = L"floatEdit_particledata_spiral_rotationradius";
			this->floatEdit_particledata_spiral_rotationradius->Precision = static_cast<System::Int16>(2);
			this->floatEdit_particledata_spiral_rotationradius->Size = System::Drawing::Size(80, 24);
			this->floatEdit_particledata_spiral_rotationradius->TabIndex = 8;
			this->floatEdit_particledata_spiral_rotationradius->ValueChanged += gcnew System::EventHandler(this, &ParticleDialog::floatEdit_particledata_spiral_rotationradius_ValueChanged);
			// 
			// label_particledata_spiral_rotationradius
			// 
			this->label_particledata_spiral_rotationradius->Location = System::Drawing::Point(16, 157);
			this->label_particledata_spiral_rotationradius->Name = L"label_particledata_spiral_rotationradius";
			this->label_particledata_spiral_rotationradius->Size = System::Drawing::Size(88, 14);
			this->label_particledata_spiral_rotationradius->TabIndex = 3;
			this->label_particledata_spiral_rotationradius->Text = L"Rotation Radius";
			this->label_particledata_spiral_rotationradius->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_particledata_spiral_speed
			// 
			this->label_particledata_spiral_speed->Location = System::Drawing::Point(16, 29);
			this->label_particledata_spiral_speed->Name = L"label_particledata_spiral_speed";
			this->label_particledata_spiral_speed->Size = System::Drawing::Size(88, 14);
			this->label_particledata_spiral_speed->TabIndex = 1;
			this->label_particledata_spiral_speed->Text = L"Speed";
			this->label_particledata_spiral_speed->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// button_save
			// 
			this->button_save->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_save->Location = System::Drawing::Point(72, 662);
			this->button_save->Name = L"button_save";
			this->button_save->Size = System::Drawing::Size(75, 24);
			this->button_save->TabIndex = 1;
			this->button_save->Text = L"Save";
			this->button_save->Click += gcnew System::EventHandler(this, &ParticleDialog::button_save_Click);
			// 
			// button_saveas
			// 
			this->button_saveas->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_saveas->Location = System::Drawing::Point(192, 662);
			this->button_saveas->Name = L"button_saveas";
			this->button_saveas->Size = System::Drawing::Size(75, 24);
			this->button_saveas->TabIndex = 2;
			this->button_saveas->Text = L"Save As";
			this->button_saveas->Click += gcnew System::EventHandler(this, &ParticleDialog::button_saveas_Click);
			// 
			// button_open
			// 
			this->button_open->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_open->Location = System::Drawing::Point(312, 662);
			this->button_open->Name = L"button_open";
			this->button_open->Size = System::Drawing::Size(75, 24);
			this->button_open->TabIndex = 3;
			this->button_open->Text = L"Open";
			this->button_open->Click += gcnew System::EventHandler(this, &ParticleDialog::button_open_Click);
			// 
			// ParticleDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(474, 726);
			this->ControlBox = false;
			this->Controls->Add(this->button_open);
			this->Controls->Add(this->button_saveas);
			this->Controls->Add(this->button_save);
			this->Controls->Add(this->tabControl_particledata);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->MinimumSize = System::Drawing::Size(464, 734);
			this->Name = L"ParticleDialog";
			this->Text = L"Particle Data";
			this->tabControl_particledata->ResumeLayout(false);
			this->tabPage_generator->ResumeLayout(false);
			this->groupBox_simulation->ResumeLayout(false);
			this->groupBox_particledata_generic_particles->ResumeLayout(false);
			this->groupBox_emitter->ResumeLayout(false);
			this->tabPage_particlestarttexture->ResumeLayout(false);
			this->groupBox_texture_info->ResumeLayout(false);
			this->groupBox_texture_info->PerformLayout();
			this->groupBox_texture_alphalevels->ResumeLayout(false);
			this->groupBox_texture_scaling->ResumeLayout(false);
			this->groupBox_texture->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_texture_framerate))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_texture_frames))->EndInit();
			this->groupBox_texture_alphablending->ResumeLayout(false);
			this->groupBox_texture_UVA->ResumeLayout(false);
			this->tabPage_streaking->ResumeLayout(false);
			this->groupBox_streaking->ResumeLayout(false);
			this->groupBox_streaking->PerformLayout();
			this->tabPage_particletype_generic->ResumeLayout(false);
			this->groupBox_assettype->ResumeLayout(false);
			this->groupBox_geometry->ResumeLayout(false);
			this->groupBox_particledata_generic_particle->ResumeLayout(false);
			this->tabPage_particletype_cone->ResumeLayout(false);
			this->groupBox_particledata_cone->ResumeLayout(false);
			this->tabPage_particletype_spiral->ResumeLayout(false);
			this->groupBox_particledata_spiral->ResumeLayout(false);
			this->ResumeLayout(false);

		}



//
private: System::Void button_save_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_saveas_Click(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void button_open_Click(System::Object ^  sender, System::EventArgs ^  e);
//
private: System::Void radioButton_assettype_1texture_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void radioButton_assettype_geometry_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);
//
private: System::Void fileChooser_texture_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void radioButton_texture_alphablending_add_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void radioButton_texture_alphablending_multiply_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void rangedFloat_texture_alphalevel_middlepercentstart_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void rangedFloat_texture_alphalevel_end_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void rangedFloat_texture_alphalevel_middle_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void rangedFloat_texture_alphalevel_start_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void radioButton_texture_scaling_linear_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void radioButton_texture_scaling_exponential_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void radioButton_texture_UVA_Lifetime_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void radioButton_texture_UVA_random_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void radioButton_texture_UVA_TUV_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
//
private: System::Void fileChooser_geometry_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
//
private: System::Void floatEdit_emitterscale_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
//
private: System::Void floatEdit_particledata_generic_particlelifetime_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_particlelifetime_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_particlerate_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_maxparticles_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_particlescale_start_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_particlescalecoefficient_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_particlestartangle_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_particlestartangle_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_particleangularvelocity_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_particleangularvelocity_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_particleangularacceleration_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_particleangularacceleration_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_generic_presimtime_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
//
private: System::Void floatEdit_particledata_cone_coneangle_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_cone_speed_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_cone_speed_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void vector3Edit_particledata_cone_acceleration_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
//
private: System::Void floatEdit_particledata_spiral_speed_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_spiral_speed_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void vector3Edit_particledata_spiral_emitdirection_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void vector3Edit_particledata_spiral_acceleration_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_spiral_rotationangle_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_spiral_rotationangle_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_spiral_rotationangular_min_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_spiral_rotationangular_max_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_spiral_rotationradius_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_particledata_spiral_radiusscalerate_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
//
private: System::Void checkedListBox_generatortype_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void checkedListBox_emittertype_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void rangedFloat_texture_alphalevel_middlepercentend_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);

private: System::Void floatEdit_texture_starttime_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void floatEdit_texture_endtime_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void numericUpDown_texture_frames_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void numericUpDown_texture_framerate_ValueChanged(System::Object ^  sender, System::EventArgs ^  e);
private: System::Void checkBox_texture_looping_CheckedChanged(System::Object ^  sender, System::EventArgs ^  e);

private: System::Void checkBox_streaking_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rangedFloat_streak_length_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rangedFloat_streak_taper_ValueChanged(System::Object^  sender, System::EventArgs^  e);
private: System::Void rangedFloat_streak_fade_ValueChanged(System::Object^  sender, System::EventArgs^  e);

private:
		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		void SetupControls()
		{
			if (m_bOurChange) return;

			m_bDisableNotify = true;

			//colorAmbient->Color = tmaManagedConversionUtil::SetColorRGBA(i_Data.GetAmbient());

			m_bDisableNotify = false;
		}

private:
	//
	//	convenience functions for updating the particle generator
	//

	//----------------------------------------------------------------------------
	//	update the particle generator alpha profile
	//----------------------------------------------------------------------------
	void Update_ParticleGenerator_AlphaProfile();

	//----------------------------------------------------------------------------
	//	replace the particle generator
	//----------------------------------------------------------------------------
	void Replace_ParticleGenerator();

	//----------------------------------------------------------------------------
	//	replace the particle generator no matter what.
	//----------------------------------------------------------------------------
	void ForceReplace_ParticleGenerator();

private:
	//
	//	updating tab data functions
	//

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_TabPage_Generator( const prtParticleGeneratorTemplate * i_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_TabPage_Streaking( const prtParticleGeneratorTemplate * i_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_TabPage_Generator_TabControl( const prtParticleGeneratorTemplate * i_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_TabPage_ParticleTexture( const prtParticleGeneratorTemplate * i_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_TabPage_ParticleTypeGeneric( const prtParticleGeneratorTemplate * i_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_TabPage_ParticleTypeCone( const prtParticleGeneratorTemplate * i_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_TabPage_ParticleTypeSpiral( const prtParticleGeneratorTemplate * i_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_Controls( const prtParticleGeneratorTemplate * i_pPGTemplate );

private:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_ParticleTemplate_From_TabPage_Generator( prtParticleGeneratorTemplate * io_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_ParticleTemplate_From_TabPage_Streaking( prtParticleGeneratorTemplate * io_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_ParticleTemplate_From_TabPage_ParticleTexture( prtParticleGeneratorTemplate * io_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_ParticleTemplate_From_TabPage_ParticleTypeGeneric( prtParticleGeneratorTemplate * io_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_ParticleTemplate_From_TabPage_ParticleTypeCone( prtParticleGeneratorTemplate * io_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_ParticleTemplate_From_TabPage_ParticleTypeSpiral( prtParticleGeneratorTemplate * io_pPGTemplate );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Update_ParticleTemplate( prtParticleGeneratorTemplate * io_pPGTemplate );

private:
	//----------------------------------------------------------------------------
	//	set-up the tab pages with default values for each type of particle type
	//----------------------------------------------------------------------------
	void Initialize_TabPages();

	//----------------------------------------------------------------------------
	//	output the data (for debug only)
	//----------------------------------------------------------------------------
	void Output_Data();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void checkedlistbox_keep_one_checked(CheckedListBox^ i_pCheckedListBox, int i_Index);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void update_controls_texturepath_changed();

};
}
