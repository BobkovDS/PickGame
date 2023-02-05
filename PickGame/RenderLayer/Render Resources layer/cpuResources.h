#pragma once

struct MaterialCPU
{
	int index;

	int MatCBIndex;
	DirectX::XMFLOAT4 DiffuseAlbedo;
	DirectX::XMFLOAT3 FresnelR0;
	DirectX::XMFLOAT4X4 MaterialTransformation;
	u32 Flags;
	u32 TextureMapIds[6];
	float Roughness;

	bool isTransparencyUsed;
	bool isTransparent;
	bool isSky;
	bool doesIncludeToWorldBB;
	bool doesItHaveNormalTexture;

	MaterialCPU() :
		MatCBIndex(-1),
		FresnelR0({ 0.5f, 0.1f, 0.1f }),
		Roughness(0.1f)
	{}
};