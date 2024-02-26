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
    output.TexCoord = float2(1.0f - input.TexCoord.x, input.TexCoord.y); // Flip horizontally
    return output;
}
