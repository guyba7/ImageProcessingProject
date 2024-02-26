struct VSInput
{
    float4 Pos : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.Pos = input.Pos;

    // Calculate the offset to keep the texture centered after scaling
    float2 centerOffset = 0.5 - (0.5 * 2);

    // Apply the scaling and re-center the texture
    output.TexCoord = (input.TexCoord * 2) + centerOffset;

    return output;
}


