
precision mediump float;

in vec2 v_TexCoords;

uniform sampler2D u_Texture;

out vec4 FragColor;

void main()
{
    FragColor = texture(u_Texture, v_TexCoords);
}
