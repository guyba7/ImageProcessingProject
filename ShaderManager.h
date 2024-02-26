#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream> // Include this for std::cerr and std::cout
#include <windows.h> // Ensure you have this for GetConsoleWindow()
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using std::string;  // Make vector available as 'vector'

class ShaderManager {
public:
    bool initalizeShaderManager(string*);

    bool createShadersFromFiles(LPCWSTR, LPCWSTR, ID3D11VertexShader**, ID3D11PixelShader**, string*);

    bool applyShaderOnImageData(unsigned char*, int, int, int, ID3D11PixelShader*, ID3D11VertexShader*, string*);

    bool test3D(unsigned char* , int, int, int); // Declaration of the function

private:

    void initializeViewport(int, int);

    bool initializeVertices(string*);

    bool create2DTextures(unsigned char*, int, int, ID3D11Texture2D**, ID3D11Texture2D**, ID3D11Texture2D**, string*);

    bool copyRenderTargetToImageData(unsigned char*, int, int, int, ID3D11Texture2D*, ID3D11Texture2D*, string*);

    void releaseAllD3DMembers();
};
