#version 410 core
out vec4 Color;

in vec3 Normal;
in vec3 FragPos;

// Структура источника света
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Структура материала
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main() {
    // 1. Окружающее освещение (Ambient)
    vec3 ambient = light.ambient * material.ambient;

    // 2. Диффузное освещение (Diffuse)
    vec3 norm      = normalize(Normal);
    vec3 lightDir  = normalize(light.position - FragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3 diffuse   = light.diffuse * (diff * material.diffuse);

    // 3. Зеркальное освещение (Specular)
    vec3 viewDir    = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular   = light.specular * (spec * material.specular);

    // Итоговый цвет
    vec3 result = ambient + diffuse + specular;
    Color = vec4(result, 1.0);
}
