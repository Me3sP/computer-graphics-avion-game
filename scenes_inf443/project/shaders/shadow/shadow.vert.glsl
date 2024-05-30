#version 330 core

layout (location = 0) in vec3 vertex_position;

uniform mat4 model; // Model affine transform matrix associated to the current shape
uniform mat4 lightprojectionMatrix;
uniform mat4 lightview;
uniform float time; //Variable de temps. Censée provenir du code c plus plus
uniform float pi;
uniform float f;

void main()
{

	// The position of the vertex in the world space
	// mat4 M = transpose(
    // mat4(1.0, 0.0, 0.0, 0.15*cos((2.0*pi/1.7)*vertex_position.z + f*time*pi/2.0),
    //     0.0, 1.0, 0.0, 0.0,
    //     0.0, 0.0, 1.0, 0.0,
    //     0.0, 0.0, 0.0, 1.0));

	gl_Position = lightprojectionMatrix*lightview/* M */* model*vec4(vertex_position,1.0);
}
