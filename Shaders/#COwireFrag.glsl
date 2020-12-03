# version 330 core
uniform mat4 modelMatrix;

in Vertex 
{
	vec4 colour;
	vec3 worldPos;
}IN;

out vec4 fragColour;


void main (void) 
{

	if((fract(IN.worldPos.x / 100) < 0.08) || (fract(IN.worldPos.z / 100) < 0.08))
	{
		fragColour = vec4(1,0.2,0.5,1);	
	}
	else
	{
		fragColour = vec4(0,0,0,0.8);	
	}

}

