#include "environment.hpp"

std::string project::path = "";
float project::gui_scale = 1.5f;

environment_structure::environment_structure()
{
	background_color = { 1.0f,1.0f,1.0f };
	light = { 0,4.0,1.5f };
}



void environment_structure::send_opengl_uniform(opengl_shader_structure const& shader, bool expected) const
{
	opengl_uniform(shader, "projection", camera_projection, expected);
	opengl_uniform(shader, "view", camera_view, expected);
	opengl_uniform(shader, "light", light, false);

	uniform_generic.send_opengl_uniform(shader, false);

}