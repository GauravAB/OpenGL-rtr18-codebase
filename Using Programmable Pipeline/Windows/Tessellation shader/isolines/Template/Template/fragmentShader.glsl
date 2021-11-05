#version 440 core

out vec4 FragColor;
uniform vec2 uResolution;
uniform float uTimer;


void main(void)
{
	vec2 uv = gl_FragCoord.xy / uResolution;
	vec3 col =  0.5 + 0.5*cos(vec3(uTimer + uv.xy, 1.0) + vec3(0, 2, 4));
	
	FragColor = vec4(col, 1.0);
}
