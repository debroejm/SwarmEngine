#version 330 core

layout(location = 1) in vec3 vertex;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 normal;

out vec3 n;
out vec3 pos_worldspace;
out vec3 eyedir_cameraspace;
out vec2 uv_frag;

uniform mat4 _m;
uniform mat4 _v;
uniform mat4 _p;

void main(){

    //gl_Position = vec4(vertex * 0.5,1);
	gl_Position = _p * _v * _m * vec4(vertex,1);

	pos_worldspace = (_m * vec4(vertex,1)).xyz;

	mat4 _mv = _v * _m;

	// Vector : vertex -> camera : camera space
	vec3 vertex_cameraspace = ( _mv * vec4(vertex,1)).xyz;
	eyedir_cameraspace = vec3(0,0,0) - vertex_cameraspace;

	n = normalize(( _mv * vec4(normal,0)).xyz);

	uv_frag = uv;
	//UV.y = 1.0 - vertexUV.y;\

}