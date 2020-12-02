#version 330 core

uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;

uniform vec3 cameraPos;
uniform bool perfect;

in Vertex 
{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main (void) 
{
	vec4 diffuse = texture ( diffuseTex , IN.texCoord );
	vec3 viewDir = normalize ( cameraPos - IN.worldPos );

	vec3 reflectDir = reflect (- viewDir , normalize (IN.normal));
	vec4 reflectTex = texture (cubeTex, reflectDir);
	if (perfect)
	{
		fragColour = reflectTex; 
	}
	else
	{
		fragColour = reflectTex + (diffuse * 0.25f);
	}
	
}
