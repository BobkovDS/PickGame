
#include "Lighting.hlsl"
#include "commonPart.hlsl"
#include "RootSignature.hlsl"


[RootSignature(rootSignatureC1)]
VertexOut VS(VertexIn vin, uint instID : SV_INSTANCEID) 
{
	VertexOut vout;
	
	 uint shapeID = instID + gInstancesOffset; 
	 InstanceData instData = gInstanceData[shapeID];
	 float4x4 wordMatrix = instData.World;  
	
	//get World transform
	float4 posW = mul(wordMatrix, float4(vin.PosL, 1.0f));
	vout.PosW = posW;
	float4x4 ViewProj = cbPass.ViewProj;

	 // Transform to homogeneous clip space.
	vout.PosH = mul(posW, ViewProj);	
	vout.NormalW = mul((float3x3) wordMatrix, vin.Normal);
	vout.UVText = vin.UVText;

	float3 tangentNU = vin.TangentU;//   normalize(vin.TangentU - dot(vin.TangentU, vin.Normal));
	vout.TangentW = float4(mul((float3x3) wordMatrix, tangentNU), vin.TangentU.w);
 
	//vout.UVTextProj = mul(orthogProj, T);
	vout.SSAOPosH = mul(posW, cbPass.ViewProjT);
	vout.ShapeID = shapeID;
	
	//vout.PosH = float4(0.5f, 0.5f, 0.0f, 1.0f);
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
		pin.NormalW = normalize(pin.NormalW);
	
		float3 toEyeW = cbPass.EyePosW - pin.PosW;
		float distToEye = length(toEyeW);
		toEyeW = toEyeW / distToEye;
		
		InstanceData instData = gInstanceData[pin.ShapeID];
		MaterialData material = gBoardMaterialData[instData.MaterialIndex];
		
		float4 diffuseAlbedo = material.DiffuseAlbedo;
	
		float3 Normal = pin.NormalW;
		// Diffuse Color
		if ((material.textureFlags & 0x01))
			diffuseAlbedo = gTextures[material.DiffuseMapIndex[0]].Sample(gsamPointWrap, pin.UVText);
	
		float diffuseTranspFactor = diffuseAlbedo.a;
	
			// Get Alpha value
		if ((material.textureFlags & 0x10))
			diffuseTranspFactor = gTextures[material.DiffuseMapIndex[4]].Sample(gsamPointWrap, pin.UVText).x;
	
		// Get SSAO factor
		float ssao_factor = 1.0f;
	
		// Get Normal
		if ((gTechFlags & (1 << RTB_NORMALMAPPING)) > 0)
		{
			if ((material.textureFlags & 0x04))
			{
				float4 readNormal = gTextures[material.DiffuseMapIndex[2]].Sample(gsamPointWrap, pin.UVText);
				Normal = NormalSampleToWorldSpace(readNormal.xyz, Normal, pin.TangentW);
				//return float4(Normal, 1.0f);
			}
		}
	
		//return float4(ssao_factor, ssao_factor, ssao_factor, 1.0f);
		float4 ambient = ssao_factor * cbPass.AmbientLight * diffuseAlbedo;
	
		const float shiness = 1.0f - material.Roughness;
		//material.FresnelR0 = float3(0.02f, 0.02f, 0.02f);
	
		MaterialLight matLight = { diffuseAlbedo, material.FresnelR0, shiness };
	
		float shadow_depth = 1.0f;
	
		float4 directLight = ComputeLighting(cbPass.Lights, matLight, pin.PosW, Normal, toEyeW, shadow_depth);
	
		float4 litColor = directLight + ambient;

		litColor.a = diffuseTranspFactor;
	
		return litColor;
	
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
}