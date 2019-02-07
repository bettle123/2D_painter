#version 330 core
// Fragment Shader - file "backdrop.frag"
//   - Should be called either solidColor or Position

out vec4 out_Color;
//in vec4 baseColor2;
in vec4 color;

void main(void)
{
	//vec4 out_Color;
	//out_Color = vec4(0.9,0.3,0.4,1.0);
	float height = 128.0;
	float width = 128.0;
	
	float x = gl_FragCoord.x / width;
	float y = gl_FragCoord.y / height;
	
	vec2 center = vec2(0.5, 0.5);
	float sigma = 3;
	float r = exp( -sigma * ( (x-center.x)*(x-center.x)+(y-center.y)*(y-center.y) ));

	out_Color = color *r ;

}
