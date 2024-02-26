#include "ShaderManager.h"

// Define vertex structure with texture coordinates
struct Vertex
{
    DirectX::XMFLOAT4 position; // Vertex position
    DirectX::XMFLOAT2 texCoord; // Texture coordinate
};

// Define quad vertices with positions and texture coordinates
const Vertex RECT_QUAD_VERTICES[] =
{
    { DirectX::XMFLOAT4(-1.0f, 1.0f, 0.0f,1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) }, // Top Left
    { DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f,1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }, // Top Right
    { DirectX::XMFLOAT4(-1.0f, -1.0f, 0.0f,1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) }, // Bottom Left
    { DirectX::XMFLOAT4(1.0f, -1.0f, 0.0f,1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }, // Bottom Right
};


// Define vertex shader input layout
D3D11_INPUT_ELEMENT_DESC inputLayout[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

bool m_isManagerInitialized = false;

ID3D11Device* m_device = nullptr;
ID3D11DeviceContext* m_deviceContext = nullptr;
D3D_FEATURE_LEVEL m_deviceFeatureLevel;


bool ShaderManager::initalizeShaderManager(string* out_error)
{
    HRESULT hr;

    // Create a Direct3D device and a device context.
    hr = D3D11CreateDevice(
        nullptr,                    // Specifies NULL to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,   // Use hardware acceleration if available.
        nullptr,                    // No software rasterizer.
        D3D11_CREATE_DEVICE_DEBUG,  // Enable debug layer.
        nullptr,                    // An array of feature levels, NULL means all supported feature levels.
        0,                          // The size of the feature level array. 0 if nullptr is passed.
        D3D11_SDK_VERSION,          // The SDK version; always set to D3D11_SDK_VERSION.
        &m_device,                  // Address of a pointer to the created device.
        &m_deviceFeatureLevel,      // The first feature level that is successfully created.
        &m_deviceContext);          // Address of a pointer to the device context.


    // Check for failure in device creation.
    if (FAILED(hr)) 
    {
        out_error = new string("Failed to create Direct3D device.");
        releaseAllD3DMembers();
        return false;
    }
    

    // init full-screen 2D rectangle vertices (all effects use the same vertices
    if (!initializeVertices(out_error))
        return false;
    
    m_isManagerInitialized = true;
    return true;
}

bool ShaderManager::initializeVertices(string* out_error)
{
    HRESULT hr;

    // Create vertex buffer decription
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(RECT_QUAD_VERTICES);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    // Set initial data as a full-screen 2D rectangle
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = RECT_QUAD_VERTICES;

    // create a buffer for the vertices
    ID3D11Buffer* vertexBuffer = nullptr;
    hr = m_device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);

    // Check for failure in device creation.
    if (FAILED(hr)) {
        out_error = new string("Failed to create vertex buffer.");
        releaseAllD3DMembers();
        return false;
    }

    // Bind vertex buffer to context
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    return true;
}

bool ShaderManager::createShadersFromFiles(LPCWSTR pixelShaderFileName, LPCWSTR vertexShaderFileName, ID3D11VertexShader** out_vertexShader, ID3D11PixelShader** out_pixelShader, string* out_error)
{
    HRESULT hr;
    ID3DBlob* errorBlob = nullptr;

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;

    //////////////////////// Create Vertex Shader ////////////////////////

    ID3DBlob* vertexShaderCompiledCodeBlock = nullptr;
    ID3D11InputLayout* pVertexInputLayout = nullptr;

    // Compile vertex shader from file
    hr = D3DCompileFromFile(
        vertexShaderFileName, // Updated Filename
        nullptr,                    // Optional macros
        nullptr,                    // Optional include files
        "main",                   // Entry point
        "vs_5_0",                   // Target shader model
        0,                          // Shader compile options
        0,                          // Effect compile options
        &vertexShaderCompiledCodeBlock,           // Double pointer to the compiled shader
        &errorBlob);                // Pointer to a blob that stores all compile errors and warnings

    // Check for errors
    if (FAILED(hr)) 
    {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }

        if (vertexShaderCompiledCodeBlock) vertexShaderCompiledCodeBlock->Release();

        out_error = new string("Failed to compile vertex shader.");
        return false;
    }

    // create vertex input layout
    hr = m_device->CreateInputLayout(
        inputLayout, // Pointer to an array of D3D11_INPUT_ELEMENT_DESC structures that describe the elements of the vertex input layout.
        ARRAYSIZE(inputLayout), // Number of elements in the inputLayout array. This tells Direct3D how many input elements are being defined.
        vertexShaderCompiledCodeBlock->GetBufferPointer(), // Pointer to the compiled vertex shader bytecode. This includes the input signature that the shader expects.
        vertexShaderCompiledCodeBlock->GetBufferSize(), // Size of the shader bytecode. This helps Direct3D verify that the input layout matches the shader's input signature.
        &pVertexInputLayout // Address of a pointer to an ID3D11InputLayout interface. On successful execution, this will point to the created input layout object.
    );
    if (FAILED(hr)) {
        if (vertexShaderCompiledCodeBlock) vertexShaderCompiledCodeBlock->Release();

        out_error = new string("Failed to create vertex shader input layout.");
        return false;
    }

    // Create the vertex shader object from the compiled shader bytecode
    hr = m_device->CreateVertexShader(
        vertexShaderCompiledCodeBlock->GetBufferPointer(),
        vertexShaderCompiledCodeBlock->GetBufferSize(),
        nullptr,
        &vertexShader);

    if (FAILED(hr)) 
    {
        if (vertexShaderCompiledCodeBlock) vertexShaderCompiledCodeBlock->Release();
        if (pVertexInputLayout) pVertexInputLayout->Release();
        if (vertexShader) vertexShader->Release();

        out_error = new string("Failed to create vertex shader.");
        return false;
    }

    //////////////////////// Create Pixel Shader ////////////////////////

    ID3DBlob* pixelShaderCompiledCodeBlock = nullptr;

    hr = D3DCompileFromFile(
        pixelShaderFileName, // Updated Filename
        nullptr,                    // Optional macros
        nullptr,                    // Optional include files
        "main",                   // Entry point
        "ps_5_0",                   // Target shader model
        0,                          // Shader compile options
        0,                          // Effect compile options
        &pixelShaderCompiledCodeBlock,           // Double pointer to the compiled shader
        &errorBlob);                // Pointer to a blob that stores all compile errors and warnings

    if (FAILED(hr)) 
    {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }

        if (vertexShaderCompiledCodeBlock) vertexShaderCompiledCodeBlock->Release();
        if (pVertexInputLayout) pVertexInputLayout->Release();
        if (vertexShader) vertexShader->Release();
        if (pixelShaderCompiledCodeBlock) pixelShaderCompiledCodeBlock->Release();

        out_error = new string("Failed to compile pixel shader.");
        return false;
    }

    // Create the pixel shader object from the compiled shader bytecode
    hr = m_device->CreatePixelShader(
        pixelShaderCompiledCodeBlock->GetBufferPointer(),
        pixelShaderCompiledCodeBlock->GetBufferSize(),
        nullptr,
        &pixelShader);

    if (FAILED(hr)) 
    {
        if (vertexShaderCompiledCodeBlock) vertexShaderCompiledCodeBlock->Release();
        if (pVertexInputLayout) pVertexInputLayout->Release();
        if (vertexShader) vertexShader->Release();
        if (pixelShaderCompiledCodeBlock) pixelShaderCompiledCodeBlock->Release();
        if (pixelShader) pixelShader->Release();

        out_error = new string("Failed to create pixel shader.");
        return false;
    }

    //////////////////////////////////////////////////////////////////////

    // Bind vertex shader input layout to device context
    m_deviceContext->IASetInputLayout(pVertexInputLayout);

    if (pixelShaderCompiledCodeBlock) pixelShaderCompiledCodeBlock->Release();
    if (vertexShaderCompiledCodeBlock) vertexShaderCompiledCodeBlock->Release();

    *out_pixelShader = pixelShader;
    *out_vertexShader = vertexShader;

    return true;
}

bool ShaderManager::applyShaderOnImageData(unsigned char* imageData, int width, int height, int channels, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, string* out_error)
{
    HRESULT hr;

    if (!m_isManagerInitialized)
    {
        // initialize shader manager
        if (!initalizeShaderManager(out_error))
            return false; // init falied
    }

    // init render viewport to size of texture
    initializeViewport(width, height);

    ID3D11Texture2D* sourceTexture = nullptr;
    ID3D11Texture2D* renderTargetTexture = nullptr;
    ID3D11Texture2D* stagingTexture = nullptr;

    // create the 2D textures required to apply effect
    if (!create2DTextures(imageData, width, height, &sourceTexture, &renderTargetTexture, &stagingTexture, out_error))
    {
        releaseAllD3DMembers();
        return false;
    }

    // apply source texture as shader resource view of context
    ID3D11ShaderResourceView* sourceTextureView = nullptr;
    hr = m_device->CreateShaderResourceView(sourceTexture, nullptr, &sourceTextureView);
    if (FAILED(hr)) 
    {
        if (sourceTexture) sourceTexture->Release();
        if (renderTargetTexture) renderTargetTexture->Release();
        if (stagingTexture) stagingTexture->Release();
        releaseAllD3DMembers();
        return false;
    }
    ID3D11ShaderResourceView* views[] = { sourceTextureView };
    m_deviceContext->PSSetShaderResources(0, 1, views);

    // apply render target as render target view of context
    ID3D11RenderTargetView* renderTargetView = nullptr;
    hr = m_device->CreateRenderTargetView(renderTargetTexture, nullptr, &renderTargetView);
    if (FAILED(hr)) 
    {
        if (sourceTexture) sourceTexture->Release();
        if (renderTargetTexture) renderTargetTexture->Release();
        if (stagingTexture) stagingTexture->Release();
        releaseAllD3DMembers();
        return false;
    }
    m_deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr); // Assuming no depth stencil


    // apply shaders
    m_deviceContext->PSSetShader(pixelShader, nullptr, 0);
    m_deviceContext->VSSetShader(vertexShader, nullptr, 0);

    // draw shader onto render target texture
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_deviceContext->Draw(4, 0); // Drawing 4 vertices, using a triangle strip to form a quad.
   // sizeof(RECT_QUAD_VERTICES);

    // copy render target texture to staging texture and then override imageData data block with rendered pixels
    if (!copyRenderTargetToImageData(imageData, width, height, channels, renderTargetTexture, stagingTexture, out_error))
    {
        if (sourceTexture) sourceTexture->Release();
        if (renderTargetTexture) renderTargetTexture->Release();
        if (stagingTexture) stagingTexture->Release();

        releaseAllD3DMembers();
        return false;
    }


    if (sourceTexture) sourceTexture->Release();
    if (renderTargetTexture) renderTargetTexture->Release();
    if (stagingTexture) stagingTexture->Release();

    return true;
}

void ShaderManager::initializeViewport(int width, int height)
{
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_deviceContext->RSSetViewports(1, &viewport);
}

bool ShaderManager::copyRenderTargetToImageData(unsigned char* targetImageDataBlock, int width, int height, int channels, ID3D11Texture2D* renderTargetTexture, ID3D11Texture2D* stagingTexture, string* out_error)
{
    HRESULT hr;
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    // Copy the processed texture to the staging texture
    m_deviceContext->CopyResource(stagingTexture, renderTargetTexture);

    // Map the staging texture to gain read access on CPU
    hr = m_deviceContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);
    if (FAILED(hr))
    {
        out_error = new string("Failed to map staging texture.");
        return false;
    }
    
    // Set targetImageDataBlock as initial destination row for the copying process
    unsigned char* destRow = targetImageDataBlock;

    // Set the mapped staging texture data as initial source row for the copying process
    const unsigned char* sourceRow = reinterpret_cast<const unsigned char*>(mappedResource.pData);

    size_t rowWidth = width * channels; // Assuming 4 channels (RGBA) for simplicity
    
    // memory copy each row according to the texture
    for (int i = 0; i < height ; ++i) {
        memcpy(destRow, sourceRow, rowWidth);

        destRow += rowWidth;
        sourceRow += mappedResource.RowPitch; // Use RowPitch to move to the next row in the source data
    }

    return true;
}

bool ShaderManager::create2DTextures(unsigned char* imageData, int width, int height, ID3D11Texture2D** out_sourceTexture, ID3D11Texture2D** out_renderTargetTexture, ID3D11Texture2D** out_stagingTexture, string* out_error)
{
    HRESULT hr;

    ID3D11Texture2D* sourceTexture = nullptr;
    ID3D11Texture2D* renderTargetTexture = nullptr;
    ID3D11Texture2D* stagingTexture = nullptr;

    // Describtion of all textures
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width; // Texture width.
    textureDesc.Height = height; // Texture height.
    textureDesc.MipLevels = 1; // Number of mipmap levels.
    textureDesc.ArraySize = 1; // Number of textures in the texture array.
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of the texture.
    textureDesc.SampleDesc.Count = 1; // The number of multisamples per pixel.
    textureDesc.Usage = D3D11_USAGE_DEFAULT; // Specify how the texture is to be read from and written to.
    

    D3D11_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.SysMemPitch = width * 4; // The distance (in bytes) from the beginning of one line of a texture to the next line.

    //////////////////////// Create Source Texture ////////////////////////

    // Provide initial data to populate the source texture.
    subResourceData.pSysMem = imageData; // Pointer to the initialization data.
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind the texture to the shader resource.

    // Create the source texture from the provided data.
    hr = m_device->CreateTexture2D(
        &textureDesc,      // Pointer to a D3D11_TEXTURE2D_DESC structure that describes the texture.
        &subResourceData,  // Pointer to an array of D3D11_SUBRESOURCE_DATA structures that describe subresources for the 2D texture. This parameter can be NULL if you do not need to initialize the texture with data upon creation.
        &sourceTexture);         // Address of a pointer to an ID3D11Texture2D interface that represents the created texture.

    if (FAILED(hr)) 
    {
        if (sourceTexture) sourceTexture->Release();

        out_error = new string("Failed to create source texture from image data.");
        return false;
    }

    //////////////////////// Create Render Target Texture ////////////////////////

    // Configuration for render target texture
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

    hr = m_device->CreateTexture2D(
        &textureDesc,      // Pointer to a D3D11_TEXTURE2D_DESC structure that describes the texture.
        &subResourceData,  // Pointer to an array of D3D11_SUBRESOURCE_DATA structures that describe subresources for the 2D texture. This parameter can be NULL if you do not need to initialize the texture with data upon creation.
        &renderTargetTexture);         // Address of a pointer to an ID3D11Texture2D interface that represents the created texture.

    if (FAILED(hr)) {
        if (sourceTexture) sourceTexture->Release();
        if (renderTargetTexture) renderTargetTexture->Release();

        out_error = new string("Failed to create render target texture.");
        return false;
    }

    //////////////////////// Create Staging Texture ////////////////////////

    // Configuration for staging texture
    textureDesc.Usage = D3D11_USAGE_STAGING; // Specify how the texture allows data transfer between GPU and CPU.
    textureDesc.BindFlags = 0; // No binding
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // Allows the CPU to access the data

    hr = m_device->CreateTexture2D(
        &textureDesc,      // Pointer to a D3D11_TEXTURE2D_DESC structure that describes the texture.
        &subResourceData,  // Pointer to an array of D3D11_SUBRESOURCE_DATA structures that describe subresources for the 2D texture. This parameter can be NULL if you do not need to initialize the texture with data upon creation.
        &stagingTexture);         // Address of a pointer to an ID3D11Texture2D interface that represents the created texture.

    if (FAILED(hr)) {
        if (sourceTexture) sourceTexture->Release();
        if (renderTargetTexture) renderTargetTexture->Release();
        if (stagingTexture) stagingTexture->Release();

        out_error = new string("Failed to create staging texture.");
        return false;
    }

    *out_sourceTexture = sourceTexture;
    *out_renderTargetTexture = renderTargetTexture;
    *out_stagingTexture = stagingTexture;

    return true;
}

void ShaderManager::releaseAllD3DMembers()
{
    if (m_deviceContext) m_deviceContext->Release();
    if (m_device) m_device->Release();

    m_isManagerInitialized = false;
}

bool ShaderManager::test3D(unsigned char* imageData, int width, int height, int channels) 
{

    // Initialize pointers for the Direct3D device, the device context, and the swap chain to null.
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    D3D_FEATURE_LEVEL featureLevel;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr;

    // Create a Direct3D 11 device and a device context.
    hr = D3D11CreateDevice(
        nullptr, // Specifies NULL to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE, // Use hardware acceleration if available.
        nullptr, // No software rasterizer.
        D3D11_CREATE_DEVICE_DEBUG, // No creation flags.
        nullptr, // An array of feature levels, NULL means all supported feature levels.
        0, // The size of the feature level array. 0 if nullptr is passed.
        D3D11_SDK_VERSION, // The SDK version; always set to D3D11_SDK_VERSION.
        &device, // Address of a pointer to the created device.
        &featureLevel, // The first feature level that is successfully created.
        &context); // Address of a pointer to the device context.

    // Check for failure in device creation.
    if (FAILED(hr)) {
        std::cerr << "Failed to create Direct3D device." << std::endl;
        return false;
    }

  //  std::cout << "Direct3D device created successfully." << std::endl;

    // Step 1: Texture Creation
    // Describe the texture to be created.
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width; // Texture width.
    textureDesc.Height = height; // Texture height.
    textureDesc.MipLevels = 1; // Number of mipmap levels.
    textureDesc.ArraySize = 1; // Number of textures in the texture array.
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of the texture.
    textureDesc.SampleDesc.Count = 1; // The number of multisamples per pixel.
    textureDesc.Usage = D3D11_USAGE_DEFAULT; // Specify how the texture is to be read from and written to.
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Bind the texture to the shader resource and render target. That way we can both read an write to the texture.

    // Provide initial data to populate the texture.
    D3D11_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pSysMem = imageData; // Pointer to the initialization data.
    subResourceData.SysMemPitch = width * 4; // The distance (in bytes) from the beginning of one line of a texture to the next line.

    // Create the texture from the provided data.
    ID3D11Texture2D* sourceTexture = nullptr;
    hr = device->CreateTexture2D(
        &textureDesc,      // Pointer to a D3D11_TEXTURE2D_DESC structure that describes the texture.
        &subResourceData,  // Pointer to an array of D3D11_SUBRESOURCE_DATA structures that describe subresources for the 2D texture. This parameter can be NULL if you do not need to initialize the texture with data upon creation.
        &sourceTexture);         // Address of a pointer to an ID3D11Texture2D interface that represents the created texture.


    // Check for failure in texture creation.
    if (FAILED(hr)) {
        std::cerr << "Failed to create texture from image data." << std::endl;
        device->Release();
        context->Release();
        return false;
    }

    ////////////// Render Target Texture ////////////////
        // Create the texture from the provided data.
    ID3D11Texture2D* renderTargeTexture = nullptr;

    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

    hr = device->CreateTexture2D(
        &textureDesc,      // Pointer to a D3D11_TEXTURE2D_DESC structure that describes the texture.
        &subResourceData,  // Pointer to an array of D3D11_SUBRESOURCE_DATA structures that describe subresources for the 2D texture. This parameter can be NULL if you do not need to initialize the texture with data upon creation.
        &renderTargeTexture);         // Address of a pointer to an ID3D11Texture2D interface that represents the created texture.


    // Check for failure in texture creation.
    if (FAILED(hr)) {
        std::cerr << "Failed to create texture from image data." << std::endl;
        device->Release();
        context->Release();
        return false;
    }

    ID3D11RenderTargetView* renderTargetView = nullptr;
    hr = device->CreateRenderTargetView(renderTargeTexture, nullptr, &renderTargetView);
    if (FAILED(hr)) {
        // Handle failure
        return false;
    }

    context->OMSetRenderTargets(1, &renderTargetView, nullptr); // Assuming no depth stencil

    // Step 2: Load and Compile Shader
    // This step would involve loading HLSL shader source, compiling it, and creating a pixel shader object.
    // For simplicity, this is omitted. You'd use D3DCompileFromFile or a similar function.

        /////////////////////////// vertex shader //////////////////////

    ID3D11VertexShader* vertexShader = nullptr;

    // Load and compile the vertex shader
    ID3DBlob* vertexShaderCodeBlock = nullptr;
    errorBlob = nullptr;
    hr = D3DCompileFromFile(
        L"shaders/DefaultVertexShader.hlsl", // Updated Filename
        nullptr,                    // Optional macros
        nullptr,                    // Optional include files
        "main",                   // Entry point
        "vs_5_0",                   // Target shader model
        0,                          // Shader compile options
        0,                          // Effect compile options
        &vertexShaderCodeBlock,           // Double pointer to the compiled shader
        &errorBlob);                // Pointer to a blob that stores all compile errors and warnings

    // Check for errors
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        if (vertexShaderCodeBlock) vertexShaderCodeBlock->Release();
        std::cerr << "Failed to compile vertex shader." << std::endl;
        return false;
    }

    // Create the vertex shader object from the compiled shader bytecode
    hr = device->CreateVertexShader(
        vertexShaderCodeBlock->GetBufferPointer(),
        vertexShaderCodeBlock->GetBufferSize(),
        nullptr,
        &vertexShader);

    if (FAILED(hr)) {
        vertexShaderCodeBlock->Release();
        std::cerr << "Failed to create vertex shader." << std::endl;
        return false;
    }

    /////////////////////////// pixel shader //////////////////////


    ID3D11PixelShader* pixelShader = nullptr;

    // Load and compile the pixel shader
    ID3DBlob* pixelShaderCodeBlock = nullptr;
    //errorBlob = nullptr;

    hr = D3DCompileFromFile(
        L"shaders/DefaultPixelShader.hlsl", // Updated Filename
        nullptr,                    // Optional macros
        nullptr,                    // Optional include files
        "main",                   // Entry point
        "ps_5_0",                   // Target shader model
        0,                          // Shader compile options
        0,                          // Effect compile options
        &pixelShaderCodeBlock,           // Double pointer to the compiled shader
        &errorBlob);                // Pointer to a blob that stores all compile errors and warnings

    // Check for errors
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        if (pixelShaderCodeBlock) pixelShaderCodeBlock->Release();
        std::cerr << "Failed to compile pixel shader." << std::endl;
        return false;
    }

    // Create the pixel shader object from the compiled shader bytecode
    hr = device->CreatePixelShader(
        pixelShaderCodeBlock->GetBufferPointer(),
        pixelShaderCodeBlock->GetBufferSize(),
        nullptr,
        &pixelShader);

    if (FAILED(hr)) {
        pixelShaderCodeBlock->Release();
        std::cerr << "Failed to create pixel shader." << std::endl;
        return false;
    }

    ////////////////////////////////////////////////////////////////





    ID3D11ShaderResourceView* sourceTextureView = nullptr;

    // Step 3.1: Rendering Setup Set Shader
   // Bind the texture as an input to the pixel shader.

    // Create the shader resource view for the texture
    hr = device->CreateShaderResourceView(sourceTexture, nullptr, &sourceTextureView);
    if (FAILED(hr)) {
        std::cerr << "Failed to create shader resource view." << std::endl;
        if (sourceTexture) sourceTexture->Release();
        if (pixelShader) pixelShader->Release();
        if (vertexShader) vertexShader->Release();
        return false;
    }

    ID3D11ShaderResourceView* views[] = { sourceTextureView };
    context->PSSetShaderResources(0, 1, views);

    // Set the pixel shader and shader resource view
    context->PSSetShader(pixelShader, nullptr, 0);
    
    

    context->VSSetShader(vertexShader, nullptr, 0);

    // Step 3.2 Rendering Setup set RenderTarget 




    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);
    
 
    // Step 4.1 create vertices (simple 2D quad)

    // Create vertex buffer
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
  //  bufferDesc.ByteWidth = sizeof(quadVertices);
    bufferDesc.ByteWidth = sizeof(RECT_QUAD_VERTICES);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
   // initData.pSysMem = quadVertices;
    initData.pSysMem = RECT_QUAD_VERTICES;

    ID3D11Buffer* vertexBuffer = nullptr;
    hr = device->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
    if (FAILED(hr)) {

        return false;
    }

    // Bind vertex buffer
   // UINT stride = sizeof(Vertex);
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);


// Define input layout
    D3D11_INPUT_ELEMENT_DESC inputLayout[] =
    {
        /*{
            "Position",                          // Semantic Name: Match the HLSL semantic name.
            0,                              // Semantic Index
            DXGI_FORMAT_R32G32_FLOAT,    // Format
            0,                              // Input Slot
            0,                              // Aligned Byte Offset
            D3D11_INPUT_PER_VERTEX_DATA,    // Input Slot Class
            0                               // Instance Data Step Rate
        },*/
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };


    UINT numElements = ARRAYSIZE(inputLayout);

    ID3D11InputLayout* pVertexLayout = nullptr;

    hr = device->CreateInputLayout(
        inputLayout, // Pointer to an array of D3D11_INPUT_ELEMENT_DESC structures that describe the elements of the vertex input layout.
        numElements, // Number of elements in the inputLayout array. This tells Direct3D how many input elements are being defined.
        vertexShaderCodeBlock->GetBufferPointer(), // Pointer to the compiled vertex shader bytecode. This includes the input signature that the shader expects.
        vertexShaderCodeBlock->GetBufferSize(), // Size of the shader bytecode. This helps Direct3D verify that the input layout matches the shader's input signature.
        &pVertexLayout // Address of a pointer to an ID3D11InputLayout interface. On successful execution, this will point to the created input layout object.
    );
    if (FAILED(hr)) {

        return false;
    }

    // Bind input layout
    context->IASetInputLayout(pVertexLayout);

    // Step 4.2: Draw
  // Execute draw call to process the texture with the shader.


    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->Draw(4, 0); // Drawing 4 vertices, using a triangle strip to form a quad.



    // Unbind the texture from the shader resource view
    //ID3D11ShaderResourceView* nullViews[] = { nullptr };
  //  context->PSSetShaderResources(0, 1, nullViews);

  // Step 5: Read Back the Texture
  // This involves copying the texture back to a CPU accessible resource to read back the processed imageData.
  // Cleanup omitted for brevity.

    ID3D11Texture2D* stagingTexture = nullptr;
 /*   D3D11_TEXTURE2D_DESC stagingDesc = {};
    texture->GetDesc(&stagingDesc);
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0; // Remove binding flags
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagingDesc.MiscFlags = 0;
    */

    // Describe the texture to be created.
    D3D11_TEXTURE2D_DESC stagingDesc = {};
    stagingDesc.Width = width; // Texture width.
    stagingDesc.Height = height; // Texture height.
    stagingDesc.MipLevels = 1; // Number of mipmap levels.
    stagingDesc.ArraySize = 1; // Number of textures in the texture array.
    stagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of the texture.
    stagingDesc.SampleDesc.Count = 1; // The number of multisamples per pixel.
    stagingDesc.Usage = D3D11_USAGE_STAGING; // Specify how the texture is to be read from and written to.
    stagingDesc.BindFlags = 0; // Bind the texture to the shader resource and render target. That way we can both read an write to the texture.
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    hr = device->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);
    if (FAILED(hr)) {
        std::cerr << "Failed to create staging texture." << std::endl;
        return false;
    }

   // ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
  //  context->PSSetShaderResources(0, 1, nullSRV);

    // Copy the processed texture to the staging texture
    context->CopyResource(stagingTexture, renderTargeTexture);

    context->Flush();

    // Map the staging texture to gain read access
    D3D11_MAPPED_SUBRESOURCE mappedResource;// = { nullptr,1,1 };

    hr = context->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);
    if (FAILED(hr)) {
        std::cerr << "Failed to map staging texture." << std::endl;
        HRESULT reason = device->GetDeviceRemovedReason();

        switch (reason) {
        case DXGI_ERROR_DEVICE_HUNG:
            std::cerr << "The GPU device hung (stalled) and was reset.";
            break;
        case DXGI_ERROR_DEVICE_RESET:
            std::cerr << "The GPU device was reset due to a driver issue.";
            break;
            // Add more cases as needed for other reason codes
        default:
            std::cerr << "Unknown reason code: " << std::hex << reason;
            break;
        }
        stagingTexture->Release();
        return false;
    }

    // Now you can read the data from mappedResource.pData
    // The data layout will depend on the format of the texture. For this example, assuming RGBA
  //  unsigned char* data = reinterpret_cast<unsigned char*>(mappedResource.pData);

    // Assuming imageData has enough space allocated for the processed texture data
    unsigned char* destRow = imageData;
    const unsigned char* sourceRow = reinterpret_cast<const unsigned char*>(mappedResource.pData);

    size_t rowWidth = width * channels; // Assuming 4 channels (RGBA) for simplicity

    for (int y = 0; y < height; ++y) {
        memcpy(destRow, sourceRow, rowWidth);

        destRow += rowWidth;
        sourceRow += mappedResource.RowPitch; // Use RowPitch to move to the next row in the source data
    }

    // Don't forget to unmap the resource once done
    context->Unmap(stagingTexture, 0);
    

    // Cleanup Direct3D resources
    if (sourceTexture) sourceTexture->Release();
    if (context) context->Release();
    if (device) device->Release();
    if (stagingTexture) stagingTexture->Release();
    std::cout << "Direct3D operation completed successfully." << std::endl;
    return true;
}


