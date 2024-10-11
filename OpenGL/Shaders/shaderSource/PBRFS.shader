#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;


// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

struct Material{

    // �������յĹ���
    // (0.0f, 0.0f, 1.0f) ��ʾֻ������ɫ����
    vec3 albedo;           // ��ɫ
    float metallic;             // ������
	float roughness;            // �ֲڶ�
    float ao;                   // �������ڱ�
};

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;

uniform sampler2D modelDiffuseMap;
uniform sampler2D modelNormalMap;
uniform sampler2D modelspecularMap;

uniform sampler2D brdfLUT;

uniform bool IsIrradianceMap;
uniform bool IsModel;

uniform Material material;
uniform float model_metallic;
uniform float model_roughness;

uniform bool IsTexture;

uniform vec3 camPos;

// ����ӳ�䵽���߿ռ�
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}




const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{   
    // a = roughness
    // ���߷ֲ������� result = a * a / PI * ( (N * H)^2 * (a * a - 1) +1)^2
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    // k Ϊ roughness ����ӳ��
    // k_dir = (a + 1)^2 / 8
    // k_IBL = a^2 / 4

    // ���κ��� result = (N * V) /( (N * V)(1 - k) + k )
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    // Ϊ����Ч�Ĺ��㼸�β��֣���Ҫ���۲췽�򣨼����ڱ�(Geometry Obstruction)���͹��߷���������������Ӱ(Geometry Shadowing)��
    // �����ǽ�ȥ�����ǿ���ʹ��ʷ��˹��(Smith��s method)�������߶��������У�
    // result = G(Normal, ViewDir, k) / G(Normal, LightDir, k)
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
   // ���������� result = F0 + (1 - F0) * ( (1 - (H * V))^5 )
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

void main()
{		

     vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
     float metallic  = texture(metallicMap, TexCoords).r;
     float roughness = texture(roughnessMap, TexCoords).r;
     float ao        = texture(aoMap, TexCoords).r;


     if(!IsTexture){
        albedo = material.albedo;
        metallic = material.metallic;
        roughness = material.roughness;
        ao = 1.0;   
     }
    

    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N);
    
    // ���ݲ�ͬ���������ò�ͬ��F0ֵ
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // ��Դ����
        vec3 L = normalize(lightPositions[i] - WorldPos);
        // ��Դ�����ߵİ������
        vec3 H = normalize(V + L);

        // ���ݾ������˥��ϵ��
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);

        // ����ǿ�� = ������ɫ * ����˥��ϵ��
        vec3 radiance = lightColors[i] * attenuation;

        // BRDF �� ˫����ֲ�����
        // ����������;��淴���������֣�kd * fd + ks * fs
        // kd + ks = 1 ;  fd = color / PI
        // fs = DFG / 4 * (ViewDir * Normal) * (Light * Normal)
        // DFG ���� ���߷ֲ����� DistributionGGX() �� ���κ��� GeometrySmith() �� ���������� fresnelSchlick()
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
        
        // ������㹫ʽ�� ks * ( DFG / 4 * (ViewDir * Normal) * (Light * Normal) )
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // ������ kS = F
        vec3 kS = F;
        
        // ������ kd = 1 - ks
        vec3 kD = vec3(1.0) - kS;
        
        // ��1 - metallic�� = �ֲڶ�
        kD *= 1.0 - metallic;	  

        // �����뷨�ߵļн�
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    // diffuse
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    // vec3 ambient = vec3(0.002);

    vec3 color = ambient + Lo;

    if(!IsIrradianceMap){
        color = (kD * diffuse) * ao + Lo;
    }

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}