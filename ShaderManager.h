#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream> // Include this for std::cout and std::cin
#include <windows.h> // Ensure you have this for GetConsoleWindow()
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using std::string;  // Make string available as 'string'

/**
 * ShaderManager is responsible for managing shaders and applying them to image data.
 * It handles the initialization of the Direct3D environment, creation of shaders,
 * and applying these shaders to perform image effects.
 */
class ShaderManager {
public:
    /**
     * Initializes the Shader Manager.
     *
     * @param out_error A pointer to a string to receive error messages, if any.
     * @return true if initialization succeeds, false otherwise.
     */
    bool initalizeShaderManager(string* out_error);

    /**
     * Creates vertex and pixel shaders from specified files.
     *
     * @param vertexShaderFile Path to the vertex shader file.
     * @param pixelShaderFile Path to the pixel shader file.
     * @param vertexShader Pointer to the created vertex shader.
     * @param pixelShader Pointer to the created pixel shader.
     * @param out_error A pointer to a string to receive error messages, if any.
     * @return true if the shaders are created successfully, false otherwise.
     */
    bool createShadersFromFiles(LPCWSTR vertexShaderFile, LPCWSTR pixelShaderFile, ID3D11VertexShader** vertexShader, ID3D11PixelShader** pixelShader, string* out_error);

    /**
     * Applies a shader to the given image data.
     *
     * @param imageData Pointer to the image data.
     * @param width Width of the image.
     * @param height Height of the image.
     * @param outputTexture The texture containing the processed image.
     * @param pixelShader The pixel shader to apply.
     * @param vertexShader The vertex shader to apply.
     * @param out_error A pointer to a string to receive error messages, if any.
     * @return true if the shader is applied successfully, false otherwise.
     */
    bool applyShaderOnImageData(unsigned char* imageData, int width, int height, int channels, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, string* out_error);

private:
    /**
     * Initializes the viewport for rendering.
     *
     * @param width Width of the viewport.
     * @param height Height of the viewport.
     */
    void initializeViewport(int width, int height);

    /**
     * Initializes the vertices for the quad to render the image.
     *
     * @param out_error A pointer to a string to receive error messages, if any.
     * @return true if vertices are initialized successfully, false otherwise.
     */
    bool initializeVertices(string* out_error);

    /**
     * Creates 2D textures for the image and shader processing.
     *
     * @param imageData Pointer to the image data.
     * @param width Width of the image.
     * @param height Height of the image.
     * @param out_sourceTexture Pointer to the input texture created.
     * @param out_renderTargetTexture Pointer to the output texture created.
     * @param out_stagingTexture Pointer to an texture that can be read by the CPU
     * @param out_error A pointer to a string to receive error messages, if any.
     * @return true if textures are created successfully, false otherwise.
     */
    bool create2DTextures(unsigned char* imageData, int width, int height, ID3D11Texture2D** out_sourceTexture, ID3D11Texture2D** out_renderTargetTexture, ID3D11Texture2D** out_stagingTexture, string* out_error);

    /**
     * Copies the rendered image from the render target to the image data buffer.
     *
     * @param imageData Pointer to the image data buffer to receive the processed image.
     * @param width Width of the image.
     * @param height Height of the image.
     * @param channels The length of a single pixel size in bytes
     * @param outputTexture The texture containing the processed image.
     * @param intermediateTexture An intermediate texture used in processing, if applicable.
     * @param errorMessage A pointer to a string to receive error messages, if any.
     * @return true if the image data is successfully copied, false otherwise.
     */
    bool copyRenderTargetToImageData(unsigned char* imageData, int width, int height, int channels, ID3D11Texture2D* outputTexture, ID3D11Texture2D* intermediateTexture, string* errorMessage);

    /**
     * Releases all Direct3D resources held by the ShaderManager.
     */
    void releaseAllD3DMembers();
};
