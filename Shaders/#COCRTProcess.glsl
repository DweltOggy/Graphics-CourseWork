#version 330 core

uniform sampler2D sceneTex;

uniform int isVertical;

in Vertex 
{
	vec2 texCoord;
}IN ;

out vec4 fragColor;

const float inensity = 0.99;

void main (void) 
{
	vec2 deltay = dFdy (IN.texCoord);
	vec2 deltax = dFdx (IN.texCoord);

	vec4 rValue = texture2D(sceneTex, IN.texCoord.xy - deltax);
	vec4 gValue = texture2D(sceneTex, IN.texCoord.xy);
	vec4 bValue = texture2D(sceneTex, IN.texCoord.xy + deltay);

	vec4 temp = vec4(rValue.r * inensity, gValue.g, bValue.b * inensity, 1.0);

	fragColor = temp;

}
