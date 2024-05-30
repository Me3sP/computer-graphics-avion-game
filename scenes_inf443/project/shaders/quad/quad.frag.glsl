#version 330 core

in vec2 TexCoords;
uniform sampler2D image_texture;

layout(location=0) out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    float depthValue = texture(image_texture, TexCoords).r;
	float depth = LinearizeDepth(depthValue) / far; // divide by far for demonstration
    FragColor = vec4(vec3(depthValue), 1.0);
}  
