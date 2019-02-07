#version 330 core 

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

in vec4 baseColor[];
out vec4 color;

void main(void)
{ 
       for (int i = 0; i < gl_in.length(); i++) {
              gl_Position = gl_in[i].gl_Position;
			  color = baseColor[i];
              EmitVertex() ;
       }
       EndPrimitive() ;
}