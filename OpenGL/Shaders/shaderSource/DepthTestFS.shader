#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
uniform sampler2D planeTexture;
uniform bool Isblinn;

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};

vec3 CalcPointLight(PointLight pointLight, vec3 Normal, vec3 viewDir){

    float lightLength = max(length(pointLight.position - FragPos), 1);
    float lightStrength = lightLength * lightLength;
    vec3 ambient = pointLight.ambient * texture(planeTexture, TexCoords).rgb ;

    vec3 nor = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(nor, lightDir), 0.0);
    vec3 diffuse = diff * pointLight.diffuse * texture(planeTexture, TexCoords).rgb / lightStrength;
    
    vec3 halfwayDir = normalize(viewDir + lightDir);
    
    vec3 reflectDir = reflect(-lightDir, nor); 
    
    float spec = pow(max(dot(halfwayDir, Normal), 0.0), 64.0);

     
    vec3 specular = spec  * texture(planeTexture, TexCoords).rgb * pointLight.specular * vec3(0.5);

    return (ambient + diffuse + specular);
}

uniform PointLight pointLight;
uniform vec3 viewPos;



void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 color = CalcPointLight(pointLight, Normal, viewDir);

    FragColor = vec4(color, 1.0);
}