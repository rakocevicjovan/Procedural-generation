struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


PixelInputType LightVertexShader(VertexInputType input)
{   
	input.position.w = 1.0f;

	PixelInputType output;

    output.position = input.position; 
	output.tex = float2(input.tex.x, 1.f - input.tex.y);
	output.normal = input.normal;
    return output;
}