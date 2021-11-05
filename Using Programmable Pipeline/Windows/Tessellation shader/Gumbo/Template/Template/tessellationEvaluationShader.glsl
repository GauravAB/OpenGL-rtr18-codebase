#version 440 core

layout(quads) in;

out vec3 tePosition;
out vec4 tePatchDistance;

uniform mat4 u_modelview_matrix;
uniform mat4 u_projection_matrix;

uniform mat4 B;
uniform mat4 BT;


void main(void)
{
	//flat tessellation
	
	float u = gl_TessCoord.x ;
	float v = gl_TessCoord.y ;
	
	/*
	vec3 a = mix(gl_in[0].gl_Position.xyz,gl_in[3].gl_Position.xyz,u);
	vec3 b = mix(gl_in[12].gl_Position.xyz,gl_in[15].gl_Position.xyz,u);
	tePosition = mix(a,b,v);
	tePatchDistance = vec4(u,v,1-u,1-v);
	gl_Position = u_projection_matrix * u_modelview_matrix * vec4(tePosition,1.0);	
	*/


	//smooth tessellation
	mat4 Px = mat4(gl_in[0].gl_Position.x,gl_in[1].gl_Position.x,gl_in[2].gl_Position.x,gl_in[3].gl_Position.x,
				gl_in[4].gl_Position.x,gl_in[5].gl_Position.x,gl_in[6].gl_Position.x,gl_in[7].gl_Position.x,
				gl_in[8].gl_Position.x,gl_in[9].gl_Position.x,gl_in[10].gl_Position.x,gl_in[11].gl_Position.x,
				gl_in[12].gl_Position.x,gl_in[13].gl_Position.x,gl_in[14].gl_Position.x,gl_in[15].gl_Position.x );

	mat4 Py = mat4(gl_in[0].gl_Position.y,gl_in[1].gl_Position.y,gl_in[2].gl_Position.y,gl_in[3].gl_Position.y,
				gl_in[4].gl_Position.y,gl_in[5].gl_Position.y,gl_in[6].gl_Position.y,gl_in[7].gl_Position.y,
				gl_in[8].gl_Position.y,gl_in[9].gl_Position.y,gl_in[10].gl_Position.y,gl_in[11].gl_Position.y,
				gl_in[12].gl_Position.y,gl_in[13].gl_Position.y,gl_in[14].gl_Position.y,gl_in[15].gl_Position.y);

	mat4 Pz = mat4(gl_in[0].gl_Position.z,gl_in[1].gl_Position.z,gl_in[2].gl_Position.z,gl_in[3].gl_Position.z,
				gl_in[4].gl_Position.z,gl_in[5].gl_Position.z,gl_in[6].gl_Position.z,gl_in[7].gl_Position.z,
				gl_in[8].gl_Position.z,gl_in[9].gl_Position.z,gl_in[10].gl_Position.z,gl_in[11].gl_Position.z,
				gl_in[12].gl_Position.z,gl_in[13].gl_Position.z,gl_in[14].gl_Position.z,gl_in[15].gl_Position.z );

	mat4 cx = B *Px * BT;
	mat4 cy = B *Py * BT;
	mat4 cz = B *Pz * BT;


	vec4 U = vec4(u*u*u, u*u , u , 1.0);
	vec4 V = vec4(v*v*v, v*v , v , 1.0);

	float x = dot(cx*V,U);
	float y = dot(cy*V,U);
	float z = dot(cz*V,U);
	tePosition = vec3(x,y,z);

	tePatchDistance = vec4(u,v,1-u,1-v);
	
	gl_Position = u_projection_matrix * u_modelview_matrix * vec4(x,y,z,1.0);
}	


