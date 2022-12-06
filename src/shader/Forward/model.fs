#version 430 core

out vec4 FragColor;

struct Material{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
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

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;

    float kc;
    float kl;
    float kq;
};

in VS_OUT {
    vec2 TexCoords;
    vec3 Position;
    vec3 Normal;
    mat3 TBN;
} fs_in;

uniform vec3 viewPos;
uniform Material material;
uniform DirectionLight dtLight[4];
uniform PointLight ptLight[4];
uniform SpotLight spLight[1];


vec3 CalculateDL(DirectionLight light, vec3 position, vec3 normal);
vec3 CalculatePL(PointLight light, vec3 position, vec3 normal);
vec3 CalculateSL(SpotLight light, vec3 position, vec3 normal);

void main() {
    vec3 norm = texture(material.texture_normal1, fs_in.TexCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0);
    vec3 pos = fs_in.TBN * fs_in.Position;

    vec3 color = pow(texture(material.texture_diffuse1, fs_in.TexCoords).rgb, vec3(2.2));
    // 方向光
    for (int i = 0; i < 4; i++)
        color = color + CalculateDL(dtLight[i], pos, norm);

    // // 点光源
    // for (int i = 0; i< 4; i++)
    //     color = color + CalculatePL(ptLight[i], pos, norm);

    // // 聚光灯
    // color = color + CalculateSL(spLight[0], pos, norm);

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}

// 计算点光源
vec3 CalculatePL(PointLight light, vec3 position, vec3 normal) {
    vec3 result = vec3(0.0);

    // diffuse
    vec3 lightDir = normalize(fs_in.TBN * light.position - position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * pow(texture(material.texture_diffuse1, fs_in.TexCoords).rgb, vec3(2.2));

    // specular
    vec3 viewDir = normalize(fs_in.TBN * viewPos - position);
    vec3 h = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, h), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb;

    // attenuation
    float dis = length(fs_in.TBN * light.position - position);
    float attenuation = 1.0 / (light.kc + light.kl * dis + light.kq * (dis * dis));

    diffuse *= attenuation;
    specular *= attenuation;

    result += (diffuse + specular);

    return result;
}

// 计算方向光
vec3 CalculateDL(DirectionLight light, vec3 position, vec3 normal) {
    vec3 result = vec3(0.0);

    // diffuse
    float diff = max(dot(normal, fs_in.TBN *(-light.direction)), 0.0);
    vec3 diffuse = light.diffuse * diff * pow(texture(material.texture_diffuse1, fs_in.TexCoords).rgb, vec3(2.2));

    // // specular
    vec3 viewDir = normalize(fs_in.TBN * viewPos - position);
    vec3 h = normalize(viewDir - fs_in.TBN * light.direction);
    float spec = pow(max(dot(normal, h), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb;

    // result += (diffuse + specular);
    result += diffuse;
    
    return result;
}

// 计算聚光灯
vec3 CalculateSL(SpotLight light, vec3 position, vec3 normal) {
    vec3 result = vec3(0.0);
    vec3 lightDir = normalize(fs_in.TBN * light.position - position);
    float theta = dot(lightDir, normalize(fs_in.TBN * (-light.direction)));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * pow(texture(material.texture_diffuse1, fs_in.TexCoords).rgb, vec3(2.2));

    // specular
    vec3 viewDir = normalize(fs_in.TBN * viewPos - position);
    vec3 h = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, h), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb;

    diffuse *= intensity;
    specular *= intensity;

    // attenuation
    float dis = length(fs_in.TBN * light.position - position);
    float attenuation = 1.0 / (light.kc + light.kl * dis + light.kq * (dis * dis));
    
    diffuse *= attenuation;
    specular *= attenuation;

    result += (diffuse + specular);
    return result;
}