#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // Получение нормали из нормальной карты
    vec3 normal = texture(normalMap, TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0); // Перевод значений из [0, 1] в [-1, 1]

    // Преобразование в мировое пространство
    normal = normalize(TBN * normal);

    // Освещение
    vec3 lightColor = vec3(1.0);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    // Отражение
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);

    vec3 color = texture(diffuseMap, TexCoords).rgb;
    vec3 lighting = (diff * lightColor + 0.5 * spec) * color;

    FragColor = vec4(lighting, 1.0);
}
