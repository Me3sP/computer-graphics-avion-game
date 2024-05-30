#version 330 core

layout (location = 0) in vec4 vertex_position;
layout (location = 3) in vec2 uv;

out vec2 TexCoords;

void main()
{
	TexCoords =  uv;

	gl_Position = vertex_position;
}
