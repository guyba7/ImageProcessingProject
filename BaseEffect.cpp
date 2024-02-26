#include "BaseEffect.h"

bool BaseEffect::ApplyEffectFromRawImageData(unsigned char* imageData, int width, int height, int channels, ShaderManager* shaderManagerRef, string* out_error)
{
    if (!shaderManagerRef)
        return false;

    // initialize shader if not initialized (lazy loading)
    if (!m_areShadersInitialized)
    {
        LPCWSTR pixelShaderFile = L"shaders/DefaultPixelShader.hlsl";
        LPCWSTR vertexShaderFile = L"shaders/DefaultVertexShader.hlsl";
        if (!shaderManagerRef->createShadersFromFiles(pixelShaderFile, vertexShaderFile, m_vertexShader, m_pixelShader, out_error))
            return false;
    }

    // check shaders valid
    if (!m_vertexShader || !m_pixelShader)
    {
        out_error = new string("Effect's shaders are invalid");
        return false;
    }

    // apply the shader effect on imageData using the ShaderManager's GPU API
    if (!shaderManagerRef->applyShaderOnImageData(imageData, width, height, channels, m_pixelShader, m_vertexShader, out_error))
        return false;

    return true;
}
