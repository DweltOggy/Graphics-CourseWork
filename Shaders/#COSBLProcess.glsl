#version 330 core

uniform sampler2D sceneTex;

uniform int isVertical;

in Vertex 
{
	vec2 texCoord;
}IN ;

out vec4 fragColor ;

const float scaleFactors [3] =
float []( 1.0 , 5.0 , 1.0);

void main (void) 
{

	fragColor = texture2D (sceneTex , IN.texCoord);

	vec2 delta = vec2 (0 ,0);
	vec2 delta2 = vec2 (0 ,0);

	vec4 tempColour;

	if(isVertical == 1) 
	{
		delta = dFdx (IN.texCoord);
		delta2 = dFdy (IN.texCoord);
	}
	else 
	{

		delta = dFdy (IN.texCoord);
		delta2 = dFdx (IN.texCoord);
	}

	for (int i = 0; i < 3; i ++) 
	{
		vec2 offset = delta * (i - 1);
		offset += delta2;
		vec4 tmp = texture2D (sceneTex , IN.texCoord.xy + offset );


		fragColor += tmp * (scaleFactors [i] * -(i -1));
	}

	for (int i = 0; i < 3; i ++) 
	{
		vec2 offset = delta * (i - 1);
		offset -= delta2;
		vec4 tmp = texture2D (sceneTex , IN.texCoord.xy + offset );
		fragColor += tmp * (scaleFactors [i] * -(i-1));
	}

}
