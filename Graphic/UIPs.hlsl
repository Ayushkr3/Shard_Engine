struct PSInput {
	float4 pos : SV_POSITION;
	float2 uv : Texcoord;
};

float4 main(PSInput PI) : SV_TARGET
{
	float4 test = float4(1.0f,0.0f,0.0f, 1);
	return test;
}