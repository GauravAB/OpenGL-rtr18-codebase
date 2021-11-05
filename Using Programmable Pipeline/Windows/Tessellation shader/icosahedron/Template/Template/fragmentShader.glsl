#version 440 core

out vec4 FragColor;
uniform vec2 uResolution;
uniform float uTimer;
in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in float gPrimitive;
uniform vec3 lightPosition;
uniform vec3 diffuseMaterial;
uniform vec3 ambientMaterial;

float amplify(float d, float scale, float offset)
{
   d = scale * d + offset;
   d = clamp(d, 0, 1);
   d = 1 - exp2(-2*d*d);
   
   return d;
}


void main(void)
{

	vec2 uv = gl_FragCoord.xy / uResolution;
	vec3 N = normalize(gFacetNormal);
	vec3 L = lightPosition;
	float df = abs(dot(N,L));
	vec3 color = ambientMaterial + vec3(1.0)*df * diffuseMaterial;

	float d1 = min(min(gTriDistance.x,gTriDistance.y),gTriDistance.z);
	float d2 = min(min(gPatchDistance.x,gPatchDistance.y),gPatchDistance.z);

	color = amplify(d1,40,-0.5) * amplify(d2,60,-0.5) * color;
	
	FragColor = vec4(color,1.0);
}
