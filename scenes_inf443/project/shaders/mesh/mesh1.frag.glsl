#version 330 core 
// Fragment shader - this code is executed for every pixel/fragment that belongs to a displayed shape
//
// Compute the color using Phong illumination (ambient, diffuse, specular) 
//  There is 3 possible input colors:
//    - fragment_data.color: the per-vertex color defined in the mesh
//    - material.color: the uniform color (constant for the whole shape)
//    - image_texture: color coming from the texture image
//  The color considered is the product of: fragment_data.color x material.color x image_texture
//  The alpha (/transparent) channel is obtained as the product of: material.alpha x image_texture.a
// 

// Inputs coming from the vertex shader
in struct fragment_data
{
    vec3 position; // position in the world space
    vec3 normal;   // normal in the world space
    vec3 color;    // current color on the fragment
    vec2 uv;       // current uv-texture on the fragment

} fragment;

// Output of the fragment shader - output color
layout(location=0) out vec4 FragColor;


// Uniform values that must be send from the C++ code
// ***************************************************** //

uniform sampler2D image_texture;   // Texture image identifiant
uniform sampler2D normalMap;
uniform sampler2D heightMap;   // Texture image identifiant
uniform sampler2D CmpheightMap;
uniform sampler2D WaveA;
uniform sampler2D WaveB;
uniform sampler2D WaveC;
uniform sampler2D WaveD;


uniform float time;
int p;
vec3 triplanarNormal(vec3 position, vec3 surfaceNormal , sampler2D norMap , float scale , float sharpness){

    // Sample normal maps (tangent space)
    vec3 tnormalX = (texture(norMap, position.zy * scale)).xyz;
    vec3 tnormalY = (texture(norMap, position.xz * scale)).xyz;
    vec3 tnormalZ = (texture(norMap, position.xy * scale)).xyz;

    // Swizzle surface normal to match tangent space, and blend with normals from normal map
    tnormalX = vec3(tnormalX.xy + surfaceNormal.zy, tnormalX.z * surfaceNormal.x);
    tnormalY = vec3(tnormalY.xy + surfaceNormal.xz, tnormalY.z * surfaceNormal.y);
    tnormalZ = vec3(tnormalZ.xy + surfaceNormal.xy, tnormalZ.z * surfaceNormal.z);

     // Calculate blend weight
    vec3 weight = pow(abs(surfaceNormal), vec3(sharpness));
    weight /= dot(weight, vec3(1));

     // Swizzle tangent normals to match world normal and blend together
    return  normalize(tnormalX.zyx * weight.x + tnormalY.xzy * weight.y + tnormalZ.xyz * weight.z);

}





uniform mat4 view;       // View matrix (rigid transform) of the camera - to compute the camera position

uniform vec3 light; // position of the light


// Coefficients of phong illumination model
struct phong_structure {
	float ambient;      
	float diffuse;
	float specular;
	float specular_exponent;
};

// Settings for texture display
struct texture_settings_structure {
	bool use_texture;       // Switch the use of texture on/off
	bool texture_inverse_v; // Reverse the texture in the v component (1-v)
	bool two_sided;         // Display a two-sided illuminated surface (doesn't work on Mac)
};

// Material of the mesh (using a Phong model)
struct material_structure
{
	vec3 color;  // Uniform color of the object
	float alpha; // alpha coefficient

	phong_structure phong;                       // Phong coefficients
	texture_settings_structure texture_settings; // Additional settings for the texture
}; 

uniform material_structure material;


void main()
{
	// Compute the position of the center of the camera
	mat3 O = transpose(mat3(view));                   // get the orientation matrix
	vec3 last_col = vec3(view*vec4(0.0, 0.0, 0.0, 1.0)); // get the last column
	vec3 camera_position = -O*last_col;

        // Current uv coordinates
        vec2 uv_image = vec2(fragment.uv.x, fragment.uv.y);
	if(material.texture_settings.texture_inverse_v) {
	uv_image.y = 1.0-uv_image.y;
	}
	// Renormalize normal
        //vec3 N = normalize(2.0*texture(normalMap, uv_image).rgb - 1);
        vec3 N = normalize(fragment.normal);

	// Inverse the normal if it is viewed from its back (two-sided surface)
	//  (note: gl_FrontFacing doesn't work on Mac)
	if (material.texture_settings.two_sided && gl_FrontFacing == false) {
		N = -N;
	}

	// Phong coefficient (diffuse, specular)
	// *************************************** //

	// Unit direction toward the light
	vec3 L = normalize(light-fragment.position);

	// Diffuse coefficient
	float diffuse_component = max(dot(N,L),0.0);
	//float diffuse_component = max( max(dot(N,L),0.0), max(dot(-N,L),0.0));

        // Specular coefficient
	float specular_component = 0.0;

	// Texture
	// *************************************** //

	

	// Get the current texture color
	vec4 color_image_texture = texture(image_texture, uv_image);
        if(material.texture_settings.use_texture == false) {
            //color_image_texture=vec4(1.0,1.0,1.0,1.0);
            //modif
            color_image_texture=texture(CmpheightMap, uv_image);
	}
        //coloring the oceans
        if (color_image_texture[0]<0.03 && color_image_texture[1]<0.08 && color_image_texture[2]<0.05 ){
            color_image_texture = texture(CmpheightMap, uv_image);
            color_image_texture[0]=0;
            color_image_texture[1]+=0.15;
            color_image_texture[2]+=0.2;

            //compute normal
            vec3 N1=triplanarNormal(fragment.position+mod(-0.005*time,1), fragment.normal, WaveA ,30 ,25 );
            vec3 N2=triplanarNormal(fragment.position+mod(+0.005*time,1), fragment.normal, WaveB ,30 ,0 );
            vec3 N3=triplanarNormal(fragment.position+mod(-0.005*time,1), fragment.normal, WaveC ,30 ,0 );
            vec3 N4=triplanarNormal(fragment.position+mod(+0.005*time,1), fragment.normal, WaveD ,20 ,0 );


            N=normalize(N3+N4);

            //diffuse component
            diffuse_component = max(dot(N,L),0.0);

            //specular component
            if(diffuse_component>0.0){
                    vec3 R = reflect(-L,N); // reflection of light vector relative to the normal.
                    vec3 V = normalize(camera_position-fragment.position);
                    specular_component = 0.7*pow( max(dot(R,V),0.0), material.phong.specular_exponent*0.2 );
            }

        }
	
	// Compute Shading
	// *************************************** //

	// Compute the base color of the object based on: vertex color, uniform color, and texture
	vec3 color_object  = fragment.color * material.color * color_image_texture.rgb;

	// Compute the final shaded color using Phong model
        float Ka = 3*material.phong.ambient;
	float Kd = material.phong.diffuse;
	float Ks = material.phong.specular;
        vec3 color_shading = (Ka + Kd*diffuse_component) * color_object + Ks * specular_component * vec3(1.0, 1.0, 1.0);
	
	// Output color, with the alpha component
	FragColor = vec4(color_shading, material.alpha * color_image_texture.a);
}
