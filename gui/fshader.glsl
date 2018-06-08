#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

//uniform sampler2D texture;
//varying vec2 v_texcoord;
varying vec4 v_position;
varying vec4 v_normal;

uniform vec3 lightPos;

//! [0]
void main()
{
	vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(lightPos - v_position);
    float diff = max(dot(norm, lightDir), 0.0);
    gl_FragColor = diff * vec4(0.9, 0.9, 0.9, 1);

    // Set fragment color from texture
    //gl_FragColor = texture2D(texture, v_texcoord);
}
//! [0]

