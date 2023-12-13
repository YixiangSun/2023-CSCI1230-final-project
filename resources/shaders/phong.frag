#version 330 core

in vec3 position;
in vec3 normal;
in vec2 uv;
//in vec4 lightSpacePos; // required only for shadow mapping (spot/directional light)
out vec4 fragColor;

uniform float blend;
uniform float anger;

//uniform sampler2D gShadowMap;        // required only for shadow mapping (spot/directional light)

uniform vec4 cAmbient;
uniform vec4 cDiffuse;
uniform vec4 cSpecular;
uniform float shininess;
uniform vec4 cameraPos;

uniform bool fireOn; // ???

uniform int numLights; // ???
uniform int lightTypes[8];
uniform bool isFires[8];
uniform vec4 lightDirs[8];
uniform vec4 lightPoses[8];
uniform vec4 lightColors[8];
uniform vec3 functions[8];
uniform float angles[8];
uniform float penumbras[8];

uniform sampler2D sampleTexture;
uniform sampler2D angryTexture;

//float CalcShadowFactor() {
//    vec3 ProjCoords = lightSpacePos.xyz / lightSpacePos.w;
//    vec2 UVCoords;
//    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
//    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
//    float z = 0.5 * ProjCoords.z + 0.5;
//    float Depth = texture(gShadowMap, UVCoords).x;

//    float bias = 0.0025f;

//    if (Depth + bias < z) {
//        return 0.5f;
//    } else {
//        return 1.f;
//    }
//}

void main() {

    vec3 realNormal = normalize(normal);  // normalize normal vector for the interpolated ones

    vec4 phongColor = vec4(0.0);
     // Ambient term
    phongColor += cAmbient;

//    float shadowFactor = 1.f; // ??????????????????????????????????????
//    shadowFactor = CalcShadowFactor(); // ??????????????????????????????????????

    for (int i = 0; i < numLights; i++) {
        if (isFires[i] && !fireOn){
            continue;
        }
        vec4 lightColor = lightColors[i];

        // Directional light
        if (lightTypes[i] == 0) {
            vec4 lightDir = normalize(lightDirs[i]);
            vec4 r = normalize(reflect(lightDir, vec4(realNormal, 0.f)));

//            vec4 unshadowedColor = vec4(0, 0, 0, 0);
            // Diffusion term
            phongColor += cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor;
            // specular term
            shininess == 0 ? phongColor += cSpecular * lightColor :
                             phongColor += cSpecular * pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess)
                                                          * lightColor;

//            phongColor += unshadowedColor * shadowFactor;
        }

        else if (lightTypes[i] == 1) {  // Point light
            vec4 lightDir = normalize(vec4(position, 1.0f) - lightPoses[i]);
            vec4 r = normalize(reflect(lightDir, vec4(realNormal, 0.f)));
            float d = length(vec4(position, 1.0f) - lightPoses[i]);
            float att = min(1.0f, 1.0f / (functions[i][0] + functions[i][1] * d + functions[i][2] * d * d));

//            vec4 unshadowedColor = vec4(0, 0, 0, 0);
            // Diffusion term
            phongColor += att * cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor;
            // specular term
            shininess == 0 ? phongColor += att * cSpecular * lightColor :
                             phongColor += att * cSpecular * pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess)
                                                    * lightColor;  // specular term

//            phongColor += unshadowedColor * shadowFactor;
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

//            vec4 unshadowedColor = vec4(0, 0, 0, 0);
            // Diffusion term
            phongColor += att * cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor;
            // specular term
            shininess == 0 ? phongColor += att * cSpecular * lightColor :
                             phongColor += att * cSpecular * pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess)
                                                    * lightColor;  // specular term

//            phongColor += unshadowedColor * shadowFactor;
        }
    }

    fragColor = phongColor;
    vec4 textureColor = texture(sampleTexture, vec2(uv)) * (1 - anger) + texture(angryTexture, vec2(uv)) * anger;
    fragColor = mix(phongColor, textureColor, blend);
}
