#version 430 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;

struct PointLight{
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    /* 衰减参数: c常数项，l一次项，q二次项 */
    float kc;
    float kl;
    float kq;
};

const int NR_LIGHTS = 32;
uniform PointLight ptLight[NR_LIGHTS];
uniform vec3 viewPos;

vec3 CalculatePL(PointLight light, vec3 position, vec3 normal, vec3 diffuse, float specular);

void main() {
    // 从gbuffer中读取数据
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gColorSpec, TexCoords).rgb;
    float Specular = texture(gColorSpec, TexCoords).a;

    // 计算光照
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 color = vec3(0.0);
    for (int i = 0; i < NR_LIGHTS; ++i) {
        color += CalculatePL(ptLight[i], FragPos, normalize(Normal), Diffuse, Specular);
    }
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}

// 计算点光源
vec3 CalculatePL(PointLight light, vec3 position, vec3 normal, vec3 diffuse, float specular) {
    vec3 result = vec3(0.0);
    light.ambient = vec3(0.001);
    // ambient
    vec3 ambient = light.ambient * diffuse;

    // diffuse
    vec3 lightDir = normalize(light.position - position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 dif = light.diffuse * diff * diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - position);
    vec3 h = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, h), 0.0), 32.0f);
    vec3 spe = light.specular * spec * specular;

    // attenuation
    float dis = length(light.position - position);
    float attenuation = 1.0 / (light.kc + light.kl * dis + light.kq * (dis * dis));

    ambient *= attenuation;
    dif *= attenuation;
    spe *= attenuation;

    result += (ambient + dif + spe);

    return result;
}
