// Assume textureCoord is passed from the vertex shader and you have a texture bound as t0
Texture2D shaderTexture : register(t0);
SamplerState samplerState : register(s0);

struct PSInput
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0; // Ensure this is passed from the vertex shader
};

float4 main(PSInput input) : SV_Target
{
    // Define the blur size, decrease to intesify blur
    float2 blurSize = float2(1.0 / 60, 1.0 / 60); 

    // Sample the original pixel and its neighbors
    float4 color = shaderTexture.Sample(samplerState, input.TexCoord);

    // Accumulate colors of neighboring pixels
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float2 sampleCoord = input.TexCoord + float2(x, y) * blurSize;
            color += shaderTexture.Sample(samplerState, sampleCoord);
        }
    }

    // Average the colors (the original pixel is also part of the accumulation)
    color /= 9.0;

    return color;
}
