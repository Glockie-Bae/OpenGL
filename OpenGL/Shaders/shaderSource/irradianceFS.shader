#version 330 core

in vec3 localPos;
out vec4 FragColor;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main(){
	vec3 normal = normalize(localPos);
	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
    up         = normalize(cross(normal, right));

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for(float phi = 0.0; phi < 2 * PI; phi += sampleDelta){
		for(float theta = 0.0; theta < PI * 0.5; theta += sampleDelta){

			// 极坐标方程 ： x = r * sin(theta) * cos(phi)   y = r * sin(theta) * sin(phi)   z = cos(theta)
			// 光照方程 ：L_irradiance * cos(theta) * sin(theta)  
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 
			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	FragColor = vec4(irradiance, 1.0);
}