#include "Effect.h"

bool BaseEffect::ApplyEffectFromRawImageData(unsigned char* imageData, int width, int height, int channels, ShaderManager* shaderManagerRef, string* out_error)
{
    if (!shaderManagerRef) 
    {
        *out_error = "ShaderManager reference is null";
        std::cout << "ShaderManager reference is null";
        return false;
    }
        

    // initialize shader if not initialized (lazy loading)
    if (!m_areShadersInitialized)
    {
        LPCWSTR pixelShaderFile = GetPixelShaderFileName();
        LPCWSTR vertexShaderFile = GetVertexShaderFileName();
        if (!shaderManagerRef->createShadersFromFiles(pixelShaderFile, vertexShaderFile, &m_vertexShader, &m_pixelShader, out_error))
            return false;
    }

    // check shaders valid
    if (!m_vertexShader || !m_pixelShader)
    {
        *out_error = "Effect's shaders are invalid";
        std::cout << "Effect's shaders are invalid";
        return false;
    }

    // apply the shader effect on imageData using the ShaderManager's GPU API
    if (!shaderManagerRef->applyShaderOnImageData(imageData, width, height, channels, m_pixelShader, m_vertexShader, out_error))
        return false;

    return true;
}
