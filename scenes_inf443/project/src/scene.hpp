#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"
#include "multipass_structure/multipass_structure.hpp"

// This definitions allow to use the structures: mesh, mesh_drawable, etc. without mentionning explicitly cgp::
using cgp::mesh;
using cgp::mesh_drawable;
using cgp::vec3;
using cgp::numarray;
using cgp::timer_basic;

// Variables associated to the GUI (buttons, etc)
struct gui_parameters {
	bool display_frame = true;

	// Ajout de la nouvelle variable display_wireframe ici
	bool display_wireframe = false;

	bool rotate_terre = false;
	bool vue_haut = false;
	

	float frequency = 5;

};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit camera_control;
	camera_controller_orbit camera_control2;
	camera_projection_perspective camera_projection;
	window_structure window;
	vec3 light2 = { 0.0f,0.0f,100.0f };
	float w;
	float delta_d; // le déplacement élementaire de l'avion quand on fait varier son altitude
	float d_max;
	float delta_alpha = 0.01f; // angle de rotation élémentaire quand on fait varier
	float c_alpha = 0.0f; //permet de limiter la rotation totale de l'avion lorsque son altitude augmente
	float c_d = 0.0f; //permet de limiter l'altitude de l'avion

	camera_controller_orbit_euler lightcamera;
	camera_projection_orthographic light_projection;


	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	timer_basic timer;
	mesh_drawable ground;
	mesh_drawable cube;
	mesh_drawable sphere;
	mesh_drawable camel;
	mesh_drawable terre;
	mesh_drawable cube1;

	multipass_structure multi;
	opengl_shader_structure shader_custom;
	opengl_shader_structure quadshader;
	opengl_shader_structure shadow;
	opengl_shader_structure shader;
	opengl_shader_structure shader_terre;
	std::vector<mesh_drawable> shapes;

	picking_structure pick;
	vec3 current_or = { 0,1,0 };





	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop


	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

	void display_info();

};



