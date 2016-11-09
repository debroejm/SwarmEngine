#version 330 core

layout(location = 1) in vec3 vertex;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 normal;
layout(location = 5) in vec3 tangent;
layout(location = 6) in vec3 bitangent;

out vec3 n;
out vec3 pos_worldspace;
out vec3 eyedir_cameraspace;
out vec2 uv_frag;
out mat3 TBN;

uniform mat4 _m;
uniform mat4 _v;
uniform mat4 _p;

void main(){

    //gl_Position = vec4(vertex * 0.5,1);
	gl_Position = _p * _v * _m * vec4(vertex,1);

	mat4 _mv = _v * _m;

	vec3 T = normalize(vec3(_mv * vec4(tangent, 1.0)));
    vec3 B = normalize(vec3(_mv * vec4(bitangent, 1.0)));
    vec3 N = normalize(vec3(_mv * vec4(normal, 1.0)));
    TBN = mat3(T, B, N);

	pos_worldspace = (_m * vec4(vertex,1)).xyz;

	// Vector : vertex -> camera : camera space
	vec3 vertex_cameraspace = ( _mv * vec4(vertex,1)).xyz;
	eyedir_cameraspace = vec3(0,0,0) - vertex_cameraspace;

	n = normalize(( _mv * vec4(normal,0)).xyz);

	uv_frag = uv;
	//UV.y = 1.0 - vertexUV.y;\

}