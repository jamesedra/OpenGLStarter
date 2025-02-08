#version 330 core

uniform float aValue;
in vec3 ourColor;
out vec4 FragColor;

void main()
{
	FragColor = vec4(ourColor.rgb, aValue);
}