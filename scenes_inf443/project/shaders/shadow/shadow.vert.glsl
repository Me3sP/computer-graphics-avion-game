#version 330 core

layout (location = 0) in vec3 vertex_position;

uniform mat4 model; // Model affine transform matrix associated to the current shape
uniform mat4 lightprojectionMatrix;
uniform mat4 lightview;

void main()
{

	gl_Position = lightprojectionMatrix*lightview* model*vec4(vertex_position,1.0);
}
