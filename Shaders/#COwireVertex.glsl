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

	vec4 worldPos = ( modelMatrix * vec4 ( position ,1));

	OUT.worldPos = worldPos.xyz ;

	OUT.colour = colour;

	gl_Position = ( projMatrix * viewMatrix ) * worldPos;

}

