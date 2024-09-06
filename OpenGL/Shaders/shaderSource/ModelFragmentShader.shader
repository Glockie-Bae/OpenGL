#version 330 core
out vec4 FragColor;

in vec3 fragPos;
in vec2 TexCoords;
in vec3 normal;

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

uniform int pointLightNumber;
uniform sampler2D texture_ambient1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_ambient2;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular2;
uniform sampler2D texture_ambient3;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular3;
uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform Material material;
uniform PointLight pointLight[4];
uniform DirLight dirLight;
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir){
    vec3 ambient = texture(texture_ambient2, TexCoords).rgb * dirLight.ambient;
    
	// 漫反射光 : 漫反射系数 * 光源
	// 根据光源位置和片段位置计算光线方向
	// 利用光线方向与法向量计算漫反射系数  dot = cos 两线越近 系数越大
    
	vec3 nor = normalize(normal);
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(nor, lightDir), 0.0);
    
    
    
    
	vec3 diffuse = diff * dirLight.diffuse * texture(texture_diffuse2, TexCoords).rgb;
    
	// 镜面光 ： 镜面系数 * 光源
	// 根据视角位置和片段位置计算观察方向
	// 利用观察方向与反射光线计算镜面反射系数 
	vec3 reflectDir = reflect(-lightDir, nor);
    
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * dirLight.specular * texture(texture_specular2, TexCoords).rgb;

    return (ambient + diffuse + specular);
}


vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 viewDir){

    vec3 ambient = pointLight.ambient * texture(texture_ambient2, TexCoords).rgb;

    vec3 nor = normalize(normal);
    vec3 lightDir = normalize(pointLight.position - fragPos);
    float diff = max(dot(nor, lightDir), 0.0);
    vec3 diffuse = diff * pointLight.diffuse * texture(texture_diffuse2, TexCoords).rgb;

    vec3 reflectDir = reflect(-lightDir, nor); 
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = spec * pointLight.specular * texture(texture_specular2, TexCoords).rgb;

    return (ambient + diffuse + specular);
}
    

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(spotLight.position - fragPos);
    float diff = max(dot(normalize(normal), lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * diff * texture(texture_ambient2, TexCoords).rgb;
    
    vec3 ambient = spotLight.ambient * texture(texture_diffuse2, TexCoords).rgb;


    float spec = pow(max(dot(viewDir, reflect(-lightDir, normalize(normal))), 0.0), material.shininess);
    vec3 specular = spotLight.specular * spec * texture(texture_specular2, TexCoords).rgb;
    
    float distance = length(spotLight.position - fragPos);
    float attenuation = (1.0) / (spotLight.constant + distance * spotLight.linear + distance * distance * spotLight.quadratic);

    
    // theta : 手电筒光源范围， 光线射向方向与直射方向的夹角
    float theta = dot(-lightDir, normalize(spotLight.direction)); 
    // 圆环差
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    // clamp 参数将结果约束在0.0到1.0之间
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);  
    // 计算得到光照强度 在内圆内强度为1.0  在外圆外强度为0.0  在内外圆间强度为0.0到1.0之间


    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (ambient + diffuse + specular);
}





void main()
{    
    
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 color = CalcDirLight(dirLight, normal, viewDir);


    
    for (int i = 0; i < pointLightNumber; i++){
        color = CalcPointLight(pointLight[i], normal, viewDir);
    }
    
    color += CalcSpotLight(spotLight, normal, viewDir);

    FragColor = vec4(color, 1.0f);
}