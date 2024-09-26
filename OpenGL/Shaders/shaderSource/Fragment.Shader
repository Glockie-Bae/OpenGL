#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform bool normalMapping;


float PI = 3.14;

float D_GGX_TR(vec3 N, vec3 H, float a){
// 计算发现分布函数
// 与某些（半程）向量h取向一致的微平面的比率。
// 公式为：result = (a * a) / PI * ((N * H)(N * H) * ((a * a - 1) +1))^2

    float a2 = a * a;
    float NDotH = max(dot(N, H), 0.0);
    float NDotH2 = NDotH * NDotH;
    float denom = NDotH2 * (a2 - 1.0) + 1.0;
	return a2 / (PI * denom * denom);
}

float SchlickGGX(vec3 N, vec3 V, float K){
// 微平面间相互遮蔽的比率
// 粗糙度较高的表面其微平面间相互遮蔽的概率就越高
// 公式为： result = N * V / (N * V) * (1 - K) + K
// k 是α的重映射(Remapping) k_dir = (a + 1) * (a + 1) / 8   k_IBL = (a * a) / 2

    float NDot = max(dot(N, V), 0.0);
    return NDot / NDot * (1.0 - K) + K;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float K){
    float ggx1 = SchlickGGX(N, V, K);
    float ggx2 = SchlickGGX(N, L, K);
    return ggx1 * ggx2;
}


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
// 涅斐尔方程 result = F + (1 - F)(1 - (H * V))^5
// F 根据不同的物体进行定义，H * V用cosTheta表示
// F0 = mix(F0, surfaceColor.rgb, metalness);

    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}




void main()
{           
    // Obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    // Transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    // Get diffuse color
    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;
    // Ambient
    vec3 ambient = 0.1 * color;
    // Diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // Specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;
    
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}