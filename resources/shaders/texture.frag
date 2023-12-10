#version 330 core

in vec3 uvCoordinates;
uniform sampler2D sampler;
uniform bool perPixel;
uniform bool kernel;

out vec4 fragColor;

void main()
{
    fragColor = texture(sampler, vec2(uvCoordinates));
    if (perPixel && !kernel) {
        float greyValue = 0.299 * fragColor.x + 0.587 * fragColor.y + 0.114 * fragColor.z;
        fragColor = vec4(greyValue, greyValue, greyValue, 1.0);
    }
    else if (kernel) {
        float blurKernel[25] = float[25](
            1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f,
            1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f,
            1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f,
            1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f,
            1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f
        );

        fragColor = vec4(0.0f);
        ivec2 texSize = textureSize(sampler, 0);

        for (int i = -2; i <= 2; i++) {
            for (int j = -2; j <= 2; j++) {
                vec2 offset = vec2(i, j) / texSize;

                vec4 intermColor = texture(sampler, vec2(uvCoordinates) + offset);
                if (perPixel) {
                    float greyValue = 0.299 * intermColor.x + 0.587 * intermColor.y + 0.114 * intermColor.z;
                    intermColor = vec4(greyValue, greyValue, greyValue, 1.0);
                }

                fragColor += intermColor * blurKernel[(i + 2) * 5 + j + 2];
            }
        }

    }
}
