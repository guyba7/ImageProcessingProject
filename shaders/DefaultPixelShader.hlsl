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
    float4 texColor = shaderTexture.Sample(samplerState, input.TexCoord);
    return float4(texColor[0], texColor[1], texColor[2], texColor[3]);

}