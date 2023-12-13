#version 330 core

in vec3 position; // Good
in vec3 normal;   // Good'
in vec4 lightSpacePos; // required only for shadow mapping (spot/directional light)s

uniform sampler2D gShadowMap;        // required only for shadow mapping (spot/directional light)

out vec4 fragColor;

uniform vec4 cAmbient;   // Good
uniform vec4 cDiffuse;   // Good
uniform vec4 cSpecular;  // Good
uniform float shininess; // Good
uniform vec4 cameraPos;  // Good

uniform int numLights;
uniform int lightTypes[8];
uniform vec4 lightDirs[8];
uniform vec4 lightPoses[8];
uniform vec4 lightColors[8];
uniform vec3 functions[8];
uniform float angles[8];
uniform float penumbras[8];

float CalcShadowFactor() {
    vec3 ProjCoords = lightSpacePos.xyz / lightSpacePos.w;
    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = 0.5 * ProjCoords.z + 0.5;
    float Depth = texture(gShadowMap, UVCoords).x;

    float bias = 0.0025f;

    if (Depth + bias < z) {
        return 0.5f;
    } else {
        return 1.f;
    }
}

void main(void)
{
    vec3 realNormal = normalize(normal);
    vec4 phongColor = vec4(0);
    phongColor += cAmbient * 0.1f; // ????????????????????????????????????????????????? suppose IA is 0.25f
//    phongColor += vec4(normal, 0);

    float shadowFactor = 1.f; // ??????????????????????????????????????
    shadowFactor = CalcShadowFactor(); // ??????????????????????????????????????

    for (int i = 0; i < numLights; i++) {
        vec4 lightColor = lightColors[i];

        // Directional light
        if (lightTypes[i] == 0) {
            vec4 lightDir = normalize(lightDirs[i]);
            vec4 r = normalize(reflect(lightDir, vec4(realNormal, 0.f)));

            vec4 unshadowedColor = vec4(0, 0, 0, 0);
            // Diffusion term
            unshadowedColor += cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor;
            // specular term
            shininess == 0 ? unshadowedColor += cSpecular * lightColor :
                             unshadowedColor += cSpecular * pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess)
                                                          * lightColor;

            phongColor += unshadowedColor * shadowFactor;
        }

        else if (lightTypes[i] == 1) {  // Point light
            vec4 lightDir = normalize(vec4(position, 1.0f) - lightPoses[i]);
            vec4 r = normalize(reflect(lightDir, vec4(realNormal, 0.f)));
            float d = length(vec4(position, 1.0f) - lightPoses[i]);
            float att = min(1.0f, 1.0f / (functions[i][0] + functions[i][1] * d + functions[i][2] * d * d));

            vec4 unshadowedColor = vec4(0, 0, 0, 0);
            // Diffusion term
            unshadowedColor += att * cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor;
            // specular term
            shininess == 0 ? unshadowedColor += att * cSpecular * lightColor :
                             unshadowedColor += att * cSpecular * pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess)
                                                    * lightColor;  // specular term

            phongColor += unshadowedColor * shadowFactor;
        }

        else if (lightTypes[i] == 2){  // spot light
            vec4 lightDir = normalize(vec4(position, 1.0f) - lightPoses[i]);
            vec4 r = normalize(reflect(lightDir, vec4(realNormal, 0.f)));
            float theta = acos(dot(lightDir, normalize(lightDirs[i])));
            float d = length(vec4(position, 1.0f) - lightPoses[i]);
            float att = min(1.0f, 1.0f / (functions[i][0] + functions[i][1] * d + functions[i][2] * d * d));

            if (theta <= angles[i] && theta > angles[i] - penumbras[i]) {
                att *= (1 + 2 * pow((theta - angles[i] + penumbras[i])/(penumbras[i]), 3)
                        - 3 * pow((theta - angles[i] + penumbras[i])/(penumbras[i]), 2));
            }
            else if (theta > angles[i]) att = 0;

            vec4 unshadowedColor = vec4(0, 0, 0, 0);
            // Diffusion term
            unshadowedColor += att * cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor;
            // specular term
            shininess == 0 ? unshadowedColor += att * cSpecular * lightColor :
                             unshadowedColor += att * cSpecular * pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess)
                                                    * lightColor;  // specular term

            phongColor += unshadowedColor * shadowFactor;
        }
    }

    fragColor = phongColor;
//    fragColor = lightSpacePos;  // !!
//    fragColor = vec4(shadowFactor, 0, 0, 0);
//    fragColor = vec4(1, 0, 0, 0);
}
