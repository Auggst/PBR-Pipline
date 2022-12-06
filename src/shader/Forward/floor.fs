#version 430 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

struct DirectionLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

uniform vec3 viewPos;
uniform DirectionLight dtLight[4];
uniform PointLight ptLight[4];
uniform SpotLight spLight[1];
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform bool normalMapping;

vec3 CalculateDL(DirectionLight light, vec3 position, vec3 normal);
vec3 CalculatePL(PointLight light, vec3 position, vec3 normal);
vec3 CalculateSL(SpotLight light, vec3 position, vec3 normal);

void main() {           
    // Obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    // Transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
    vec3 position = fs_in.TBN * fs_in.FragPos;

    // Get diffuse color
    vec3 color = 0.001 * pow(texture(diffuseMap, fs_in.TexCoords).rgb, vec3(2.2));

    // 计算方向光
    for (int i = 0; i < 4; i++) {
        color += CalculateDL(dtLight[i], position, normal);
    }
    // 计算点光源
    for (int i = 0; i < 4; i++) {
        color += CalculatePL(ptLight[i], position, normal);
    }
    // 计算聚光灯
    color += CalculateSL(spLight[0], position, normal);

     // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}

// 计算方向光
vec3 CalculateDL(DirectionLight light, vec3 position, vec3 normal) {
    vec3 result = vec3(0.0);

    // diffuse
    float diff = max(dot(normal, fs_in.TBN * (-light.direction)), 0.0);
    vec3 diffuse = light.diffuse * diff * pow(texture(diffuseMap, fs_in.TexCoords).rgb, vec3(2.2));

    // specular
    vec3 viewDir = normalize(fs_in.TBN * viewPos - position);
    vec3 h = normalize(viewDir - fs_in.TBN * light.direction);
    float spec = pow(max(dot(normal, h), 0.0), 32.0);
    vec3 specular = light.specular * spec * texture(diffuseMap, fs_in.TexCoords).r;

    result += (diffuse + specular);
    
    return result;
}

// 计算点光源
vec3 CalculatePL(PointLight light, vec3 position, vec3 normal) {
    vec3 result = vec3(0.0);
    // diffuse
    vec3 lightDir = normalize(fs_in.TBN * light.position - position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * pow(texture(diffuseMap, fs_in.TexCoords).rgb, vec3(2.2));

    // specular
    vec3 viewDir = normalize(fs_in.TBN * viewPos - position);
    vec3 h = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, h), 0.0), 32.0);
    vec3 specular = light.specular * spec * texture(diffuseMap, fs_in.TexCoords).r;

    // attenuation
    float dis = length(fs_in.TBN * light.position - position);
    float attenuation = 1.0 / (light.kc + light.kl * dis + light.kq * (dis * dis));

    diffuse *= attenuation;
    specular *= attenuation;

    result += (diffuse + specular);

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
    vec3 diffuse = light.diffuse * diff * pow(texture(diffuseMap, fs_in.TexCoords).rgb, vec3(2.2));

    // specular
    vec3 viewDir = normalize(fs_in.TBN * viewPos - position);
    vec3 h = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, h), 0.0), 32.0f);
    vec3 specular = light.specular * spec * texture(diffuseMap, fs_in.TexCoords).r;

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