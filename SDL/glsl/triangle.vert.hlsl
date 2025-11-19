struct VSInput
{
    float3 pos : POSITION;
    float3 color : COLOR0;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float3 color : COLOR0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = float4(input.pos, 1.0);
    output.color = input.color;
    return output;
}
