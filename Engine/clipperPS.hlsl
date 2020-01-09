cbuffer LightBuffer
{
	float3 alc;
	float ali;
	float3 dlc;
	float dli;
	float3 slc;
	float sli;
	float4 lightPosition;
	float4 eyePos;
};


struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 worldPos : WPOS;
	float planeDistSigned : PDS;
};

Texture2D shaderTexture : register(t0);

SamplerState SampleType;

static const float SpecularPower = 8.f;


float3 applyFog(in float3 rgb, in float distance, in float3 rayDir, in float3 sunDir) 
{
	float fogAmount = 1.0 - exp(-distance * 0.0001f);
	float moonIntensity = max(dot(rayDir, sunDir), 0.0);
	float3 fogColor = lerp(float3(0.5, 0.6, 0.8), float3(1.0, 0.9, 0.7), pow(moonIntensity, 8.0));
	return lerp(rgb, fogColor, fogAmount);
}



float4 calcAmbient(in float3 alc, in float ali) 
{
	return saturate(float4(alc, 1.0f) * ali);
}


float4 calcDiffuse(in float3 invLightDir, in float3 normal, in float3 dlc, in float dli, inout float dFactor)
{
	dFactor = max(dot(normal, invLightDir), 0.0f);
	return saturate(float4(dlc, 1.0f) * dli * dFactor);
}


float4 calcSpecular(in float3 invLightDir, in float3 normal, in float3 slc, in float sli, in float3 invViewDir, in float dFactor)
{
	float3 reflection = normalize(reflect(invLightDir, normal));
	float sFactor = pow(saturate(dot(reflection, invViewDir)), SpecularPower);
	return saturate(float4(slc, 1.0f) * sFactor * sli * dFactor);
}



float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	if (input.planeDistSigned < 0) discard;

	input.normal = normalize(input.normal);

	float3 lightDir = normalize(input.worldPos.xyz - lightPosition.xyz);
	float3 invLightDir = -lightDir;

	float3 viewDir = input.worldPos.xyz - eyePos.xyz;
	float distance = length(viewDir);
	viewDir = viewDir / distance;
	float3 invViewDir = -viewDir;

	//texture colour
	float4 colour = shaderTexture.Sample(SampleType, input.tex);

	//calculate ambient light
	float4 ambient = calcAmbient(alc, ali);

	//calculate diffuse light
	float dFactor = 0.f;
	float4 diffuse = calcDiffuse(invLightDir, input.normal, dlc, dli, dFactor);

	//calculate specular light
	float4 specular = calcSpecular(invLightDir, input.normal, slc, sli, viewDir, dFactor);

	colour = (ambient + diffuse) * colour + specular;

	//colour = float4(applyFog(colour.xyz, distance, viewDir, lightDir), 1.0f);

	colour.rgb = pow(colour.xyz, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));

	colour.a = 1.f;
	return colour;
}