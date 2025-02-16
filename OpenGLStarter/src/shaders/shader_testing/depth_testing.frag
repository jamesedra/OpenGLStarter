#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform float nearPlane;
uniform float farPlane;

void main() {

	float ndc = gl_FragCoord.z * 2.0 - 1.0;
	float linearDepth = (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - ndc * (farPlane - nearPlane));
	FragColor = vec4(vec3(linearDepth / farPlane), 1.0);
}