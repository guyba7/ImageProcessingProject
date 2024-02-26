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

    // Calculate the vector from the texture coordinate to the center of the texture (assumed to be at 0.5, 0.5)
    float2 toCenter = float2(0.5, 0.5) - input.TexCoord;

    // Calculate distortion factor based on the distance to the center
    float distance = length(toCenter);
    // Increase the 'stretchFactor' to make the effect more pronounced
    float stretchFactor = 1.0 + distance * 0.2; // This factor determines how much the texture is stretched

    // Apply the distortion
    output.TexCoord = sin(input.TexCoord); //float2(0.5, 0.5) + toCenter * stretchFactor;

    return output;
}
