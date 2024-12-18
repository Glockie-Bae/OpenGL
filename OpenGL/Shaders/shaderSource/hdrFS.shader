#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool IsHDR;
uniform float exposure;

void main()
{
    if(IsHDR){
        const float gamma = 2.2;
        vec3 hdrColor = texture(screenTexture, TexCoords).rgb;

        // 曝光色调映射
        vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
        // Gamma校正 
        mapped = pow(mapped, vec3(1.0 / gamma));

        FragColor = vec4(mapped, 1.0);
    }
    else{
        FragColor = texture(screenTexture, TexCoords);
    }
    
} 