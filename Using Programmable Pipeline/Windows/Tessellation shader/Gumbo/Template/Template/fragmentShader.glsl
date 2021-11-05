#version 440 core

out vec4 FragColor;
uniform vec2 uResolution;
uniform float uTimer;
in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec4 gPatchDistance;


uniform float shininess;	
uniform vec3 lightPosition;
uniform vec3 diffuseMaterial;
uniform vec3 ambientMaterial;
uniform vec3 specularMaterial;

const vec3 InnerLineColor = vec3(1.0,1.0,1.0);
const bool DrawLines = false;

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
	vec3 E = vec3(0.0,0.0,1.0);
	vec3 H = normalize(L + E);


	float df = abs(dot(N,L));
	float sf = abs(dot(N,H));
	sf = pow(sf,shininess);

	vec3 color = ambientMaterial + df * diffuseMaterial + sf * specularMaterial;

	if(DrawLines)
	{
		 float d1 = min(min(gTriDistance.x,gTriDistance.y),gTriDistance.z);
		float d2 = min(min(gPatchDistance.x,gPatchDistance.y),gPatchDistance.z);

		d1 = 1 - amplify(d1,50,-0.5);
		d2 = amplify(d2,50,-0.5);
	
		color = d2 * color + d1 * d2 ;
	

	}
	
	
	
	FragColor = vec4(color,1.0);
}
