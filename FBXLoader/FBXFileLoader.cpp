#include <fbxsdk.h>
#include "FBXFileLoader.h"

#include "Logger.h"

#define NORMALMODE // if defined, Normal is just read from file. In another case, Normal should be averaged.

using namespace std;

FBXFileLoader::FBXFileLoader()
{

}

FBXFileLoader::~FBXFileLoader()
{
}

void FBXFileLoader::loadFile(const string& fileName)
{
	FbxManager* sdkManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(ios);

	//Create Import
	FbxImporter* lImporter = FbxImporter::Create(sdkManager, "");

	LogTextBlock textBlock;

	Logger::log("Loading FBX file: " + Logger::value(fileName) + "...");

	if (!lImporter->Initialize(fileName.c_str(), -1, sdkManager->GetIOSettings()))
	{
		Logger::errorMsg("faild");
		Logger::log("Call to FbxImporter::Initialize() is failed with the next error: ");
		//LogAutoInc inc;
		Logger::errorMsg(string(lImporter->GetStatus().GetErrorString()));
		return;
	}
	Logger::done();

	if (lImporter->IsFBX())
	{
		sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_MATERIAL, true);
		sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_TEXTURE, true);
		sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_LINK, true);
		sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_SHAPE, true);
		sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_GOBO, true);
		sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_ANIMATION, true);
		sdkManager->GetIOSettings()->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Get short name
	size_t pos1 = fileName.find_last_of('\\');
	pos1 = (pos1 == string::npos) ? fileName.find_last_of('/') : pos1;
	pos1 = (pos1 == string::npos) ? 0 : pos1 += 1;

	size_t pos2 = fileName.find_last_of('.');
	string sceneName = fileName.substr(pos1, pos2 - pos1);

	m_scene = FbxScene::Create(sdkManager, sceneName.c_str());
	lImporter->Import(m_scene);
	lImporter->Destroy();

	m_scene->SetName(sceneName.c_str());

	m_sceneName = sceneName;
	createScene();

	m_goodStatus = true;
	Logger::logln("Loading of " + fileName + " is done");
}

void FBXFileLoader::prepare()
{
	buildTextureIDList();
	buildMeshsIDByNameList();
	buildMaterialsIDList();
}

void FBXFileLoader::createScene()
{
	LogAutoInc incrementer;
	Logger::log("Creating the scene");

	if (m_scene == nullptr)
	{
		Logger::logln(" No scene was created before calling createScene() or it is empty");
		return;
	}

	Logger::logln(" " + Logger::value(m_scene->GetName()));


	// Do required metric conversation
	const FbxSystemUnit::ConversionOptions cnvOptions =
	{
		false, // mConvertRrsNodes
		true, // mConverLimit
		true, // mConvertClusters
		true, // mCinverLightIntensity
		true, // mConvertPhotometricLProperties
		true  // mConvertCameraClipPlanes
	};

	FbxSystemUnit::m.ConvertScene(m_scene, cnvOptions);

	// Take a look and process all Nodes in FBX file
	processNodes();
}

void FBXFileLoader::processNodes()
{
	Logger::logln("Begin FBX Nodes proccesing");

	auto rootNode = m_scene->GetRootNode();
	if (rootNode)
	{
		int childCount = rootNode->GetChildCount();

		for (int i = 0; i < childCount; i++)
			processEachNode(rootNode->GetChild(i));
	}
	else
	{
		Logger::logln("FBX file is empty");
	}
}

void FBXFileLoader::processEachNode(FbxNode* pNode)
{
#ifdef LOGL1
	LogAutoInc loginc;
	Logger::log("[Node:] " + Logger::value(pNode->GetName()));
#endif // LOGL1

	FBXFileLoader::Instance newNodeInstance = {};

	// We Node - New Instance
	newNodeInstance.Visible = pNode->GetVisibility();
	
	// Any kind of Node can have a material
	processNodeMaterial(pNode, newNodeInstance);

	if (pNode->GetNodeAttribute())
	{
		switch (pNode->GetNodeAttribute()->GetAttributeType())
		{
		case fbxsdk::FbxNodeAttribute::eUnknown:
			// It can be a "root" node for Skeleton, let's check it
			processEachNode(pNode->GetChild(0));
			break;
		
		case fbxsdk::FbxNodeAttribute::eSkeleton:
			processNodeSkeleton(pNode, newNodeInstance);
			break;
		
		case fbxsdk::FbxNodeAttribute::eMesh:
			processNodeMesh(pNode, newNodeInstance);
			break;
		
		case fbxsdk::FbxNodeAttribute::eCamera:
			processNodeCamera(pNode, newNodeInstance);
			break;

		case fbxsdk::FbxNodeAttribute::eLight:
			processNodeLight(pNode, newNodeInstance);
			break;

		case fbxsdk::FbxNodeAttribute::eLODGroup:
			processNodeLight(pNode, newNodeInstance);

		default: break;
		}
	}
	m_instances.push_back(newNodeInstance);

#ifdef LOGL1
	Logger::logln("");
#endif // LOGL1

}

void FBXFileLoader::processNodeMaterial(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst)
{
	using namespace FBXLoader;

	for (u8 i = 0; i < pNode->GetMaterialCount(); i++)
	{
		auto& fbxMaterial = *pNode->GetMaterial(i);

		// If we have read this material for another Node, no need read it again
		if (m_materials.find(fbxMaterial.GetName()) != m_materials.end())
		{
			inst.Materials.push_back(&m_materials.find(fbxMaterial.GetName())->second);
			continue;
		}
		
#ifdef LOGL1
		LogAutoInc _inc;
		Logger::logln("");
		Logger::logln(" [New Materail reading: ] " + Logger::value(fbxMaterial.GetName()));
#endif //LOGL1

		Material newMaterial;
		newMaterial.Name = fbxMaterial.GetName();

		if (fbxMaterial.ShadingModel.Get() == "Phong")
		{
			FbxSurfacePhong* phongMaterial = static_cast<FbxSurfacePhong*>(&fbxMaterial);

			readMaterialCustomProperties(phongMaterial, newMaterial);

			readMaterialDefaultProperties(phongMaterial, newMaterial);
		}

		m_materials[newMaterial.Name] = newMaterial;
		inst.Materials.push_back(&m_materials[newMaterial.Name]);
	}
}

void FBXFileLoader::processNodeSkeleton(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst)
{	
	FbxNode* parent = pNode->GetParent();
	if (parent)
	{
		FBXLoader::TreeBoneNode* rootBone = new FBXLoader::TreeBoneNode();
		rootBone->Name = parent->GetName();
		rootBone->Node = parent;

		//TODO::Con
	}
	
}

void FBXFileLoader::processNodeMesh(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst)
{
	assert(pNode);

	auto pMesh = static_cast<FbxMesh*>(pNode->GetNodeAttribute());

	FbxProperty customProperty = pNode->FindProperty(FBXLOADER_MESH_CUSTOMPR_LOD);
	
	if (customProperty != NULL)
	{
		inst.MeshName = std::string(customProperty.Get<FbxString>());
		inst.NodeType = FBXFileLoader::NT_Mesh;

#ifdef LOGL1
		Logger::log(" [-mesh: LOD_GROUP] " + Logger::value(inst.MeshName));
#endif // LOGL1
		return;
	}

#ifdef LOGL1
	Logger::log(" [-mesh: ] " + Logger::value(pMesh->GetName()));
#endif // LOGL1

	inst.MeshName = pMesh->GetName();

	if (m_meshes.find(pMesh->GetName()) != m_meshes.end())
	{
#ifdef LOGL1
		Logger::log(" [old] ");
#endif // LOG1
		return;
	}

#ifdef LOGL1
	Logger::log(" [new] ");
#endif // LOG1

	auto res = m_meshes.emplace(std::make_pair(inst.MeshName, Mesh()));
	auto& newMesh = res.first->second;
	
	// work with new mesh here
	newMesh.Name = pMesh->GetName();
	
	// Does it need to be (with all Instances) excluded from Frustom Culling (i.e. Sky, Land etc)
	customProperty = pNode->FindProperty(FBXLOADER_MESH_CUSTOMPR_EXCLFC);
	if (customProperty != NULL)
		newMesh.ExcludeFromCulling = customProperty.Get<bool>();
	
	// Does it need to be (with all Instances) excluded from mirror reflection
	customProperty = pNode->FindProperty(FBXLOADER_MESH_CUSTOMPR_EXLCMR);
	if (customProperty != NULL)
		newMesh.ExcludeFromMirrorReflection = customProperty.Get<bool>();

	readMeshInfo(pMesh, newMesh);
	
	convertFBXMatrixToFloat4X4(pNode->EvaluateGlobalTransform(), inst.GlobalTransformation);


}

void FBXFileLoader::processNodeCamera(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst)
{
}

void FBXFileLoader::processNodeLight(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst)
{
}

void FBXFileLoader::processNodeLODGroup(FBXSDK_NAMESPACE::FbxNode* pNode, FBXFileLoader::Instance& inst)
{
}

void FBXFileLoader::readMaterialCustomProperties(FbxSurfacePhong* phongMaterial, Material& material)
{

}

void FBXFileLoader::readMaterialDefaultProperties(FbxSurfacePhong* phongMaterial, Material& material)
{
	assert(phongMaterial);
	
	//Get Diffuse data
	{
		D4toF4 d4tof4(phongMaterial->Diffuse.Get().Buffer(), phongMaterial->AmbientFactor);
		material.DiffuseAlbedo = d4tof4.toF4();

		readTextureInfo(&phongMaterial->Diffuse, TEXTURE_TYPE::TEXTURE_TYPE_DIFFUSE, material);
	}

	// Get Specular data
	{
		D4toF4 d4tof4(phongMaterial->Specular.Get().Buffer(), phongMaterial->SpecularFactor);
		material.Specular = d4tof4.toF4();
		material.Roughness = (1.0f - material.Specular.w) > 0.9999f ? 0.9999f : (1.0f - material.Specular.w);
		material.FresnelR0 = DirectX::XMFLOAT3(material.Specular.x, material.Specular.y, material.Specular.z);

		readTextureInfo(&phongMaterial->Specular, TEXTURE_TYPE::TEXTURE_TYPE_SPECULAR, material);
	}

	// Get Normal data
	{
		material.doesItHaveNormalTexture = readTextureInfo(&phongMaterial->NormalMap, TEXTURE_TYPE::TEXTURE_TYPE_NORMAL,
			material);
	}

	// Get Transparency data
	{
		D4toF4 d4tof4(phongMaterial->TransparentColor.Get().Buffer(), phongMaterial->TransparencyFactor);
		material.TransparencyColor = d4tof4.toF4();
		material.isTransparent =
			!(material.TransparencyColor.x == material.TransparencyColor.y == material.TransparencyColor.z);

		material.isTransparencyUsed =
			readTextureInfo(&phongMaterial->TransparencyFactor, TEXTURE_TYPE::TEXTURE_TYPE_TRANSPARENCY, material);
	}
}

bool FBXFileLoader::readTextureInfo(FbxProperty* fbxProperty, TEXTURE_TYPE textureType, Material& material)
{
	assert(fbxProperty);

	bool atLeastOneTextureWasAdded = false;

	int srcObjectCount = fbxProperty->GetSrcObjectCount<FbxFileTexture>(); // We try to use FbxFileTexture instead FbxTexture
	for (int i=0; i < srcObjectCount; i++)
	{
		auto textureInfo = fbxProperty->GetSrcObject<FbxFileTexture>(i);
		string name = textureInfo->GetName();
		string path = textureInfo->GetFileName();
		addTexturePathByName_(name, path);

		auto textTypeByName = std::make_pair(textureType, name);
		material.TextureNameType.push_back(textTypeByName);
		atLeastOneTextureWasAdded = true;

#ifdef LOGL2
		LogAutoInc _inc;
		Logger::logln("Texture(" + string(PRINT_DEF_NAME( textureType )) + "): "
			+ name + "[" + path + "]");
#endif
	}

	return atLeastOneTextureWasAdded;
}

bool FBXFileLoader::readMeshInfo(FbxMesh* pMesh, Mesh& newMesh, bool tessellated)
{
	using namespace DirectX;

	assert(pMesh);

	// Copy all Vertices
	auto vertexData = pMesh->GetControlPoints();
	int vertexCount = pMesh->GetControlPointsCount();
	
	newMesh.Vertices.resize(vertexCount);
	for (int i = 0; i < vertexCount; i++, vertexData++)
		newMesh.Vertices[i] = DirectX::XMFLOAT3(vertexData->mData[0], vertexData->mData[1], vertexData->mData[2]);

	// Prepare for UV
	auto UVLayer = pMesh->GetElementUV();
	if (UVLayer)
	{
		assert(
			UVLayer->GetMappingMode() == FbxGeometryElement::EMappingMode::eByPolygonVertex &&
			UVLayer->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eIndexToDirect
		);
	}

	// Prepare data for Normals
	vector<DirectX::XMFLOAT3> averageNormal;
	auto normalLayer = pMesh->GetElementNormal();
	if (normalLayer)
	{
		assert(
			normalLayer->GetMappingMode() == FbxGeometryElement::EMappingMode::eByPolygonVertex &&
			normalLayer->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eDirect
		);

#ifndef NORMALMODE
		averageNormal(pMesh->Vertices.size());

		//ADD SOME STUFF HERE
#endif
	}

	auto tangentLayer = pMesh->GetElementTangent();
	if (tangentLayer)
	{ 
		assert(
			tangentLayer->GetMappingMode() == FbxGeometryElement::EMappingMode::eByPolygonVertex &&
			tangentLayer->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eDirect
		);
	}
	
	u32 vertexID = 0;
	for (u32 pi = 0; pi < pMesh->GetPolygonCount(); pi++)
	{
		newMesh.VertexPerPolygon = pMesh->GetPolygonSize(pi);

		if (newMesh.VertexPerPolygon == 3)
		{
			if (tessellated)
				assert(false && "Triangle for Tesselated Mesh");

			newMesh.Indices.push_back(pMesh->GetPolygonVertex(pi, 0));
			newMesh.Indices.push_back(pMesh->GetPolygonVertex(pi, 1));
			newMesh.Indices.push_back(pMesh->GetPolygonVertex(pi, 2));

			// UV
			if (UVLayer)
			{
				int UVID0 = pMesh->GetTextureUVIndex(pi, 0);
				int UVID1 = pMesh->GetTextureUVIndex(pi, 0);
				int UVID2 = pMesh->GetTextureUVIndex(pi, 0);

				FbxVector2 uv0 = UVLayer->GetDirectArray().GetAt(UVID0);
				FbxVector2 uv1 = UVLayer->GetDirectArray().GetAt(UVID1);
				FbxVector2 uv2 = UVLayer->GetDirectArray().GetAt(UVID2);

				newMesh.UVs.push_back(DirectX::XMFLOAT2(uv0.mData[0], 1.0f - uv0.mData[1]));
				newMesh.UVs.push_back(DirectX::XMFLOAT2(uv1.mData[0], 1.0f - uv1.mData[1]));
				newMesh.UVs.push_back(DirectX::XMFLOAT2(uv2.mData[0], 1.0f - uv2.mData[1]));
			}

			// Normal
			if (normalLayer)
			{
#ifdef NORMALMODE
				FBXArrayToXMFLOATVector(normalLayer->GetDirectArray(), newMesh.Normals, vertexID);
#else
				// Here averageNormal vector should aready have normal calculated (in {Prepare data for Normals})
				auto normal0 = averageNormal[lmesh->Indices[lmesh->Indices.size() - 3]];
				auto normal0 = averageNormal[lmesh->Indices[lmesh->Indices.size() - 2]];
				auto normal0 = averageNormal[lmesh->Indices[lmesh->Indices.size() - 1]];

				newMesh.Normal.push_back(normal0);
				newMesh.Normal.push_back(normal0);
				newMesh.Normal.push_back(normal0);
#endif
			}

			// Tangent
			if (tangentLayer)
			{
				auto biTangentLayer = pMesh->GetElementBinormal(); // TODO: How to use a Index here?

				FBXArrayToXMFLOATVector(tangentLayer->GetDirectArray(), newMesh.Tangents, vertexID);
				FBXArrayToXMFLOATVector(biTangentLayer->GetDirectArray(), newMesh.Tangents, vertexID);
			}

			vertexID += 3;
		}
		else if (newMesh.VertexPerPolygon == 4)
		{
			BuildQuad<u32>(
				pMesh->GetPolygonVertex(pi, 0),
				pMesh->GetPolygonVertex(pi, 1),
				pMesh->GetPolygonVertex(pi, 2),
				pMesh->GetPolygonVertex(pi, 3),
				newMesh.Indices);

			// UV
			if (UVLayer)
			{
				auto UVID0 = pMesh->GetTextureUVIndex(pi, 0);
				auto UVID1 = pMesh->GetTextureUVIndex(pi, 1);
				auto UVID2 = pMesh->GetTextureUVIndex(pi, 2);
				auto UVID3 = pMesh->GetTextureUVIndex(pi, 3);

				auto uv0 = UVLayer->GetDirectArray().GetAt(UVID0);
				auto uv1 = UVLayer->GetDirectArray().GetAt(UVID1);
				auto uv2 = UVLayer->GetDirectArray().GetAt(UVID2);
				auto uv3 = UVLayer->GetDirectArray().GetAt(UVID3);

				auto xmuv0 = XMFLOAT2(uv0.mData[0], 1.0f - uv0.mData[1]);
				auto xmuv1 = XMFLOAT2(uv1.mData[0], 1.0f - uv1.mData[1]);
				auto xmuv2 = XMFLOAT2(uv2.mData[0], 1.0f - uv2.mData[1]);
				auto xmuv3 = XMFLOAT2(uv3.mData[0], 1.0f - uv3.mData[1]);

				BuildQuad<XMFLOAT2>(xmuv0, xmuv1, xmuv2, xmuv3, newMesh.UVs);
			}

			if (normalLayer)
			{
#ifdef NORMALMODE
				FBXQuadArrayToVector(normalLayer->GetDirectArray(), newMesh.Normals, vertexID, true);
#else
				BuildQuad<XMFLOAT3>(
					averageNormal[pMesh->GetPolygonVertex(pi, 0)],
					averageNormal[pMesh->GetPolygonVertex(pi, 1)],
					averageNormal[pMesh->GetPolygonVertex(pi, 2)],
					averageNormal[pMesh->GetPolygonVertex(pi, 3)],
					newMesh.Normals);
#endif
			}

			if (tangentLayer)
			{
				auto biTangentLayer = pMesh->GetElementBinormal();
				FBXQuadArrayToVector(tangentLayer->GetDirectArray(), newMesh.Tangents, vertexID, true);
				FBXQuadArrayToVector(biTangentLayer->GetDirectArray(), newMesh.BiTangents, vertexID, true);
			}

			vertexID += 4;
		}
		else
		{
			bool PolygonSizeNot_3_or_4 = false;
			assert(PolygonSizeNot_3_or_4);
		}
	}

	return true; // all is right
}


void FBXFileLoader::convertFBXMatrixToFloat4X4(const FBXSDK_NAMESPACE::FbxMatrix& fbxM, DirectX::XMFLOAT4X4& m4x4)
{
	FbxVector4 r0 = fbxM.GetRow(0);
	FbxVector4 r1 = fbxM.GetRow(1);
	FbxVector4 r2 = fbxM.GetRow(2);
	FbxVector4 r3 = fbxM.GetRow(3);

	DirectX::XMMATRIX m = DirectX::XMMatrixSet(
		r0.mData[0], r0.mData[1], r0.mData[2], r0.mData[3],
		r1.mData[0], r1.mData[1], r1.mData[2], r1.mData[3],
		r2.mData[0], r2.mData[1], r2.mData[2], r2.mData[3],
		r3.mData[0], r3.mData[1], r3.mData[2], r3.mData[3]);

	DirectX::XMStoreFloat4x4(&m4x4, m);
}


void FBXFileLoader::addTexturePathByName_(const std::string& name, const std::string& path)
{
	auto it = m_texturePathByName.find(name);
	if (it == m_texturePathByName.end())
		m_texturePathByName[name] = path;
}