#version 330 core 

in struct fragment_data
{
	vec3 position;
	vec3 normal;

} fragment;

// Output of the fragment shader - output color
layout(location=0) out vec4 FragColor;

// Material of the mesh (using a Phong model)
struct material_structure
{
	vec3 color;  // Uniform color of the object

}; 

uniform material_structure material;
uniform mat4 view;  // View matrix (rigid transform) of the camera


void main()
{

	// Compute the position of the center of the camera
	mat3 O = transpose(mat3(view));                   // get the orientation matrix
	vec3 last_col = vec3(view*vec4(0.0, 0.0, 0.0, 1.0)); // get the last column
    vec3 camera_position = -O*last_col;

	// Renormalize normal
	vec3 N = normalize(fragment.normal);

	// Unit direction toward the camera
	vec3 L = normalize(camera_position - fragment.position);

	float coef = abs(dot(N,L)); //necessaire pour le degradé du soleil
	// Compute Shading
	// *************************************** //
	// Compute the base color of the object based on: vertex color, uniform color, and texture
	vec3 color_object = 2.5*coef*material.color;
	
	// Output color, with the alpha component
	FragColor = vec4(color_object, 1.0);
}