#version 330 core
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
 
struct Light {
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
 
	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
};
 
uniform Material material;
uniform Light dirLight;
uniform Light pointLight;
uniform Light spotLight;
uniform vec3 objectColor;
uniform vec3 viewPos;
 
out vec4 FragColor;
 
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
 
vec3 directionalLightColor() {
	vec3 ambient = dirLight.ambient * vec3(texture(material.diffuse, TexCoords));
 
	// light calculations
	vec3 lightDir = normalize(-dirLight.direction);
 
	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = dirLight.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
 
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = dirLight.specular * spec * vec3(texture(material.specular, TexCoords));
 
	return ambient + diffuse + specular;
}
 
vec3 pointLightColor() {
	vec3 lightDir = pointLight.position - FragPos;
	float distance = length(lightDir);
	lightDir = normalize(lightDir);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));
 
	vec3 ambient = pointLight.ambient * vec3(texture(material.diffuse, TexCoords));
 
	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = pointLight.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
 
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = pointLight.specular * spec * vec3(texture(material.specular, TexCoords));
 
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
 
	return ambient + diffuse + specular; 
}
 
vec3 spotLightColor() {
	vec3 lightDir = normalize(spotLight.position - FragPos);
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutOff - spotLight.outerCutOff;
	float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
 
	vec3 ambient = spotLight.ambient * vec3(texture(material.diffuse, TexCoords));
 
	if (intensity > 0.0)
	{
		vec3 norm = normalize(Normal);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = spotLight.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
 
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = spotLight.specular * spec * vec3(texture(material.specular, TexCoords));
 
		diffuse *= intensity;
        specular *= intensity;
 
		return diffuse + specular;
	}
	else return vec3(0,0,0); // since point light and directional light already has ambient, don't add anymore.
 
}
 
void main() {
	FragColor = vec4(directionalLightColor() + pointLightColor() + spotLightColor(), 1.0);
}