#version 330 core

in vec2 uv_frag;
in vec3 pos_worldspace;
in vec3 n;
in vec3 eyedir_cameraspace;
in mat3 TBN;

out vec4 colorOut;

uniform vec3 _ambient_light_color;
uniform vec3 _ambient_light_direction;

uniform sampler2D _texture_diffuse;
uniform sampler2D _texture_specular;
uniform sampler2D _texture_normal;
uniform sampler2D _texture_emissive;

void main() {

    float minLight = 0.15;

    //vec3 diffuse = vec3(uv_frag.x, (uv_frag.x+uv_frag.y)/3.0, uv_frag.y);
    vec3 color_diffuse = texture2D( _texture_diffuse, uv_frag ).rgb;
    vec3 color_specular = texture2D( _texture_specular, uv_frag ).rgb;
    vec4 color_emissive = texture2D( _texture_emissive, uv_frag );

    vec3 normal = normalize( TBN * ( texture2D( _texture_normal, uv_frag ).rgb * 2.0f - 1.0f ) );

    vec3 l = normalize( _ambient_light_direction );

    float cosTheta = clamp( dot( normal,l ), 0, 1 );

    vec3 E = normalize( eyedir_cameraspace );
    vec3 R = reflect( -l,normal );

    float cosAlpha = clamp( dot( E,R ), 0, 1 );

    vec3 colorVal = (color_emissive.rgb * color_emissive.a) + minLight * color_diffuse + ( (1 - minLight) * color_diffuse * _ambient_light_color * cosTheta ) + ( color_specular * _ambient_light_color * pow(cosAlpha,5) );
    colorOut = vec4(colorVal,1);

}
