#version 330 core

in vec2 uv_frag;
in vec3 pos_worldspace;
in vec3 n;
in vec3 eyedir_cameraspace;
in vec3 lightdir_cameraspace;

out vec4 color;

uniform sampler2D texturemap;

void main(){
	vec3 LightColor = vec3(3.0f,3.0f,3.0f);
	float LightPower = 50.0f;
	vec3 LightPosition_worldspace = vec3(0.0f, 0.0f, 0.0f);
	
	vec3 MaterialDiffuseColor = texture2D( texturemap, uv_frag ).rgb;
	vec3 MaterialAmbientColor = vec3(0.5,0.5,0.5) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);
	
	float distance = length( LightPosition_worldspace - pos_worldspace );

	vec3 l = normalize( lightdir_cameraspace );
	
	// clamped:
	//  light is at the vertical of the triangle : 1
	//  light is perpendicular to the triangle : 0
	//  light is behind triangle : 0
	//  light is at another angle : >0
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	vec3 E = normalize(eyedir_cameraspace);
	vec3 R = reflect(-l,n);
	
	// clamped:
	//  looking into reflection : 1
	//  looking elsewhere : <1
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	vec3 colorVal = MaterialAmbientColor + ( MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) ) + ( MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance) );
	color = vec4(colorVal,1.0f);
}