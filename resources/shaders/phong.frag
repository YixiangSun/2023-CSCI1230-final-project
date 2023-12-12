#version 330 core

in vec3 position;
in vec3 normal;
in vec2 uv;
out vec4 fragColor;

uniform float blend;
uniform float anger;

//uniform float k_a;
//uniform float k_d;
//uniform float k_s;
//uniform mat4 viewMatrix;
//uniform mat4 modelmatrix;

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

void main() {

    vec3 realNormal = normalize(normal);  // normalize normal vector for the interpolated ones

    vec4 phongColor = vec4(0.0);
//    phongColor += k_a * cAmbient;  // Ambient term
    phongColor += cAmbient;

    for (int i = 0; i < numLights; i++) {
        if (isFires[i] && !fireOn){
            continue;
        }
        vec4 lightColor = lightColors[i];

        if (lightTypes[i] == 0) { // Directional light
            vec4 lightDir = normalize(lightDirs[i]);
            vec4 r = normalize(reflect(lightDir, vec4(realNormal, 0.f)));

//            phongColor += k_d * cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor; // Diffusion term
            phongColor += cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor;
//            shininess == 0 ? phongColor += k_s * cSpecular * lightColor :
//                    phongColor += k_s * cSpecular *
//                    pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess) * lightColor;  // specular term
            shininess == 0 ? phongColor += cSpecular * lightColor :
                             phongColor += cSpecular * pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess)
                                                     * lightColor;
        }

        else if (lightTypes[i] == 1) {  // Point light
            vec4 lightDir = normalize(vec4(position, 1.0f) - lightPoses[i]);
            vec4 r = normalize(reflect(lightDir, vec4(realNormal, 0.f)));
            float d = length(vec4(position, 1.0f) - lightPoses[i]);
            float att = min(1.0f, 1.0f / (functions[i][0] + functions[i][1] * d + functions[i][2] * d * d));

//            phongColor += att * k_d * cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor; // Diffusion term
            phongColor += att * cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor;
//            shininess == 0 ? phongColor += att * k_s * cSpecular * lightColor :
//                    phongColor += att * k_s * cSpecular *
//                    pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess) * lightColor;  // specular term
            shininess == 0 ? phongColor += att * cSpecular * lightColor :
                             phongColor += att * cSpecular * pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess)
                                               * lightColor;  // specular term
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

//            phongColor += att * k_d * cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor;  // Diffusion term
            phongColor += att * cDiffuse * max(0.0, dot(realNormal, -vec3(lightDir))) * lightColor;
//            shininess == 0 ? phongColor += att * k_s * cSpecular * lightColor :
//                    phongColor += att * k_s * cSpecular *
//                    pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess) * lightColor;  // specular term
            shininess == 0 ? phongColor += att * cSpecular * lightColor :
                             phongColor += att * cSpecular * pow(max(0, dot(vec3(r), normalize(vec3(cameraPos) - position))), shininess)
                                               * lightColor;  // specular term
        }
    }

    fragColor = phongColor;
    vec4 textureColor = texture(sampleTexture, vec2(uv)) * (1 - anger) + texture(angryTexture, vec2(uv)) * anger;
    fragColor = mix(phongColor, textureColor, blend);
}
