#version 330 core
// Vertex Shader - file "backdrop.vert"

in  vec2 in_Position;

uniform vec4 col;

out vec4 baseColor;

void main(void)
{
	gl_Position = vec4(in_Position, 1.0, 1.0);
	baseColor = vec4(0.95f,0.5f,0.0f,1.0f);
}
