#include "scene.hpp"


using namespace cgp;

// This function is called only once at the beginning of the program
// This function can contain any complex operation that can be pre-computed once
void scene_structure::initialize()
{

	// Set the behavior of the camera and its initial position
	// ********************************************** //
	camera_control.initialize(inputs, window); 
	//camera_control.set_rotation_axis_z(); // camera rotates around z-axis
	//   look_at(camera_position, targeted_point, up_direction)
	camera_control.look_at(
		environment.light /* position of the camera in the 3D scene */,
		{ 0.0f,0.0f,0.0f } /* targeted point in 3D scene */,
		{0.0f,1.0f,0.0f} /* direction of the "up" vector */);


	camera_control2.initialize(inputs, window);
	//camera_control.set_rotation_axis_z(); // camera rotates around z-axis
	//   look_at(camera_position, targeted_point, up_direction)
	camera_control2.look_at(
		light2 /* position of the camera in the 3D scene */,
		{ 0.0f,0.0f,45.0f } /* targeted point in 3D scene */,
		{ 0.0f,1.0f,0.0f } /* direction of the "up" vector */);
	camera_projection.depth_max = 5000;
	camera_projection.field_of_view = Pi / 60;
	




	
	//set camera to lightview for depth map--------------------------------------------


	lightcamera.initialize(inputs, window);
	lightcamera.set_rotation_axis_z(); // camera rotates around z-axis
	//   look_at(camera_position, targeted_point, up_direction)
	lightcamera.look_at(
		environment.light  /* position of the camera in the 3D scene */,
		{ 0,0,0 } /* targeted point in 3D scene */,
		{ 0,1,0 } /* direction of the "up" vector */);
	environment.uniform_generic.uniform_mat4["lightview"] = lightcamera.camera_model.matrix_view();
	environment.uniform_generic.uniform_mat4["lightprojectionMatrix"] = light_projection.matrix();
	/*std::cout << str_pretty(light_projection.matrix()) << std::endl;
	std::cout << str_pretty(camera_projection.matrix()) << std::endl;*/

	//-----------------------------------------------------------------------------


	// General information
	display_info();

	// Create the global (x,y,z) frame
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());


	// Create the shapes seen in the 3D scene
	// ********************************************** //

	mesh sphere_terre = mesh_primitive_sphere(1.0f, {0,0,0}, 700, 700);

	terre.initialize_data_on_gpu(sphere_terre);
	
	terre.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/Placeholder_Earth.jpg");
	terre.supplementary_texture["normalMap"].load_and_initialize_texture_2d_on_gpu(project::path + "assets/Normal_Map.jpg");
	terre.supplementary_texture["heightMap"].load_and_initialize_texture_2d_on_gpu(project::path + "assets/Topo_Custom3.png");

	auto struct_shape = mesh_load_file_obj_advanced(project::path + "assets/AVION-HELICE-TEST/", "AVION-HELICE-TEST.obj");
	shapes = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_shape);


	for (int k = 0; k < shapes.size(); ++k) {


		shapes[k].model.scaling = 0.0001f;
		shapes[k].model.rotation = rotation_transform::from_axis_angle({0,1,0}, Pi) * 
			rotation_transform::from_axis_angle({ 1,0,0 }, -(Pi/2 - 0.3f));
		shapes[k].model.translation = { 0,0,1.2f };
	}



	// Load a shader from a file
	
	quadshader.load(
		project::path + "shaders/quad/quad.vert.glsl",
		project::path + "shaders/quad/quad.frag.glsl");

	
	shadow.load(
		project::path + "shaders/shadow/shadow.vert.glsl",
		project::path + "shaders/shadow/shadow.frag.glsl");

	shader.load(
		project::path + "shaders/mesh/mesh.vert.glsl",
		project::path + "shaders/mesh/mesh.frag.glsl");

	shader_terre.load(
		project::path + "shaders/mesh/mesh1.vert.glsl",
		project::path + "shaders/mesh/mesh1.frag.glsl");

	terre.shader = shader_terre;

	

	std::cout << "End function scene_structure::initialize()" << std::endl;

}


// This function is called permanently at every new frame
// Note that you should avoid having costly computation and large allocation defined there. This function is mostly used to call the draw() functions on pre-existing data.
void scene_structure::display_frame()
{
	glEnable(GL_CULL_FACE);

	if (gui.vue_haut) {

		environment.light = camera_control2.camera_model.position();
		environment.camera_view = camera_control2.camera_model.matrix_view();
		w = 0.0001f;
		delta_d = 1.0f;
		d_max = 500.0f;
		//camera_projection.field_of_view = Pi / 18;
	}
	else {

		w = 0.00007f;
		d_max = 30.0f;
		delta_d = 0.04f;
		//camera_projection.field_of_view = Pi / 60;

		if (ImGui::Button("reset_view")) {

			camera_control.look_at(
				{ 0.0f,-27.0f,64.0f + c_d } /* position of the camera in the 3D scene */,
				{ 0.0f,1.6f,45.0f + c_d } /* targeted point in 3D scene */,
				{ 0.0f,1.0f,0.0f } /* direction of the "up" vector */);
		}
		else {

			environment.light = camera_control.camera_model.position();
			environment.camera_view = camera_control.camera_model.matrix_view();
			
		}

	}



	// Update time
	timer.update();
	environment.uniform_generic.uniform_float["time"] = timer.t;
	environment.uniform_generic.uniform_float["pi"] = Pi;
	environment.uniform_generic.uniform_float["f"] = gui.frequency;

	

	/*vec3 ps = { 0.0f, 15.0f * sin(w * (timer.t + 0.01f)), 15.0f * cos(w * (timer.t + 0.01)) };
	vec3 p = { 0.0f, 15.0f * sin(w * timer.t), 15.0f * cos(w * timer.t) };

	vec3 tang = normalize(ps - p);*/

	for (int k = 0; k < shapes.size(); ++k) {

		//shapes[k].model.rotation *= rotation_transform::from_vector_transform(tang, current_or);
		//shapes[k].model.rotation *= rotation_transform::from_axis_angle({ 1,0,0 },timer.t);
		//shapes[k].model.translation = {0.0f, 15.0f * sin(w*timer.t), 15.0f*cos(w*timer.t)};
		draw(shapes[k], environment);
	}
	

	//current_or = tang;
	
	if (gui.rotate_terre) {
		terre.model.rotation *= rotation_transform::from_axis_angle({ 1,0,0 }, w*timer.t);
	}

	draw(terre, environment);

	// Visualisation des maillages de nos differenets objets
	if (gui.display_wireframe) {

		draw_wireframe(terre, environment);
	}

	// conditional display of the global frame (set via the GUI)
	if (gui.display_frame)
	draw(global_frame, environment);

}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);

	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	ImGui::Checkbox("rotate", &gui.rotate_terre);

	ImGui::Checkbox("vue_2_haut", &gui.vue_haut);

	ImGui::SliderFloat("camel-x", &camel.model.translation.x, -2.0f, 2.0f);


	//ImGui::SliderFloat("fov", &camera_projection.field_of_view, 10.0f * Pi / 180.0f, 150.0 * Pi / 180.0f);

	ImGui::SliderFloat("frequency", &gui.frequency, -0.0f, 15.0f);
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);

	//// Current position of the mouse
	//vec2 const& p = inputs.mouse.position.current;

	//// The picking and deformation is only applied when pressing the shift key
	//if (inputs.keyboard.shift)
	//{
	//	// If the mouse is not clicked, compute a picking on the vertices of the grid
	//	if (!inputs.mouse.click.left)
	//		pick = picking_spheres(p, {environment.light}, 0.2f, camera_control.camera_model, camera_projection);

	//	// Key position translation
	//	if (inputs.mouse.click.left && pick.active)
	//	{
	//		vec3 new_position = picking_plane_orthogonal_to_camera(p, pick.position, camera_control.camera_model, camera_projection).position;
	//		
	//		sphere.model.translation = new_position;
	//		environment.light = new_position;
	//	}
	//}
	//else
	//	pick.active = false;

}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	//camera_control.idle_frame(environment.camera_view);
	

	if (inputs.keyboard.up) {

		c_alpha += delta_alpha;
		c_d += delta_d;

		if (c_alpha < Pi / 10) {
			for (int k = 0; k < shapes.size(); ++k) {

				shapes[k].model.rotation *= rotation_transform::from_axis_angle({ -1,0,0 }, delta_alpha);
			}
		}

		if (c_d < d_max) {

			for (int k = 0; k < shapes.size(); ++k) {

				shapes[k].model.translation += { 0.0f, 0.0f, delta_d };
			}

			camera_control2.camera_model.manipulator_translate_front(delta_d);

			camera_control.look_at(
				{ 0.0f,-27.0f,64.0f  + c_d } /* position of the camera in the 3D scene */,
				{ 0.0f,1.6f,45.0f + c_d } /* targeted point in 3D scene */,
				{ 0.0f,1.0f,0.0f } /* direction of the "up" vector */);

		}


	}
	if (inputs.keyboard.last_action.is_released(GLFW_KEY_UP)) {

		c_alpha = 0.0f;

		for (int k = 0; k < shapes.size(); ++k) {

			shapes[k].model.rotation = rotation_transform::from_axis_angle({ 0,1,0 }, Pi) *
				rotation_transform::from_axis_angle({ 1,0,0 }, -(Pi / 2 - 0.3f));
		}
	
	}
}

void scene_structure::display_info()
{
	std::cout << "\nCAMERA CONTROL:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << camera_control.doc_usage() << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;


	std::cout << "\nSCENE INFO:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "Introductory scene for INF443." << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;
}

