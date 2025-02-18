#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
	vec3 viewDir = normalize(FragPos - cameraPos); // view direction of the camera
	vec3 reflectionVector = reflect(viewDir, normalize(Normal));
	FragColor = vec4(texture(skybox, reflectionVector).rgb, 1.0);
}