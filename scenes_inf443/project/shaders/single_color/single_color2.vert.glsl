#version 330 core

// Inputs coming from VBOs
layout (location = 0) in vec3 vertex_position; // vertex position in local space (x,y,z)
layout (location = 2) in vec3 vertex_color;    // vertex color      (r,g,b)

// Output variables sent to the fragment shader
out struct fragment_data
{
    vec3 color;    // vertex color
} fragment;

// Uniform variables expected to receive from the C++ program
uniform mat4 model; // Model affine transform matrix associated to the current shape
uniform mat4 view;  // View matrix (rigid transform) of the camera
uniform mat4 projection; // Projection (perspective or orthogonal) matrix of the camera



void main()
{
	// The position of the vertex in the world space
	vec4 position = model * vec4(vertex_position, 1.0);
	// The projected position of the vertex in the normalized device coordinates:
	vec4 position_projected = projection * view * position;
	
	fragment.color = vertex_color;
	// gl_Position is a built-in variable which is the expected output of the vertex shader
	gl_Position = position_projected; // gl_Position is the projected vertex position (in normalized device coordinates)
}