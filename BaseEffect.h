#pragma once
#include "ShaderManager.h"
#include <iostream>

using std::string;  // Make string available as 'string'
 
class BaseEffect {
public:
    virtual ~BaseEffect() {}

    bool m_areShadersInitialized = false;
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;

    // Returns the display name of the effect
    virtual string GetEffectDisplayName() const = 0;

    // Returns the file suffix for the effect
    virtual string GetEffectFileSuffix() const = 0;

    virtual string GetFragmentShaderSource() const = 0;

    bool ApplyEffectFromRawImageData(unsigned char* imageData, int width, int height, int channels, ShaderManager* shaderManagerRef, string* out_error);
};

class BlurEffect : public BaseEffect {
public:
    string GetEffectDisplayName() const override {
        return "Blur Effect";
    }

    string GetEffectFileSuffix() const override {
        return "blur";
    }

    string GetFragmentShaderSource() const override {
        // Return the GLSL source code for the blur effect shader
        // Placeholder shader source; you should replace it with actual GLSL code for blurring
        return R"glsl(
            #version 330 core
            out vec4 FragColor;
            in vec2 TexCoords;
            uniform sampler2D textureSampler;

            void main() {
                // Implement blur logic here
                FragColor = texture(textureSampler, TexCoords);
            }
        )glsl";
    }
};

class ColorInversionEffect : public BaseEffect {
public:
    string GetEffectDisplayName() const override {
        return "Color Inversion Effect";
    }

    string GetEffectFileSuffix() const override {
        return "inverted";
    }

    string GetFragmentShaderSource() const override {
        // Return the GLSL source code for the color inversion shader
        return R"glsl(
            #version 330 core
            out vec4 FragColor;
            in vec2 TexCoords;
            uniform sampler2D textureSampler;

            void main() {
                vec4 texColor = texture(textureSampler, TexCoords);
                FragColor = vec4(1.0 - texColor.rgb, texColor.a); // Invert the color
            }
        )glsl";
    }
};
