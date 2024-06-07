#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"
#include "multipass_structure/multipass_structure.hpp"
#include <limits>


// This definitions allow to use the structures: mesh, mesh_drawable, etc. without mentionning explicitly cgp::
using cgp::mesh;
using cgp::mesh_drawable;
using cgp::vec3;
using cgp::numarray;
using cgp::timer_basic;

// Variables associated to the GUI (buttons, etc)
struct gui_parameters {
	bool display_frame = false;

	// Ajout de la nouvelle variable display_wireframe ici
	bool display_wireframe = false;

	bool vue_haut = false;
	bool overview = false;

};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit_euler camera_control;
	camera_controller_orbit camera_control1;
	camera_controller_orbit camera_control2;
	camera_projection_perspective camera_projection;
	window_structure window;
	vec3 light1 = { 0.0f, - 8.0f, 12.0f };
	vec3 light2 = { 0.0f,0.0f,7.0f };
	
	float w = 0.003f;
	float delta_d; // le d�placement �lementaire de l'avion quand on fait varier son altitude
	float d_max; //limiter l'altitude....
	float d_min = 1.004f; // ...  de l'avion
	float delta_alpha = 0.02f; // angle de rotation �l�mentaire quand on fait varier
	float c_alpha = 0.0f; //permet de limiter la rotation totale de l'avion lorsque son altitude augmente
	float maxFloat = std::numeric_limits<float>::max();

	camera_controller_orbit_euler lightcamera;
	camera_projection_orthographic light_projection;


	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

    cgp::skybox_drawable skybox;

	timer_basic timer;
	mesh_drawable sun;
	mesh_drawable terre;
	trajectory_drawable trajectory;
	trajectory_drawable trajectory1;
	trajectory_drawable trajectory2;


	multipass_structure multi;
	opengl_shader_structure shader_quad;
	opengl_shader_structure shadow;
	opengl_shader_structure shader;
	opengl_shader_structure shader_terre;
	std::vector<mesh_drawable> shapes;

	picking_structure pick;

	vec3 p = { 0,0,1.1f };
	vec3 vecRot = { -1,0,0 };
	vec3 vecHaut = { 0,0,1 };
	vec3 vecDir;




	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop

	vec2 raySphere(vec3 sphereCenter, float radius, vec3 rayOrigin, vec3 rayDir);


	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

	void display_info();

};



