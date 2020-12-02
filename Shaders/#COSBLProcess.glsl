#version 330 core

uniform sampler2D sceneTex;

uniform int isVertical;

in Vertex 
{
	vec2 texCoord;
}IN ;

out vec4 fragColor;

const int xdirectionL[3] = int[](-1, -2, -1);
const int xdirectionR[3] = int[](1, 2, 1);

const int ydirectionU[3] = int[](-1, -2, -1);
const int ydirectionD[3] = int[](1, 2, 1);

void main (void) 
{
	fragColor = vec4 (0 ,0 ,0 ,1);
	vec2 delta = vec2 (0 ,0);

	if(isVertical == 1) 
	{
		delta = dFdx (IN.texCoord);
		//delta.y = 1;
	}
	else 
	{
		delta = dFdy (IN.texCoord);
		//delta.x = 1;
	}

	for (int i = 0; i < 7; i ++) 
	{
		//vec2 offset = delta * (i - 3);
		//vec4 tmp = texture2D (sceneTex , IN.texCoord.xy + offset );
		//fragColor += tmp * scaleFactors [i];
	}

}
