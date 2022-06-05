// Fragment shader
#version 460 core

out vec4 FragColor;

in vec3 ofragCoord;


//uniform vec3 uColor;

void main()
{
    //FragColor = vec4(uColor, 1.0);
	
	FragColor = vec4(ofragCoord, 1.0);
}
