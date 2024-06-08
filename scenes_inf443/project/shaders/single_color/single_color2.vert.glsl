#version 330 core

// Inputs coming from VBOs
layout (location = 0) in vec3 vertex_position; // vertex position in local space (x,y,z)
layout (location = 1) in vec3 vertex_normal;   // vertex normal in local space   (nx,ny,nz)

// Output variables sent to the fragment shader
out struct fragment_data
{
	vec3 position;
	vec3 normal;
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
	// The normal of the vertex in the world space
	mat4 modelNormal = transpose(inverse(model));
	vec4 normal = modelNormal * vec4(vertex_normal, 0.0);
	vec4 position_projected = projection * view * position;
	
	fragment.position = position.xyz;
	fragment.normal = normal.xyz;
	
	gl_Position = position_projected;
}