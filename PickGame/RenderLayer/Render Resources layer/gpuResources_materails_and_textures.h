#pragma once
#include "..\..\Include\datatypedef.h"
#include "DirectXMath.h"
#include <winrt/base.h>
#include <d3d12.h>

struct ResourceWithUploader
{
	winrt::com_ptr<ID3D12Resource> ResourceInDefaultHeap;
	winrt::com_ptr<ID3D12Resource> ResourceInUploadHeap;
};

struct MaterialData
{
	DirectX::XMFLOAT4 DiffuseAlbedo;
	DirectX::XMFLOAT3 FresnelR0;
	float Roughness;

	DirectX::XMFLOAT4X4 MatTransformation;

	UINT Flags;
	UINT TextureMapIndex[6];
	//not used
	UINT pad0;
};


#define MaxLights 10

struct LightGPU
{
	DirectX::XMFLOAT3 Strength;
	float falloffStart;
	DirectX::XMFLOAT3 Direction;
	float falloffEnd;
	DirectX::XMFLOAT3 Position;
	float spotPower;

	float lightType;
	float turnOn; // to find check it for int16 or something like that
	float dummy2;
	float dummy3;
	DirectX::XMFLOAT4X4 ViewProj; //View-Projection matrix (LOD1 Level Shadow)
	DirectX::XMFLOAT4X4 ViewProjT; //View-Projection-toUV matrix (LOD1 Level Shadow)
	DirectX::XMFLOAT3 ReflectDirection;
	float dummy4;
};

struct PassConstantsGPU
{
	DirectX::XMFLOAT4X4 View;// = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 Proj;// = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj;// = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj;// = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProjT;// = MathHelper::Identity4x4();

	DirectX::XMFLOAT3 EyePosW = { 1.0f, 1.0f, 0.0f };
	float pad0;

	DirectX::XMFLOAT4 AmbientLight = { 0.5f, 0.5f, 0.5f, 1.0f };
	DirectX::XMFLOAT4 FogColor = { 0.0f, 0.5f, 0.4f, 1.0f };

	float FogStart = 5.0f;
	float FogRange = 50.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	LightGPU Lights[MaxLights];
};