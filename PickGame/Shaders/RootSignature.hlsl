#define rootSignatureC1 "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),\
 RootConstants(num32BitConstants=2, b0),\
 SRV(t0, space=1),\
 SRV(t1, space=1),\
 SRV(t2, space=1),\
 CBV(b3),\
 DescriptorTable( SRV(t0, numDescriptors = unbounded), visibility=SHADER_VISIBILITY_ALL),\
 StaticSampler(s0, visibility = SHADER_VISIBILITY_ALL),\
 StaticSampler(s1, addressU=TEXTURE_ADDRESS_BORDER, addressV=TEXTURE_ADDRESS_BORDER, borderColor=STATIC_BORDER_COLOR_OPAQUE_WHITE, filter = FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, comparisonFunc =COMPARISON_LESS_EQUAL, visibility=SHADER_VISIBILITY_PIXEL)"
 

uint gInstancesOffset:register(b0);
uint gTechFlags:register(b1);
StructuredBuffer<InstanceData> gInstanceData: register(t0, space1);
StructuredBuffer<MaterialData> gBoardMaterialData: register(t1, space1);
StructuredBuffer<BoneData> cbEnvMaterialData: register(t2, space1);
ConstantBuffer<PassStruct> cbPass : register(b3);
Texture2D gTextures[100] : register(t0); // Here is important that this value >= than number of SRV (t) in previous Technical Descriptor Heap, just the mapping with rootsignature (here t10=t10) should be the same

SamplerState gsamPointWrap : register(s0);
SamplerComparisonState gsamShadow : register(s1);