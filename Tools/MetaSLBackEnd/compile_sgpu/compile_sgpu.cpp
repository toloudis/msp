/*****************************************************************************
 * Copyright 1986-2009 by mental images GmbH, Fasanenstr. 81, D-10623 Berlin,
 * Germany. All rights reserved.
 *****************************************************************************/

#include <string>
// for Fedora 9 gcc 4.3
extern "C" {
#include <string.h>
}

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

#include "compile_sgpu.h"

bool setup_complete = false;	//!< indicates if graph library is ready to go	

/*! \mainpage mental mill® compiler sample code

	This sample code and its documentation are intended for mental mill
	integrators who want to become acquainted with how to get started
	with the mental mill compiler SDK.  It shows how to create and
	compile shaders with a program that executes three examples.
	The code is not intended to be an example of production ready software,
	but rather focuses on demonstrating the basic sequence of calls one
	might use to get started on a project.
	
	\section how_to How to use the sample

	To use the sample on Windows, open compile_sgpu.sln in the
	compile_sgpu/src directory and build the compile_sgpu project in
	either debug or release mode. The result will be an executable called
	compile_sgpu.exe in the Debug or Release folder, respectively,
	of the compile_sgpu/src directory.  To run the program, start it from
	any directory, but leave the exectuable in the directory in which it
	was created.  The sample code assumes that the mental mill Integrator
	Edition directory structure is untouched.

	When the program is run, it executes three examples, each of
	which produces one or more output files.
	These files are written into the directory from which the program
	is run, so be sure that the directory from which the program
	is run is writable.

	To use the sample on Linux, use the Makefile provided in the
	compile_sgpu/src directory, and then start the program from any
	directory, again leaving the exectuable where it was created.

	\section interfaces Main interfaces of the mental mill compiler SDK

	The mental mill libraries are based on
	the concept of interfaces.  The mental mill interface is used to
	get the two main interfaces of interest when using the compiler library.
	One is IMill_compiler, the main interface to the compiler.  The other is
	IMill_graph.  Like the mental mill GUI library, the mental mill
	compiler library contains a graph component to create nodes and perform
	related operations, and the IMill_graph interface is the main interface
	to this graph component.

	Once the main interfaces have been obtained, a graph library and graph
	node classes in it can be loaded.  These are resources from which
	graph nodes can be instantiated and used as building blocks to create
	shader graphs.

	Once a shader graph has been created, the compiler library can be used to
	compile the graph and generate shader code, which can in turn be written
	out to files.  This program shows several examples of how to do this. */

/*! There is one class in this code sample.  It is called Compiler_samples.
	Its constructor simply prints information, and initializes members. */
Compiler_samples::Compiler_samples()
{
	fprintf(stderr,
		"This program runs three examples.\n"
		"It takes 1 optional argument after the executable name,\n"
		"the path to the mental mill IE installation.\n"
		"If no argument is given, it is assumed that the mental mill\n"
		"directory structure is untouched, and the compile_sgpu program\n"
		"is run from the directory in which it was created.\n"
		"Output is written to the current directory.\n\n");
	
	mill = NULL;
	mill_compiler = NULL;
	mill_graph = NULL;
	graph_library = NULL;

	sgpu_class = NULL;

	phong_class = NULL;
	comp_phong_class = NULL;
	comp_refl_class = NULL;
	comp_refr_class = NULL;
	comp_falloff_class = NULL;

	tex_class = NULL;
	noise_class = NULL;

	norm_class = NULL;

	color_lerp_class = NULL;
	color_add_class = NULL;
	color_mult_class = NULL;

	env_class = NULL;
	point_light_class = NULL;
	spot_light_class = NULL;
}

/*! This function is a helper function of get_app_dir().  It converts
	a path to one using forward slashes. */
std::string Compiler_samples::convert_to_forward(const char *path)
{
	if(!path)
		return std::string("");
	std::string fs(path);
	size_t pos = 0;
	while(pos != std::string::npos) {
		pos = fs.find('\\', pos);
		if(pos != std::string::npos) {
			fs.replace(pos,1,"/");
		}
	}
	return fs;
}

/*! This function is a helper function of get_app_dir().  It strips away
	the trailing slash of a path. */
std::string Compiler_samples::strip_trailing(std::string &path)
{
	size_t len = path.size();
	if((path[len-1] == '/') || (path[len-1] == '\\'))
		return path.substr(0,len-1);
	else
		return path;
}

/*! This function is a helper function of get_app_dir().  It gets the
	path name from a path. */
std::string Compiler_samples::pathname(const char *path)
{
	std::string result;

	const char *forward = strrchr(path,'/');
	const char *backward = strrchr(path,'\\');
	if(backward < forward) {
		for(const char *p = path; p < forward; p++)
			result += *p;
	} else if(forward < backward) {
		for(const char *p = path; p < backward; p++)
			result += *p;
	} else {
		result = path;
	}
	return result;
}

/*! Get the directory in which the executable of this sample
    code resides. */
std::string Compiler_samples::get_app_dir()
{
	std::string app_dir;

#if defined(WIN32) || defined(WIN64)
	char app_path[_MAX_PATH];
	GetModuleFileName(NULL,app_path,_MAX_PATH);
	char drive[_MAX_DRIVE],dir[_MAX_DIR],name[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath(app_path,drive,dir,name,ext);
	app_dir = std::string(drive) + std::string(dir);
	app_dir = convert_to_forward(app_dir.c_str());
	app_dir = strip_trailing(app_dir);
#else
	pid_t pid = getpid();
	std::string ss("/proc/");
	char pidstr[32];
	sprintf(pidstr,"%d",pid);
	ss += pidstr;
	ss += "/exe";
	char buf[2048];
	int len = readlink(ss.c_str(),buf,2048);
	if(len == -1) {
		static const int max_path_length = 2048;
		char *cwd = new char[max_path_length];
		cwd = ::getcwd(cwd,max_path_length);
		app_dir = cwd;
		delete [] cwd;
	} else {
		// readlink doesn't append a null..
		buf[len] = '\0';
		app_dir = pathname(buf);
		app_dir = strip_trailing(app_dir);
	}
#endif

	return app_dir;
}

/*! The init function initializes mental mill and then uses the
   mental mill interface returned to set the paths where mental mill
   will look for the implemented code of shader classes, textures and
   the graph library used in the program.  The mental mill interface
   is also used to get the two main interfaces used in this code,
   namely those to the compiler and graph components.  A graph library
   and classes from it are loaded using the graph interface.  After
   this initializer is called, everything is set to begin building
   shader graphs which use the loaded node classes. */
bool Compiler_samples::init()
{
	fprintf(stderr, "Loading library.\n");

	// get the mental mill interface
	mill = initialize_mill(); 
	if(!mill) {
		fprintf(stderr, "Unable to initialize mental mill.\n");
		return false;
	}

	// set paths where mental mill will look for .xmsl and .msl files
#if defined(WIN32) || defined(WIN64)
    std::string separator = ";";
#else
    std::string separator = ":";
#endif
	//std::string data = get_app_dir() + "/../../../../data";
	std::string data = get_app_dir() + "/../../MetaSLNodeGraph/MSLData";
	
	std::string shaders = data + "/shaders" + separator +
		get_app_dir() + "/..";
	mill->set_shader_path(shaders.c_str());

	// set paths where mental mill will look for .dds files
	std::string textures = data + "/textures";
	mill->set_texture_path(textures.c_str());

	// get the main compiler interface from the mental mill interface
	mill_compiler = 
	static_cast<IMill_compiler *>(mill->get_interface(I_MILL_COMPILER));
	if(!mill_compiler) {
		fprintf(stderr, "Unable to initialize mental mill compiler.\n");
		return false;
	}

	// get the main graph interface from the mental mill interface
	mill_graph = 
	static_cast<IMill_graph *>(mill->get_interface(I_MILL_GRAPH));
	if(!mill_graph) {
		fprintf(stderr, "Unable to initialize mental mill graph interface.\n");
		return false;
	}

	// load the graph library from which shader classes will be drawn
	// NOTE: Some of the shaders in this library use the
	// mi_msl_shared.msl file, which in turn uses constants.msl, which is why
	// mi_msl_shared.msl and constants.msl are included in shader_lib.xmsl.
	ICompiler_errors * errors = mill_compiler->create_errors();
	graph_library = mill_graph->load_graph_library("shader_lib.xmsl", NULL, errors);
	if(!graph_library) {
		fprintf(stderr, "Unable to open the graph library.  Check paths.\n");
		return false;
	}
	if (errors->count() > 0)
	{
		for (int i = 0; i < errors->count(); i++)
		{
			int elen = errors->get_error_string_length(i);
			char* buf = new char[elen+1];
			errors->get_error_string(i, buf, elen);
			fprintf(stderr, buf);
			fprintf(stderr, "\n");
			delete [] buf;
		}
	}
	errors->release();

	// retrieve Phong illumination and component surface shader classes
	// from the graph library
	// NOTE: The Component_refraction shader is located in the 
	// component_reflection.msl file.
	//phong_class = graph_library->get_class("Illumination_phong");

// testing:
//	sgpu_class = graph_library->get_class("Illumination_sgpu_Anisotropic");

	phong_class = graph_library->get_class("Illumination_sgpu"); //bga - switched to a basic shader
	comp_phong_class = graph_library->get_class("Component_phong");
	comp_refl_class = graph_library->get_class("Component_reflection");
	comp_refr_class = graph_library->get_class("Component_refraction");
	comp_falloff_class = graph_library->get_class("Component_falloff");

	// retrieve 2D texture lookup shader class
	tex_class = graph_library->get_class("Texture_lookup_2d"); 

	// generator class
	noise_class = graph_library->get_class("Generator_cellular"); 

	// retrieve normal shader class to transform normals from tangent
	// space to internal space
	norm_class = graph_library->get_class("Normals_make_normal");

	// retrieve math color shader classes to perform linear algebra
	// operations on color values
	// NOTE: Math operations are created on-the-fly.  In shader_lib.xmsl,
	// including intrinsic_nodes allows access to these.
	color_lerp_class = graph_library->get_class("Math_color_lerp");
	color_add_class = graph_library->get_class("Math_color_add");
	color_mult_class = graph_library->get_class("Math_color_multiply");

	// retrieve an environment sub-shader class
	env_class = graph_library->get_class("Environment_map_cubic");
	
	// retrieve light sub-shader classes
	point_light_class = graph_library->get_class("Light_point");
	spot_light_class = graph_library->get_class("Light_spot");

	// check that everything loaded correctly
	if (!(phong_class && comp_phong_class && comp_refl_class &&
		  comp_refr_class && comp_falloff_class &&
		  tex_class && norm_class &&
		  color_lerp_class && color_add_class && color_mult_class &&
		  env_class && point_light_class && spot_light_class)) {
			fprintf(stderr, "One or more classes not loaded properly.\n");
			return false;
	}

	setup_complete = true;
	fprintf(stderr, "Library setup complete.\n");
	return true;
}

/*! The destructor cleans up interfaces retrieved in the constructor. */
Compiler_samples::~Compiler_samples()
{
	if (graph_library) {
		// destroy the graph library
		// NOTE: Destroying the graph library also destroys its contents,
		// including all of the classes in it and the node instances created
		// from those classes.  
		graph_library->destroy();

		// release the graph library interface
		// NOTE: When the graph_library interface was created, its reference
		// count was incremented to 1. Calling release on graph_library causes
		// the reference count to be decremented to 0, meaning that it is no
		// longer available as an interface.  No method can be subsequently
		// called on it, including destroy.  Therefore destroy must be
		// called on graph_library before release.
		graph_library->release(); graph_library = NULL;
	}

	// release the shader class interfaces
	if (sgpu_class) {
		sgpu_class->release(); sgpu_class = NULL;
	}
	if (phong_class) {
		phong_class->release(); phong_class = NULL;
	}
	if (comp_phong_class) {
		comp_phong_class->release(); comp_phong_class = NULL;
	}
	if (comp_refl_class) {
		comp_refl_class->release(); comp_refl_class = NULL;
	}
	if (comp_refr_class) {
		comp_refr_class->release(); comp_refr_class = NULL;
	}
	if (comp_falloff_class) {
		comp_falloff_class->release(); comp_falloff_class = NULL;
	}
	if (tex_class) {
		tex_class->release(); tex_class = NULL;
	}
	if (noise_class) {
		noise_class->release(); noise_class = NULL;
	}
	if (norm_class) {
		norm_class->release(); norm_class = NULL;
	}
	if (color_lerp_class) {
		color_lerp_class->release(); color_lerp_class = NULL;
	}
	if (color_add_class) {
		color_add_class->release(); color_add_class = NULL;
	}
	if (color_mult_class) {
		color_mult_class->release(); color_mult_class = NULL;
	}
	if (env_class) {
		env_class->release(); env_class = NULL;
	}
	if (point_light_class) {
		point_light_class->release(); point_light_class = NULL;
	}
	if (spot_light_class) {
		spot_light_class->release(); spot_light_class = NULL;
	}

	// release the interfaces to the compiler and graph components
	// NOTE: No destroy call is necessary on these interfaces.
	if(mill_compiler) {
		mill_compiler->release(); mill_compiler = NULL;
	}
	if(mill_graph) {
		mill_graph->release(); mill_graph = NULL;
	}

	// release the mental mill interface
	// NOTE: No destroy call is necessary on this interface.
	if(mill) {
		mill->release(); mill = NULL;
	}

	fprintf(stderr,"Press Return to exit.\n");
	std::getchar();
}


/*! This method generates an HLSL FX shader that implements a Phong
	illumination model with two inputs altered.  First, a 2D texture is
	used to define the diffuse color of the Phong illumination model.
	Second, the normal state parameter is overwritten with a normal texture,
	which is transformed to the internal space of the surface to produce a
	textured look on the surface.  An annotation is made to the diffuse color
	input.  A point light and cubic environment map
	are set as sub-shaders for the surface shader.  This example shows how to:
	  - Create shader nodes
	  - Edit an input parameter
	  - Add a state parameter as an input parameter
	  - Attach nodes
	  - Add a graph annotation
	  - Set a surface shader
	  - Set light and environment sub-shaders
	  - Set a shader group for compilation
	  - Compile a shader group
	  - Set HLSL FX options
	  - Generate an HLSL FX shader
	  - Write a generated HLSL FX shader to a file */
bool Compiler_samples::run_second_sample()
{
	IGraph_node *phong_illum;	// Phong surface illumination model node
	IGraph_node *diffuse_tex;	// 2D diffuse texture lookup node
	IGraph_node *noise_color;	// color from noise generator
	//IGraph_node *norm_transf;	// normal transformation node
	//IGraph_node *norm_tex;		// 2D normal lookup node
	//IGraph_node *cube_env;		// cubic environment map node
	//IGraph_node *point_light;	// point light node
	//IType *vector3_type;		// 3D vector type to be used in setting normals
	IParameter_iterator *params;// parameter iterator
	IGraph_annotations *annos;	// list of annotations
	IType *anno_type;			// type of annotation (a string)
	ICompiler_options *options;	// options for compilation of the shader graph
	ICompilation_unit *cunit;	// compilation unit of the shader graph
	IGenerated_shader *shader;	// shader generated by mental mill

	if (!setup_complete) {
		fprintf(stderr, "Cannot run second sample because initialization "
						"is not complete.\n");
		return false;
	}

	fprintf(stderr, "Running second sample.\n");

	// Create Nodes: Choose nodes to be used in the shader graph
	phong_illum = graph_library->create_node(phong_class, "phong_illum");
	diffuse_tex = graph_library->create_node(tex_class, "diffuse_tex");
	noise_color = graph_library->create_node(noise_class, "noise_color");
	//norm_transf = graph_library->create_node(norm_class, "norm_transf");
	//norm_tex  = graph_library->create_node(tex_class, "norm_tex");
	//cube_env = graph_library->create_node(env_class, "cube_env");
	//point_light = graph_library->create_node(point_light_class, "point_light");

	// Edit Input Parameters: Set which normal map file to use as
	// the texture of the norm_tex node
	//IParameter_iterator *param_it = 
	//	norm_tex->get_parameters(MI::MSDK::IGraph_node_class::INPUT);
	//param_it->to_next("texture");
	//IValue_iterator *val_it = param_it->get_value_iterator();
	//IType *val_type = val_it->get_type();
	//if (val_type->get_typecode() != MI::MSDK::IType::TYPE_TEXTURE2D) {
	//	fprintf(stderr, "Error: Expected a texture. Cannot set parameter.\n");
	//	return false;
	//}
	//val_it->set_texture2d("unfiltered_noise_normal.dds");
	//param_it->set_value(val_it);

	// Build a Shader Graph, part one: Attach the diffuse texture node
	// to the diffuse_color input of the phong_node
	phong_illum->set_input_attachment("diffuse_color", diffuse_tex, "result");
	//phong_illum->set_input_attachment("diffuse_color", noise_color, "result1");

	// Build a Shader Graph, part two: Add a normal state parameter to the
	// phong_illum node and overwrite it by the normal tranformation node
	//vector3_type = mill_graph->create_type("Vector3", IType::TYPE_VECTOR3, 1);
	//phong_illum->add_parameter(IGraph_node_class::INPUT, "normal",
	//                           vector3_type);
	//phong_illum->set_input_attachment("normal", norm_transf, "result");

	// Build a Shader Graph, part three: Attach the normal texture
	// lookup to the norm_transf node to determine its color input
	//norm_transf->set_input_attachment("color", norm_tex, "result");

	// Add an annotation, part one: Get the diffuse parameter
	params = phong_illum->get_parameters(IGraph_node_class::INPUT);
	params->to_next("diffuse_color");
	if (params->at_end())
		fprintf(stderr, "Diffuse color parameter not found\n");

	// Add an annotation, part two: Get the annotations list
	annos = params->get_annotations();

	// Add an annotation, part three: Create an instance of the type of
	// the annotation value (a string), define the annotation name
	// as "semantic", and define the annotation value as "Diffuse"
	anno_type = mill_graph->create_type("String", IType::TYPE_STRING, 1);
	const char *value = "Diffuse";    
	annos->append("semantic", anno_type, &value);
    
	// Compile the Shader Graph, part one: Create compiler options
	options = mill_compiler->create_options();

	// Compile the Shader Graph, part two: Specify the shader source and type
	options->set("shader_name", "phong_illum");
	options->set("shader_type", "surface");

	// Compile the Shader Graph, part three: Specify the sub-shaders for
	// the surface shader
	//options->set("environment", "cube_env");
	//options->set("lights", "point_light");

	// Compile the Shader Graph, part four: Get the compilation unit
	// by compiling the group specified above
	cunit = mill_compiler->compile_group(graph_library, NULL, options);
	if (!cunit) {
	    fprintf(stderr, "Failed to compile shader graph.\n");
	    return false;
	}

	// Generate an HLSL FX shader
	options->set("format", "HLSL");
	options->set("vertex_shader_profile", "vs_3_0");
	options->set("fragment_shader_profile", "ps_3_0");
	options->set("generate_samplers", "true");
	options->set("generate_umapped_annotations", "false");
	options->set("vs_binding_tangent", "TANGENT");
	options->set("vs_binding_binormal", "BINORMAL");
	options->set("flip_tex2d_v", "false");
	//shader = cunit->generate_shader("FX", options);
	shader = cunit->generate_shader("sgpu", options);	// this is our custom shader
	int count = 0;
	if(!shader) {
		fprintf(stderr, "Unable to generate an HLSL FX shader.\n");
		return false;
	} else {
		count = shader->get_source_code_count();
	}
	if (count < 1) {
		fprintf(stderr, "Error: expected one source fragment for HLSL FX.\n");
		return false;
	}

	// Write out the shader: The FX back-end generates a single source
	// code string, so the procedure is to grab the first source code
	// string, get the size and allocate a buffer to hold it.
	int size = 0;
	char *buffer = NULL;
	if(count >= 1) {
		size = shader->get_source_code_size(0);
		if (size) {
			buffer = new char[size];
			shader->get_source_code(0, buffer, size);
			FILE *file = fopen("./sgpu_sample.fx", "wt");
			if (!file) {
				fprintf(stderr, "Current directory unwritable.\n");
				return false;
			} else {
				fwrite (buffer, 1, size-1, file);
				fprintf(stderr, "File sgpu_sample.fx written in current "
								"directory.\n");
				fclose(file);
			}
			delete[] buffer;
		} else {
			fprintf(stderr, "Buffer for HLSL shader is empty.\n");
			return false;
		}
	}
	buffer = NULL;

	// clean up interfaces
	// NOTE: Graph nodes are destroyed when the graph library is (see the
	// destructor of the Compiler_samples class).  Destroy must be called
	// separately on the IType interfaces since these are not included in
	// the graph library. No destroy call is necessary on 
	// IGraph_annotations, IParameter_iterator, ICompiler_options,
	// ICompilation_unit and IGenerated_shader interfaces.
	//val_it->release(); val_it = NULL;
	//val_type->release(); val_type = NULL;
	//param_it->release(); param_it = NULL;
	phong_illum->release(); phong_illum = NULL;
	diffuse_tex->release(); diffuse_tex = NULL;
	noise_color->release(); noise_color = NULL;
	//norm_transf->release(); norm_transf = NULL;
	//norm_tex->release(); norm_tex = NULL; 
	//cube_env->release(); cube_env = NULL;
	//point_light->release(); point_light = NULL;
	//vector3_type->destroy(); vector3_type->release(); vector3_type = NULL;
	params->release(); params = NULL;
	annos->release(); annos = NULL;
	anno_type->destroy(); anno_type->release(); anno_type = NULL;
	options->release(); options = NULL;    
	cunit->release(); cunit = NULL;
	shader->release(); shader = NULL;

	fprintf(stderr, "Second sample complete.\n");
	return true;
}


/*  The main program expects the user to run it with an argument to
	indicate which of the samples to demonstrate.  Each sample is
	implemented as a method that uses the graph library and associated
	classes loaded by the constructor of Compiler_samples. */
int main(int argc,char *argv[])
{
	// construct the setup for the sample to be demonstrated
	Compiler_samples sample;

	// initialize and run the three samples
	if (sample.init()) {
		//sample.run_first_sample();
		sample.run_second_sample();
		//sample.run_third_sample();
	} else {
		fprintf(stderr, "Initialization failed.\n");
	}
	return 0;
}
