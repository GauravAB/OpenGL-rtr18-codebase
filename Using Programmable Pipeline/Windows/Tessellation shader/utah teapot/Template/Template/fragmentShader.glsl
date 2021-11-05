#version 440 core

out vec4 FragColor;
uniform vec2 uResolution;
uniform float uTimer;

void main(void)
{
	vec2 uv = gl_FragCoord.xy / uResolution;
	
	FragColor = vec4(1.0);
}
