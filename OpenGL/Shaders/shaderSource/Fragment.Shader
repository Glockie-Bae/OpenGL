#version 330 core

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

out vec4 FragColor;
  
struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform sampler2D texture_3;
uniform float matrixMove;


void main()

    // 冯氏光照模型：物体光 = 环境光 * 漫反射光 * 镜面光
{   
    // 环境光 : 环境光系数 * 光源
    float ambientStrength = 0.1f;
    //vec3 ambient = ambientStrength * lightColor;
    vec3 ambient = texture(texture_1, texCoords).rgb * light.ambient ;

    // 漫反射光 : 漫反射系数 * 光源
    // 根据光源位置和片段位置计算光线方向
    // 利用光线方向与法向量计算漫反射系数  dot = cos 两线越近 系数越大
    vec3 nor = normalize(normal);
    vec3 lightDir = normalize(lightPos  - fragPos);
    float diff = max(dot(lightDir, nor), 0.0);
    vec3 diffuse = diff * light.diffuse * texture(texture_1, texCoords).rgb;
    
    // 镜面光 ： 镜面系数 * 光源
    // 根据视角位置和片段位置计算观察方向
    // 利用观察方向与反射光线计算镜面反射系数 
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, nor);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * texture(texture_2, texCoords).rgb;

    // 找到光亮的位置 贴图上去
    //vec3 emission = texture(texture_2, texCoords).rgb * texture(texture_3, texCoords).rgb;

    vec3 emission = texture(texture_3, vec2(texCoords.x, texCoords.y + matrixMove)).rgb ;

    FragColor = vec4((ambient + diffuse + specular + emission), 1.0);
}