#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
	float ratio = 1.00/1.52; // air/glass refractive index
	vec3 viewDir = normalize(FragPos - cameraPos);
	vec3 refractionVector = refract(viewDir, normalize(Normal), ratio);
	FragColor = vec4(texture(skybox, refractionVector).rgb, 1.0);
}