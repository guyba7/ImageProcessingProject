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
    output.Pos = float4(input.Pos.xy, 0.0f, 1.0f); // Assuming z = 0 and w = 1 for all vertices
    output.TexCoord = input.TexCoord;
    return output;
}
