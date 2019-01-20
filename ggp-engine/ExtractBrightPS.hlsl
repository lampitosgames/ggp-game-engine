//input
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

//textures and such
Texture2D Pixels        : register(t0);
SamplerState Sampler    : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	//sample color for the current uv
	float4 finalColor = Pixels.Sample(Sampler, input.uv);
	//return finalColor;
	//determine if this pixel is bright or not
	//if it is bright enough, keep it
	//if it is not, make it black
	float brightness = 0.2126f * finalColor.r + 0.7152f * finalColor.g + 0.0722f * finalColor.b;
	////light object, always bloom
	//if (finalColor.a == 0)
	//{
	//	return float4(finalColor.rgb, 0);
	//}
	//else 
	if (finalColor.a == 1.0f)
	{
		if (brightness <= 0.65f)
		{
			finalColor.rgb /= 15.0f;
		}
		else if (brightness <= 0.70f)
		{
			finalColor.rgb /= 12.0f;
		}
		else if (brightness <= 0.75f)
		{
			finalColor.rgb /= 8.0f;
		}
		else if (brightness <= 0.85f)
		{
			finalColor.rgb /= 6.0f;
		}
		else if (brightness <= 0.92f)
		{
			finalColor.rgb /= 3.0f;
		}
	}
	//skybox, very little bloom
	else if(finalColor.a == 0.0f)
	{
		if (brightness <= 0.95f)
		{
			finalColor.rgb = float3(0.0f, 0.0f, 0.0f);
		}
		else
		{
			finalColor.rgb /= 25.0f;
		}
	}
	
	return finalColor;
}