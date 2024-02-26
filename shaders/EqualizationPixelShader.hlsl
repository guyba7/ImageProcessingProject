Texture2D shaderTexture : register(t0);
SamplerState samplerState : register(s0);

struct PSInput
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float4 AdjustContrast(float4 color, float contrast)
{
    // Adjusts the contrast of the image
    // The formula used here is a simple linear stretch of the form: f(x) = (x - 0.5) * contrast + 0.5
    // where `contrast` is a factor that determines how much the contrast is adjusted.
    // A `contrast` value of 1.0 means no change, less than 1.0 reduces contrast, and greater than 1.0 increases it.
    float3 adjusted = (color.rgb - 0.5) * contrast + 0.5;
    return float4(adjusted, color.a);
}

float4 main(PSInput input) : SV_Target
{
    float4 texColor = shaderTexture.Sample(samplerState, input.TexCoord);
    float contrast = 2; // contrast value; adjust as needed for your use case
    return AdjustContrast(texColor, contrast);
}
