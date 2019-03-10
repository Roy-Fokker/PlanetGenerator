cbuffer transformBuffer : register(b0)
{
	matrix transform;
}


float4 main( float4 pos : POSITION ) : SV_POSITION
{
	pos.w = 1.0f;

	pos = mul(pos, transform);
	
	return pos;
}