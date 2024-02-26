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

    bool ApplyEffectFromRawImageData(unsigned char* imageData, int width, int height, int channels, ShaderManager* shaderManagerRef, string* out_error);

protected:

    // Returns the file of the effect's pixelshader
    virtual LPCWSTR GetPixelShaderFileName()
    {
        return L"shaders/DefaultPixelShader.hlsl";
    }

    // Returns the file of the effect's pixelshader
    virtual LPCWSTR GetVertexShaderFileName()
    {
        return L"shaders/DefaultVertexShader.hlsl";
    }
};

class BlurEffect : public BaseEffect {
public:
    string GetEffectDisplayName() const override; 
    string GetEffectFileSuffix() const override; 
};

class ColorInversionEffect : public BaseEffect {
public:
    string GetEffectDisplayName() const override;
    string GetEffectFileSuffix() const override;

protected:
    LPCWSTR GetPixelShaderFileName() override
    {
        return L"shaders/ColorInversionPixelShader.hlsl";
    }
};
