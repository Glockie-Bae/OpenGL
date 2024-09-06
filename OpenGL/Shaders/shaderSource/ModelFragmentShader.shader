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
    
	// ������� : ������ϵ�� * ��Դ
	// ���ݹ�Դλ�ú�Ƭ��λ�ü�����߷���
	// ���ù��߷����뷨��������������ϵ��  dot = cos ����Խ�� ϵ��Խ��
    
	vec3 nor = normalize(normal);
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(nor, lightDir), 0.0);
    
    
    
    
	vec3 diffuse = diff * dirLight.diffuse * texture(texture_diffuse2, TexCoords).rgb;
    
	// ����� �� ����ϵ�� * ��Դ
	// �����ӽ�λ�ú�Ƭ��λ�ü���۲췽��
	// ���ù۲췽���뷴����߼��㾵�淴��ϵ�� 
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

    
    // theta : �ֵ�Ͳ��Դ��Χ�� ������������ֱ�䷽��ļн�
    float theta = dot(-lightDir, normalize(spotLight.direction)); 
    // Բ����
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    // clamp ���������Լ����0.0��1.0֮��
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);  
    // ����õ�����ǿ�� ����Բ��ǿ��Ϊ1.0  ����Բ��ǿ��Ϊ0.0  ������Բ��ǿ��Ϊ0.0��1.0֮��


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