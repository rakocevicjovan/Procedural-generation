cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

PixelInputType CMVS(VertexInputType input) {

	PixelInputType output;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix).xyww;

	output.texCoord = input.position.xyz;

    output.normal = input.normal;

	return output;
}