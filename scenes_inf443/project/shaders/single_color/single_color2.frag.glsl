#version 330 core 

in struct fragment_data
{
    vec3 color;    // current color on the fragment

} fragment;

// Output of the fragment shader - output color
layout(location=0) out vec4 FragColor;

// Material of the mesh (using a Phong model)
struct material_structure
{
	vec3 color;  // Uniform color of the object

}; 

uniform material_structure material;


void main()
{
	
	// Compute Shading
	// *************************************** //

	// Compute the base color of the object based on: vertex color, uniform color, and texture
	vec3 color_object  = fragment.color *material.color;
	
	// Output color, with the alpha component
	FragColor = vec4(color_object, 1.0);
}