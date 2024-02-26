Texture2D shaderTexture : register(t0);
SamplerState samplerState : register(s0);

struct PSInput
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float4 main(PSInput input) : SV_Target
{
    float edgeDetectionThreshold = 0.1;

    // Hardcoded texel size for demonstration. Adjust based on actual texture dimensions.
    float2 texelSize = float2(0.001f, 0.001f); // Example texel size, replace with your actual values

    float gx = 0.0;
    float gy = 0.0;

    // Sobel operator matrices
    float3x3 Gx =
    {
        { -1, 0, 1 },
        { -2, 0, 2 },
        { -1, 0, 1 },
    };
    float3x3 Gy =
    {
        { -1, -2, -1 },
        { 0, 0, 0 },
        { 1, 2, 1 },
    };

    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float2 sampleCoord = input.TexCoord + float2(x, y) * texelSize;
            float4 sampleColor = shaderTexture.Sample(samplerState, sampleCoord);
            float intensity = dot(sampleColor.rgb, float3(0.299, 0.587, 0.114)); // Convert to grayscale
            gx += intensity * Gx[y + 1][x + 1];
            gy += intensity * Gy[y + 1][x + 1];
        }
    }

    float edge = sqrt(gx * gx + gy * gy);
    if (edge > edgeDetectionThreshold)
        return float4(1, 1, 1, 1); // Edge -> White
    else
        return float4(0, 0, 0, 1); // Non-edge -> Black
}
