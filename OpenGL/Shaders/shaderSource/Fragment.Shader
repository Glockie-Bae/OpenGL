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

struct PointLight {
    vec3 position;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 lightFront;
uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLight;
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform sampler2D texture_3;
uniform float matrixMove;


float Attenuation(PointLight pointLight, vec3 fragPos){
    float distance = length(pointLight.position - fragPos);
    float attenuation = (1.0) / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * distance * distance);
    return attenuation;
}

float Intensity(float theta, float cutOff, float outerCutOff){
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);  
    return intensity;
}

void main()

    // 冯氏光照模型：物体光 = 环境光 * 漫反射光 * 镜面光
{   
    // 光照衰减公式 attenuation = 1.0 / (常量 + 线性常量 * 距离 + 二次常量 * 距离 * 距离) 

   float attenuation = Attenuation(pointLight, fragPos);

   // 环境光 : 环境光系数 * 光源
   float ambientStrength = 0.1f;
   //vec3 ambient = ambientStrength * lightColor;
   vec3 ambient = texture(texture_1, texCoords).rgb * pointLight.ambient ;

   // 漫反射光 : 漫反射系数 * 光源
   // 根据光源位置和片段位置计算光线方向
   // 利用光线方向与法向量计算漫反射系数  dot = cos 两线越近 系数越大
   vec3 nor = normalize(normal);
   vec3 lightDir = normalize(pointLight.position - fragPos);
   float diff = max(dot(lightDir, nor), 0.0);
   vec3 diffuse = diff * pointLight.diffuse * texture(texture_1, texCoords).rgb;
   
   // 镜面光 ： 镜面系数 * 光源
   // 根据视角位置和片段位置计算观察方向
   // 利用观察方向与反射光线计算镜面反射系数 
   vec3 viewDir = normalize(viewPos - fragPos);
   vec3 reflectDir = reflect(-lightDir, nor);

   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   vec3 specular = spec * pointLight.specular * texture(texture_2, texCoords).rgb;

   // 找到光亮的位置 贴图上去

   // 根据光源方向和直射方向计算theta
   vec3 cameraDir = normalize(viewPos - fragPos);
   float theta = dot(cameraDir, normalize(-lightFront)); 

   // 根据内外圆差，计算光亮
   float intensity = Intensity(theta, pointLight.cutOff, pointLight.outerCutOff);  

   diffuse *= intensity;
   specular *= intensity;

   // 衰减
   ambient  *= attenuation; 
   diffuse  *= attenuation;
   specular *= attenuation;   
        

   FragColor = vec4((ambient + diffuse + specular), 1.0);

   
   
}