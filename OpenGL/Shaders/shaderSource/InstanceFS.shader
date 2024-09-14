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
    
	// ������� : ������ϵ�� * ��Դ
	// ���ݹ�Դλ�ú�Ƭ��λ�ü�����߷���
	// ���ù��߷����뷨��������������ϵ��  dot = cos ����Խ�� ϵ��Խ��
    
	vec3 nor = normalize(normal);
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(nor, lightDir), 0.0);
    
    
    
    
	vec3 diffuse = diff * dirLight.diffuse * texture(texture_diffuse1, TexCoords).rgb;
    
	// ����� �� ����ϵ�� * ��Դ
	// �����ӽ�λ�ú�Ƭ��λ�ü���۲췽��
	// ���ù۲췽���뷴����߼��㾵�淴��ϵ�� 
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