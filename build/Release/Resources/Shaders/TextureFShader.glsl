#version 330 core

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

out vec3 color;

uniform sampler2D textureSampler;
uniform vec3 LightPosition_worldspace;

void main(){
	vec3 LightColor = vec3(3.0f,3.0f,3.0f);
	float LightPower = 50.0f;
	
	vec3 MaterialDiffuseColor = texture2D( textureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.5,0.5,0.5) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);
	
	float distance = length( LightPosition_worldspace - Position_worldspace );
	
	vec3 n = normalize( Normal_cameraspace );
	vec3 l = normalize( LightDirection_cameraspace );
	
	// clamped:
	//  light is at the vertical of the triangle : 1
	//  light is perpendicular to the triangle : 0
	//  light is behind triangle : 0
	//  light is at another angle : >0
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	vec3 E = normalize(EyeDirection_cameraspace);
	vec3 R = reflect(-l,n);
	
	// clamped:
	//  looking into reflection : 1
	//  looking elsewhere : <1
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	color = MaterialAmbientColor + ( MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) ) + ( MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance) );
}