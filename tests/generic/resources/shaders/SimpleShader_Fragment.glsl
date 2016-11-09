#version 330 core

in vec2 uv_frag;
in vec3 pos_worldspace;
in vec3 n;
in vec3 eyedir_cameraspace;

out vec4 colorOut;

uniform vec3 _ambient_light_color;
uniform vec3 _ambient_light_direction;

uniform sampler2D _texture_diffuse;

void main() {

    //vec3 diffuse = vec3(uv_frag.x, (uv_frag.x+uv_frag.y)/3.0, uv_frag.y);
    vec3 diffuse = texture2D( _texture_diffuse, uv_frag ).rgb;

    vec3 l = normalize( _ambient_light_direction );

    float cosTheta = clamp( dot( n,l ), 0,1 );

    vec3 E = normalize( eyedir_cameraspace );
    vec3 R = reflect( -l,n );

    float cosAlpha = clamp( dot( E,R ), 0,1 );

    vec3 colorVal = 0.5 * (diffuse + ( _ambient_light_color * cosTheta ) + ( _ambient_light_color * pow(cosAlpha,5) ));
    colorOut = vec4(colorVal,1);

}
