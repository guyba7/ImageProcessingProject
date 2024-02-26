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

    string GetEffectDisplayName() const override
    {
        return "Blur";
    }

    string GetEffectFileSuffix() const override
    {
        return "blur";
    }

protected:
    LPCWSTR GetPixelShaderFileName() override
    {
        return L"shaders/BlurPixelShader.hlsl";
    }
};

class ColorInversionEffect : public BaseEffect {
public:

    string GetEffectDisplayName() const override
    {
        return "Color Inversion";
    }

    string GetEffectFileSuffix() const override
    {
        return "inverted";
    }

protected:
    LPCWSTR GetPixelShaderFileName() override
    {
        return L"shaders/ColorInversionPixelShader.hlsl";
    }
};

class MirrorEffect : public BaseEffect {
public:

    string GetEffectDisplayName() const override
    {
        return "Mirror";
    }

    string GetEffectFileSuffix() const override
    {
        return "mirror";
    }

protected:
    LPCWSTR GetVertexShaderFileName() override
    {
        return L"shaders/MirrorVertexShader.hlsl";
    }
};

class ShrinkEffect : public BaseEffect {
public:

    string GetEffectDisplayName() const override
    {
        return "Shrink";
    }

    string GetEffectFileSuffix() const override
    {
        return "shrink";
    }

protected:
    LPCWSTR GetVertexShaderFileName() override
    {
        return L"shaders/ShrinkVertexShader.hlsl";
    }
};

class FishEyeEffect : public BaseEffect {
public:

    string GetEffectDisplayName() const override
    {
        return "Fish-Eye Effect";
    }

    string GetEffectFileSuffix() const override
    {
        return "fish_eye";
    }

protected:
    LPCWSTR GetVertexShaderFileName() override
    {
        return L"shaders/FishEyeVertexShader.hlsl";
    }
};

class EdgeDetectionEffect : public BaseEffect {
public:

    string GetEffectDisplayName() const override
    {
        return "Edge Detection";
    }

    string GetEffectFileSuffix() const override
    {
        return "edges";
    }

protected:
    LPCWSTR GetPixelShaderFileName() override
    {
        return L"shaders/EdgeDetectionPixelShader.hlsl";
    }
};

class EqualizationEffect : public BaseEffect {
public:

    string GetEffectDisplayName() const override
    {
        return "Equalization";
    }

    string GetEffectFileSuffix() const override
    {
        return "equalize";
    }

protected:
    LPCWSTR GetPixelShaderFileName() override
    {
        return L"shaders/EqualizationPixelShader.hlsl";
    }
};