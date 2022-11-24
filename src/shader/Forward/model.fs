#version 430 core

out vec4 FragColor;

struct Material{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

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

struct DirectionLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight{
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float kc;
    float kl;
    float kq;
};

in VS_OUT {
    vec2 TexCoords;
    vec3 Position;
    vec3 Normal;
} fs_in;

uniform int plNums;
uniform int dlNums;
uniform int slNums;
uniform vec3 viewPos;
uniform Material material;
uniform PointLight plLight[4];
uniform DirectionLight dlLight[4];
uniform SpotLight spLight[4];

vec3 CalculatePL(vec3 position, vec3 normal);
vec3 CalculateDL(vec3 position, vec3 normal);
vec3 CalculateSL(vec3 position, vec3 normal);

void main() {
    vec3 norm = normalize(fs_in.Normal);

    // 点光源
    vec3 color = CalculatePL(fs_in.Position, norm);

    // 方向光
    color += CalculateDL(fs_in.Position, norm);

    // 聚光灯
    color += CalculateSL(fs_in.Position, norm);

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}

// 计算点光源
vec3 CalculatePL(vec3 position, vec3 normal) {
    vec3 result = vec3(0.0);
    for (int i = 0; i < plNums; i++) {
        // ambient
        vec3 ambient = plLight[i].ambient * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;

        // diffuse
        vec3 lightDir = normalize(plLight[i].position - position);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = plLight[i].diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

        // specular
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        vec3 h = normalize(viewDir + lightDir);
        float spec = pow(max(dot(normal, h), 0.0), material.shininess);
        vec3 specular = plLight[i].specular * spec * texture(material.specular, fs_in.TexCoords).rgb;

        // attenuation
        float dis = length(plLight[i].position - FragPos);
        float attenuation = 1.0 / (plLight[i].kc + plLight[i].kl * dis + plLight[i].kq * (dis * dis));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        result += (ambient + diffuse + specular);
    }
    return result;
}

// 计算方向光
vec3 CalculateDL(vec3 position, vec3 normal) {
    vec3 result = vec3(0.0);
    for (int i = 0; i < dlNums; i++) {
        // ambient
        vec3 ambient = dlLight[i].ambient * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;

        // diffuse
        float diff = max(dot(normal, dlLight[i].direction), 0.0);
        vec3 diffuse = dlLight[i].diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

        // specular
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        vec3 h = normalize(viewDir + dlLight[i].direction);
        float spec = pow(max(dot(normal, h), 0.0), material.shininess);
        vec3 specular = dlLight[i].specular * spec * texture(material.specular, fs_in.TexCoords).rgb;

        result += ambient + diffuse + specular;
    }
    return result;
}

// 计算聚光灯
vec3 CalculateSL(vec3 position, vec3 normal) {
    vec3 result = vec3(0.0);
    for (int i = 0; i < slNums; i++) {
        vec3 lightDir = normalize(spLight[i].position - position);
        float theta = dot(lightDir, normalize(-spLight[i].direction));
        float epsilon = spLight[i].cutOff - spLight[i].outerCutOff;
        float intensity = clamp((theta - spLight[i].outerCutOff) / epsilon, 0.0, 1.0);

        // ambient
        vec3 ambient = spLight.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

        // diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = spLight[i].diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

        // specular
        vec3 viewDir = normalize(viewPos - position);
        vec3 h = viewDir + lightDir;
        float spec = pow(max(dot(normal, h), 0.0), material.shininess);
        vec3 specular = spLight[i].specular * spec * texture(material.texture_specular1, TexCoords).rgb;

        // attenuation
        float dis = length(spLight[i].position - position);
        float attenuation = 1.0 / (spLight[i].kc + spLight[i].kl * dis + spLight[i].kq * (dis * dis));
        
        

        diffuse *= (attenuation * intensity);
        specular *= (attenuation * intensity);

        result += (ambient + diffuse + specular);
    }
    return result;
}