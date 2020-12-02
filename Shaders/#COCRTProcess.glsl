#version 330 core

uniform sampler2D sceneTex;
uniform sampler2D mixTex;

in Vertex 
{
	vec2 texCoord;
}IN ;

out vec4 fragColor;


void main (void) 
{
	
	vec4 mixer = texture (mixTex , IN.texCoord);

	vec2 deltay = dFdy (IN.texCoord);
	vec2 deltax = dFdx (IN.texCoord);

	vec4 rValue = texture2D(sceneTex, IN.texCoord.xy - (deltax * 1));
	vec4 gValue = texture2D(sceneTex, IN.texCoord.xy);
	vec4 bValue = texture2D(sceneTex, IN.texCoord.xy + (deltay * 1));

	vec4 temp = vec4(rValue.r, gValue.g, bValue.b, 1.0);

	fragColor = temp + (mixer * 0.06f);

}
