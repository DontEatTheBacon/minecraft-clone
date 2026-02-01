#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D texture;

uniform float offset;

void main()
{
    FragColor = texture(texture, TexCoord);
}
