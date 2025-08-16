struct VSOut
{
	float4 pos : SV_POSITION;
	float2 uv : Texcoord;
};
struct VSIn {
	float3 pos: Position;
	float2 uv: Texcoord;
	float2 XY: XY; //position
	float2 SXY:SXY; //scaling
	uint Instance: SV_InstanceID;
};
cbuffer PerFrameBuffer : register (b0) {
	row_major matrix orthoProj;
}
VSOut main(VSIn vs)
{
	VSOut vso;
	vso.pos = mul(float4(vs.pos,1.0f),orthoProj);
	vso.pos.x = (vso.pos.x + vs.XY.x)*vs.SXY.x;
	vso.pos.y = (vso.pos.y + vs.XY.y)*vs.SXY.y;
	vso.uv = vs.uv;
	return vso;
}