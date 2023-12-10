#version 330 core

in vec3 uvCoordinates;
uniform sampler2D sampler;
uniform bool perPixel;
uniform bool kernel;

out vec4 fragColor;

void main()
{
    fragColor = texture(sampler, vec2(uvCoordinates));
    if (fragColor.x <= 0.005 && fragColor.y <= 0.005 && fragColor.z <= 0.005) {
        fragColor == vec4(0.0, 0.0, 0.545, 1.0);
    }
}
