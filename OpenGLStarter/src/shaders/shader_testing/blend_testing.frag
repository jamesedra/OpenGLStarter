#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture1;

void main() {
	vec4 texColor = texture(texture1, TexCoords);
	if (texColor.a < 0.2) discard;
	FragColor = vec4(texColor.rgb, max(texColor.a - 0.3, 0.0)); // note we are only subtracting the alpha value for testing
}