#version 330 core

layout(location = 1) in vec3 vertex; // Actually bones
layout(location = 2) in vec3 normal; // Actually vertex offsets from bones
layout(location = 4) in vec2 uv;

out vec3 n;
out vec3 pos_worldspace;
out vec3 eyedir_cameraspace;
out vec3 lightdir_cameraspace;
out vec2 uv_frag;

uniform mat4 _m;
uniform mat4 _v;
uniform mat4 _p;

void main(){
    // Specialized bone shader
	vec3 vertexPosition_modelspace = vertex + normal;
	
	gl_Position = _p * _v * _m * vec4(vertexPosition_modelspace,1);
	//gl_Position = vec4(vertexPosition_modelspace,1);
	
	pos_worldspace = (_m * vec4(vertexPosition_modelspace,1)).xyz;
	
	// Vector : vertex -> camera : camera space
	vec3 vertexPosition_cameraspace = ( _v * _m * vec4(vertexPosition_modelspace,1)).xyz;
	eyedir_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
	
	// Vector : vertex -> light : camera space
	vec3 LightPosition_worldspace = vec3(0.0f);
	vec3 LightPosition_cameraspace = ( _v * vec4(LightPosition_worldspace,1)).xyz;
	lightdir_cameraspace = LightPosition_cameraspace + eyedir_cameraspace;
	
	n = normalize(( _v * _m * vec4(normal,0)).xyz);
	
	uv_frag = uv;
	//UV.y = 1.0 - vertexUV.y;
}