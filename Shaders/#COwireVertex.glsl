#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec2 texCoord;
in vec4 colour;

out Vertex 
{
	vec4 colour;
	vec3 worldPos;
}OUT;

void main (void) 
{
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;

	gl_Position = mvp * vec4 (position , 1.0);
	OUT.worldPos = (modelMatrix * vec4(position, 1.0)).xyz;
	OUT.colour = colour;


	//OUT.texCoord = (textureMatrix * vec4 (texCoord , 0.0 , 1.0)).xy;
}

