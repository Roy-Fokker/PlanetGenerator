cbuffer viewBuffer : register(b0)
{
    float4x4 projection;
}

cbuffer frameBuffer : register(b1)
{
    float4x4 view;
}

cbuffer transformBuffer : register(b2)
{
    float4x4 transform;
}


float4 main( float4 pos : POSITION ) : SV_POSITION
{
	pos.w = 1.0f;

	pos = mul(pos, transform);
    pos = mul(pos, view);
    pos = mul(pos, projection);
	
	return pos;
}