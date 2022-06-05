// Vertex Shader
#version 460 core
in vec3 iPosition;

out vec3 ofragCoord;

uniform float uOffset;
//uniform mat4 uTransform;
//uniform mat4 uProjection;
//uniform mat4 uView;

void main()
{
    //fragCoord = aPosition.xy + vec2(0.5);
    //gl_Position = uProjection * uView * (uTransform * vec4(aPosition, 1.0));
	
	ofragCoord = iPosition;
	gl_Position = vec4(iPosition.x + uOffset, -iPosition.y, iPosition.z, 1.0);
}
