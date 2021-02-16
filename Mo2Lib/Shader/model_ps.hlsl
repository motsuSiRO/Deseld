#include "Structures.hlsli"
#include "Common.hlsli"

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

//float3 Diffuse(float3 N, float3 L, float3 C, float3 K);
//float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E, float3 K, float Power);
//float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E, float3 K, float Power);
//float3 Fog(float3 color, float3 viewPos, float3 Pos, float3 fogColor, float Near, float Far);
//float3 HemiSphereLight(float3 N, float3 SkyColor, float3 GroundColor);

//static const float3 sfog_color = { 0.8f,0.8f,0.6f };
//static const float sfog_near = 70.0;
//static const float sfog_far = 5000.0;

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord);
	float3 N = normalize(pin.w_normal);
	float3 E = normalize(cam.position.xyz - pin.w_pos);
	float3 L = normalize(light_direction.xyz);

	//ŠÂ‹«Œõ
	float3 A = float3(0.5f, 0.5f, 0.5f);

	//ŠgŽU”½ŽË
	float3 C = light_color.rgb;
	float3 Kd = float3(1.0, 1.0, 1.0);
	float3 D = Diffuse(N, L, C, Kd);

	//‹¾–Ê”½ŽË
	float3 Ks = float3(1., 1., 1.);
	float3 S = BlinnPhongSpecular(N, L, C, E, Ks, 1000);

	color *= pin.color *float4(A + D + S, 1.0);

	color.rgb = Fog(color.rgb, cam.position.xyz, pin.w_pos, fog_color, fog_near, fog_far);

	return color;
}


