#include "scene.hpp"



using namespace cgp;


vec2 scene_structure::raySphere(vec3 sphereCenter, float radius, vec3 rayOrigin, vec3 rayDir) {

	// If ray origin is inside sphere, dstToSphere = 0 
	// If ray misses sphere, dstToSphere = maxValue; dstThroughSphere = 0 

	vec3 offset = rayOrigin - sphereCenter; 
	float a =1; // Set to dot(rayDir, rayDir) if rayDir might not be normalized 
	float b = 2 * dot(offset, rayDir); 
	float c = dot (offset, offset) - radius * radius; 
	float d=b*b-4*a*c; // Discriminant from quadratic formula

	// Number of intersections : 0 when d < 0; 1 when d = 0; 2 when d > 0 
	if (d > 0) {
		float s = std::sqrt(d);
		float dstToSphereNear = std::max(0.0f , (-b - s) / (2 * a));
		float dstToSphereFar = (-b + s) / (2 * a);
		//Ignore intersections that occur behind the ray 
		if (dstToSphereFar >= 0) {
			return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
		}
	}
	// Ray did not intersect sphere return float2(maxFloat, 0);
	return vec2(maxFloat, 0);
}

// This function is called only once at the beginning of the program
// This function can contain any complex operation that can be pre-computed once
void scene_structure::initialize()
{

	// Set the behavior of the camera and its initial position
	// ********************************************** //
	camera_control.initialize(inputs, window); 
	camera_control.set_rotation_axis_z(); // camera rotates around z-axis
	//   look_at(camera_position, targeted_point, up_direction)
	camera_control.look_at(
		environment.light /* position of the camera in the 3D scene */,
		{ 0.0f,0.0f,0.0f } /* targeted point in 3D scene */,
		{0.0f,1.0f,0.0f} /* direction of the "up" vector */);


	camera_control1.initialize(inputs, window);
	
	camera_control1.look_at(
		light1,
		p + vec3(0,0.09f,0.02f),
		{ 0.0f,1.0f,0.0f });

	camera_control2.initialize(inputs, window);
	camera_control2.look_at(
		light2,
		{ 0.0f,0.0f,1.1f },
		{ 0.0f,1.0f,0.0f });
	camera_projection.depth_max = 5000;
	
	
	mesh sun_mesh = mesh_primitive_sphere(0.5f);
	sun.initialize_data_on_gpu(sun_mesh);

	sun.material.color = environment.sun_color;
	environment.uniform_generic.uniform_vec3["sun_color"] = environment.sun_color;
	sun.shader.load(project::path + "shaders/single_color/single_color2.vert.glsl",
		project::path + "shaders/single_color/single_color2.frag.glsl");




	
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

	trajectory = trajectory_drawable(100);
	trajectory1 = trajectory_drawable(70);
	trajectory2 = trajectory_drawable(70);

	// Create the shapes seen in the 3D scene
	// ********************************************** //


	mesh sphere_terre = mesh_primitive_sphere(1.0f, {0,0,0}, 500, 500);

	terre.initialize_data_on_gpu(sphere_terre);
	
	terre.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/combined_image_max_quality.jpg");
	terre.supplementary_texture["heightMap"].load_and_initialize_texture_2d_on_gpu(project::path + "assets/Topo_Custom3.png");

	auto struct_shape = mesh_load_file_obj_advanced(project::path + "assets/AVION-HELICE-TEST/", "AVION-HELICE-TEST.obj");
	shapes = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_shape);


	for (int k = 0; k < shapes.size(); ++k) {


		shapes[k].model.scaling = 0.0002f;
		shapes[k].model.rotation = rotation_transform::from_axis_angle({0,1,0}, Pi) * 
			rotation_transform::from_axis_angle({ 1,0,0 }, -Pi/2);
		shapes[k].model.translation = { 0,0,1.1f };
	}



	// Load a shader from a file
	shader_quad.load(
		project::path + "shaders/quad/quad.vert.glsl",
		project::path + "shaders/quad/quad.frag.glsl"
	);

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

	environment.uniform_generic.uniform_float["pi"] = Pi;

	// Initialize multipass renderer
	/*multi.initialize();
	multi.set_shader_pass_2(shader_quad);*/
	

	std::cout << "End function scene_structure::initialize()" << std::endl;

}


// This function is called permanently at every new frame
// Note that you should avoid having costly computation and large allocation defined there. This function is mostly used to call the draw() functions on pre-existing data.
void scene_structure::display_frame()
{
	glEnable(GL_CULL_FACE);
	vecDir = rotation_transform::from_axis_angle(vecHaut, Pi / 2) * vecRot;


	if (gui.vue_haut) {

		delta_d = 0.1f; // le déplacement élementaire de l'avion quand on fait varier son altitude
		d_max = 11.1f;
		camera_control2.look_at(
			p + 7*vecHaut,
			p,
			-vecDir);

		camera_projection.field_of_view = Pi / 25;



		environment.camera_view = camera_control2.camera_model.matrix_view();
		
	}
	else if (gui.overview) {
		delta_d = 0.01f; // le déplacement élementaire de l'avion quand on fait varier son altitude
		d_max = 1.8f;
		camera_projection.field_of_view = Pi / 4;
		environment.camera_view = camera_control.camera_model.matrix_view();

		if (shapes[0].model.translation.z > d_max) {

			float d = shapes[0].model.translation.z - d_max;

			for (int k = 0; k < shapes.size(); ++k) {

				shapes[k].model.translation.z = d_max;
			}
		}

		
	}
	else{

		delta_d = 0.01f; // le déplacement élementaire de l'avion quand on fait varier son altitude
		d_max = 1.8f;

		camera_projection.field_of_view = Pi / 100;
		
		if (shapes[0].model.translation.z > d_max) {

			float d = shapes[0].model.translation.z - d_max;

			for (int k = 0; k < shapes.size(); ++k) {

				shapes[k].model.translation.z = d_max;
			}
		}

		camera_control1.look_at(
			p +8.0f*vecDir + 10.9f*vecHaut,
			p - 0.09f*vecDir + 0.02f*vecHaut,
			-vecDir);

			environment.camera_view = camera_control1.camera_model.matrix_view();

			if (ImGui::Button("cam_pos")) {
				std::cout << camera_control1.camera_model.position() << std::endl;
			}
	}



	// Update time
	timer.update();
	environment.uniform_generic.uniform_float["time"] = timer.t;
	environment.uniform_generic.uniform_vec3["light"] = environment.light;
	
	p = rotation_transform::from_axis_angle(vecRot,w*timer.scale)*p;
	vecHaut = normalize(p);

	trajectory.visual.color = { 1.0,0.5, 0.0 };
	trajectory1.visual.color = {  1.0, 0.5, 0.0 };
	trajectory2.visual.color = { 1.0,0.5,0.0 };
	trajectory.add(p + 0.025f * vecHaut);
	trajectory1.add(p + 0.045f*vecRot + 0.025f*vecHaut + 0.01f * vecDir);
	trajectory2.add(p - 0.045f * vecRot + 0.025f*vecHaut + 0.01f * vecDir);

	draw(trajectory, environment);
	draw(trajectory1, environment);
	draw(trajectory2, environment);
	

	//// ************************************** //
	//// First rendering pass
	//// ************************************* //
	//multi.update_screen_size(window.width, window.height);

	//// 1- Activate the rendering on the FBO
	//multi.start_pass_1();

	//terre.shader = shadow;
	

	for (int k = 0; k < shapes.size(); ++k) {
		
	
		shapes[k].model.rotation = rotation_transform::from_axis_angle(vecRot, w * timer.scale) * shapes[k].model.rotation;
		shapes[k].model.translation = p;

		//shapes[k].shader = shadow;
		
		draw(shapes[k], environment);
	}

	sun.model.translation = environment.light;
	
	draw(terre, environment);
	draw(sun, environment);

	// 3- Stop the rendering on the FBO
	//multi.end_pass_1();

	// ************************************** //
	// Second rendering pass
	// ************************************* //

	// Display the result on a quad taking all the size of the screen
	//  The quad is associated to a shader with a screen-based effect (ex. image gradient)
	//  The texture used by the quad is the output texture of the FBO

	/*multi.start_pass_2();
	multi.draw_pass_2(environment);

	multi.end_pass_2();*/

	// Visualisation des maillages de nos differenets objets
	if (gui.display_wireframe) {

		draw_wireframe(terre, environment);
		draw_wireframe(sun, environment);
	}


	// conditional display of the global frame (set via the GUI)
	if (gui.display_frame)
	draw(global_frame, environment);

}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);

	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	ImGui::Checkbox("vue_2_haut", &gui.vue_haut);

	ImGui::Checkbox("overview", &gui.overview);
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);

	// Current position of the mouse
	vec2 const& p = inputs.mouse.position.current;

	// The picking and deformation is only applied when pressing the shift key
	if (inputs.keyboard.shift)
	{
		// If the mouse is not clicked, compute a picking on the vertices of the grid
		if (!inputs.mouse.click.left)
			pick = picking_spheres(p, {environment.light}, 0.5f, camera_control.camera_model, camera_projection);

		// Key position translation
		if (inputs.mouse.click.left && pick.active)
		{
			vec3 new_position = picking_plane_orthogonal_to_camera(p, pick.position, camera_control.camera_model, camera_projection).position;
			environment.light = new_position;
		}
	}
	else
		pick.active = false;

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
	

	//deplacement de l'avion vers le haut
	if (inputs.keyboard.up) {

		if (norm(shapes[0].model.translation) + delta_d <= d_max) {

			p += delta_d*vecHaut;

			if(!gui.vue_haut && !gui.overview){
				
				if (c_alpha < Pi / 6) {

					c_alpha += delta_alpha;

					for (int k = 0; k < shapes.size(); ++k) {

						shapes[k].model.rotation = rotation_transform::from_axis_angle(vecRot, -delta_alpha) * shapes[k].model.rotation;

					}

				}
			
			}
			
			
		}

	}

	if (inputs.keyboard.last_action.is_released(GLFW_KEY_UP)) {

		for (int k = 0; k < shapes.size(); ++k) {
			shapes[k].model.rotation = rotation_transform::from_axis_angle(vecRot, c_alpha) * shapes[k].model.rotation;

		}

		c_alpha = 0;

	}


	//deplacement de l'avion vers le bas
	if (inputs.keyboard.down) {

		if (norm(shapes[0].model.translation) - delta_d >= d_min) {

			p -= delta_d * vecHaut;

			if (!gui.vue_haut && !gui.overview) {

				if (c_alpha < Pi / 6) {

					c_alpha +=0.5f*delta_alpha;

					for (int k = 0; k < shapes.size(); ++k) {

						shapes[k].model.rotation = rotation_transform::from_axis_angle(vecRot, 0.5f*delta_alpha) * shapes[k].model.rotation;

					}

				}

			}


		}

	}

	if (inputs.keyboard.last_action.is_released(GLFW_KEY_DOWN)){

		for (int k = 0; k < shapes.size(); ++k) {
			shapes[k].model.rotation = rotation_transform::from_axis_angle(vecRot,-c_alpha) * shapes[k].model.rotation;

		}

		c_alpha = 0;

	}



	//mouvement à gauche avion
	if (inputs.keyboard.left) {

	

			for (int k = 0; k < shapes.size(); ++k) {

				shapes[k].model.rotation = rotation_transform::from_axis_angle(vecHaut,delta_alpha)
					* shapes[k].model.rotation;
			}

			if (!gui.vue_haut && !gui.overview) {

				if (c_alpha < Pi / 6) {

					c_alpha +=  delta_alpha;

					for (int k = 0; k < shapes.size(); ++k) {

						shapes[k].model.rotation = rotation_transform::from_axis_angle(vecDir,delta_alpha) * shapes[k].model.rotation;

					}

				}

			}



		vecRot = normalize(rotation_transform::from_axis_angle(vecHaut, delta_alpha) * vecRot);
	}
	if (inputs.keyboard.last_action.is_released(GLFW_KEY_LEFT)) {

		for (int k = 0; k < shapes.size(); ++k) {
			shapes[k].model.rotation = rotation_transform::from_axis_angle(vecDir, -c_alpha) * shapes[k].model.rotation;

		}

		c_alpha = 0;

	}


	//mouvement à droite avion
	if (inputs.keyboard.right) {



		for (int k = 0; k < shapes.size(); ++k) {

			shapes[k].model.rotation = rotation_transform::from_axis_angle(vecHaut,-delta_alpha)
				* shapes[k].model.rotation;

		}


		if (!gui.vue_haut && !gui.overview) {

			if (c_alpha < Pi / 6) {

				c_alpha += delta_alpha;

				for (int k = 0; k < shapes.size(); ++k) {

					shapes[k].model.rotation = rotation_transform::from_axis_angle(vecDir, -delta_alpha) * shapes[k].model.rotation;

				}

			}

		}

		vecRot = normalize(rotation_transform::from_axis_angle(vecHaut, -delta_alpha) * vecRot);
	}
	if (inputs.keyboard.last_action.is_released(GLFW_KEY_RIGHT)) {

		for (int k = 0; k < shapes.size(); ++k) {
			shapes[k].model.rotation = rotation_transform::from_axis_angle(vecDir, c_alpha) * shapes[k].model.rotation;

		}

		c_alpha = 0;

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

