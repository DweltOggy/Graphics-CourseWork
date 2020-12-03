#version 330 core

uniform sampler2D sceneTex;

in Vertex 
{
	vec2 texCoord;
}IN ;

out vec4 fragColor ;

void main (void) 
{
    vec4 temp = texture2D (sceneTex , IN.texCoord);
    float gray = dot(temp.rgb, vec3(0.299, 0.587, 0.114));
    
    fragColor = vec4(gray, gray, gray, 1.0);

}
