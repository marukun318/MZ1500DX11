Texture2D diffuseTexture: register(t0);
SamplerState diffuseTextureSampler: register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
//�O���[�o��
cbuffer global : register(b0)
{
	matrix g_WVP; //���[���h����ˉe�܂ł̕ϊ��s��
};

cbuffer cbNeverChanges : register(b1)
{
	matrix View;
};

cbuffer cbChangeOnResize : register(b2)
{
	matrix Projection;
};

cbuffer cbChangesEveryFrame : register(b3)
{
	matrix World;
};

//�\����
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
	float2 Tex : TEXCOORD;
};

//
//
//�o�[�e�b�N�X�V�F�[�_�[
VS_OUTPUT VS(float4 Pos : POSITION, float4 Col : COLOR, float2 Tex : TEXCOORD0)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	Pos = mul(Pos, World);
	Pos = mul(Pos, View);
	output.Pos = mul(Pos, Projection);
	output.Tex = Tex;
	output.Col = Col;

	return output;
}

//
//
//�s�N�Z���V�F�[�_�[
float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 texel = diffuseTexture.Sample(diffuseTextureSampler, input.Tex);
	return input.Col * texel;
//	return texel;

//	return float4(1,1,1,1);
}
