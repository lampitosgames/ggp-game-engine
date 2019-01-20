struct PixelOut 
{
	float4 color        : SV_TARGET0; // Render normally
	float4 distance     : SV_TARGET1; // Depth render target
};

struct VertexToPixel
{
    float4 position		: SV_POSITION;
    float3 sampleDir	: TEXCOORD;
	float depth         : DEPTH;
};

// Define globals for the texture samples
TextureCube SkyTexture		: register( t0 );
SamplerState BasicSampler	: register( s0 );

cbuffer externalData : register(b0)
{
	float4 dofPara;
};

// Entry point for this pixel shader
PixelOut main( VertexToPixel input ) 
{
	PixelOut output;
	output.color = float4(SkyTexture.Sample(BasicSampler, input.sampleDir).rgb,0.0f);

	float f = 0.0f;
	if (input.depth < dofPara.y)
	{
		f = (input.depth - dofPara.y) / (dofPara.y - dofPara.x);
	}
	else
	{
		f = (input.depth - dofPara.y) / (dofPara.z - dofPara.y);
		f = clamp(f, 0, dofPara.w);
	}
	output.distance = float4((f*0.5f + 0.5f), 0, 0, 0);

	return output;
}