#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() {
	float laplacianMatrix[9] = float[]( -1, -1, -1, -1, 8, -1, -1, -1, -1 );

	vec3 sampleTex[9];
	int index = 0;

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			vec2 offset = vec2(i, j) * (1.0/300.0);
			sampleTex[index] = vec3(texture(screenTexture, TexCoords.st + offset));
			index++;
		}
	}

	vec3 col = vec3(0,0,0);
	for (int i = 0; i < 9; i++) {
		col += sampleTex[i] * laplacianMatrix[i];
	}

	FragColor = vec4(col, 1.0);
}