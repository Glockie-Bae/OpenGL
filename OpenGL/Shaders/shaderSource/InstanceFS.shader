#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D texture_diffuse1;

struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


vec3 CalcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir){
    vec3 ambient = texture(texture_diffuse1, TexCoords).rgb * dirLight.ambient;
    
	// 漫反射光 : 漫反射系数 * 光源
	// 根据光源位置和片段位置计算光线方向
	// 利用光线方向与法向量计算漫反射系数  dot = cos 两线越近 系数越大
    
	vec3 nor = normalize(normal);
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(nor, lightDir), 0.0);
    
    
    
    
	vec3 diffuse = diff * dirLight.diffuse * texture(texture_diffuse1, TexCoords).rgb;
    
	// 镜面光 ： 镜面系数 * 光源
	// 根据视角位置和片段位置计算观察方向
	// 利用观察方向与反射光线计算镜面反射系数 
	vec3 reflectDir = reflect(-lightDir, nor);
    
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = spec * dirLight.specular * texture(texture_diffuse1, TexCoords).rgb;

    return (ambient + diffuse + specular);
}

uniform DirLight dirLight;
uniform vec3 cameraPos;

void main()
{
    vec3 viewDir = normalize(fragPos - cameraPos);
    vec3 color = CalcDirLight(dirLight, normalize(normal), viewDir);
    FragColor = vec4(texture(texture_diffuse1, TexCoords).rgb * color, 1.0);
}