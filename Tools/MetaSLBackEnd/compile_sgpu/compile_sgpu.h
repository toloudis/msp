/*****************************************************************************
 * Copyright 1986-2009 by mental images GmbH, Fasanenstr. 81, D-10623 Berlin,
 * Germany. All rights reserved.
 *****************************************************************************/

#include "mentalmill.h"
#include <stdio.h>

using namespace MI::MSDK; // include this to allow direct references to the API

class Compiler_samples
{
public :
	Compiler_samples();
	~Compiler_samples();

	bool init();				//!< sets up mental mill and the graph library
	bool run_first_sample();	//!< runs one example of using the compiler
	bool run_second_sample();	//!< runs a second example
	bool run_third_sample();	//!< runs a third example

private :

	//! converts slashes to forward slashes
	std::string convert_to_forward(const char *path);

	//! strips the trailing slash of a path name
	std::string strip_trailing(std::string &path);

	//! gets the path name from a path
	std::string pathname(const char *path);

	//! gets the directory in which the sample exectuable resides
	std::string get_app_dir();

	IMill				*mill;				//!< main mental mill interface
	IMill_compiler		*mill_compiler;		//!< mental mill compiler interface
	IMill_graph			*mill_graph;		//!< mental mill graph interface
	IGraph_library		*graph_library;		//!< graph library interface
 
	IGraph_node_class	*sgpu_class;		//!< Phong illumination model class

	IGraph_node_class	*phong_class;		//!< Phong illumination model class
	IGraph_node_class	*comp_phong_class;	//!< Phong component class
	IGraph_node_class	*comp_refl_class;	//!< reflection component class
	IGraph_node_class	*comp_refr_class;	//!< refraction component class
	IGraph_node_class	*comp_falloff_class;//!< falloff component class

	IGraph_node_class	*tex_class;			//!< 2D texture lookup class
	IGraph_node_class	*noise_class;		//!< noise generator class

	IGraph_node_class	*norm_class;		//!< class to transform normals

	IGraph_node_class	*color_lerp_class;	//!< algebra class - linear interp
	IGraph_node_class	*color_add_class;	//!< algebra class - addition
	IGraph_node_class	*color_mult_class;	//!< algebra class - multiplication

	IGraph_node_class	*env_class;			//!< environment sub-shader class
	IGraph_node_class	*point_light_class;	//!< point light sub-shader class
	IGraph_node_class	*spot_light_class;	//!< spot light sub-shader class
};
