/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
//	Do not edit. MakeTypeDescForHandler.bat will update this file.
	
	Frame* pFrame = NULL;
	desc = DBG_NEW UTTypeDesc("Frame");
	desc->size = sizeof(Frame);
	desc->access = DBG_NEW UTAccess<Frame>;
	db->RegisterDesc(desc);
	
	FrameTransformMatrix* pFrameTransformMatrix = NULL;
	desc = DBG_NEW UTTypeDesc("FrameTransformMatrix");
	desc->size = sizeof(FrameTransformMatrix);
	desc->access = DBG_NEW UTAccess<FrameTransformMatrix>;
	field = desc->AddField("", "Affinef", "matrix", "");
	field->offset = (char*)&(pFrameTransformMatrix->matrix) - (char*)pFrameTransformMatrix;
	db->RegisterDesc(desc);
	
	Light8* pLight8 = NULL;
	desc = DBG_NEW UTTypeDesc("Light8");
	desc->size = sizeof(Light8);
	desc->access = DBG_NEW UTAccess<Light8>;
	field = desc->AddField("", "int", "type", "");
	field->offset = (char*)&(pLight8->type) - (char*)pLight8;
	field = desc->AddField("", "Vec4f", "diffuse", "");
	field->offset = (char*)&(pLight8->diffuse) - (char*)pLight8;
	field = desc->AddField("", "Vec4f", "specular", "");
	field->offset = (char*)&(pLight8->specular) - (char*)pLight8;
	field = desc->AddField("", "Vec4f", "ambient", "");
	field->offset = (char*)&(pLight8->ambient) - (char*)pLight8;
	field = desc->AddField("", "Vec4f", "position", "");
	field->offset = (char*)&(pLight8->position) - (char*)pLight8;
	field = desc->AddField("", "float", "range", "");
	field->offset = (char*)&(pLight8->range) - (char*)pLight8;
	field = desc->AddField("", "float", "attenuation0", "");
	field->offset = (char*)&(pLight8->attenuation0) - (char*)pLight8;
	field = desc->AddField("", "float", "attenuation1", "");
	field->offset = (char*)&(pLight8->attenuation1) - (char*)pLight8;
	field = desc->AddField("", "float", "attenuation2", "");
	field->offset = (char*)&(pLight8->attenuation2) - (char*)pLight8;
	field = desc->AddField("", "Vec3f", "spotDirection", "");
	field->offset = (char*)&(pLight8->spotDirection) - (char*)pLight8;
	field = desc->AddField("", "float", "spotFalloff", "");
	field->offset = (char*)&(pLight8->spotFalloff) - (char*)pLight8;
	field = desc->AddField("", "float", "spotInner", "");
	field->offset = (char*)&(pLight8->spotInner) - (char*)pLight8;
	field = desc->AddField("", "float", "spotCutoff", "");
	field->offset = (char*)&(pLight8->spotCutoff) - (char*)pLight8;
	db->RegisterDesc(desc);
	
	Material* pMaterial = NULL;
	desc = DBG_NEW UTTypeDesc("Material");
	desc->size = sizeof(Material);
	desc->access = DBG_NEW UTAccess<Material>;
	field = desc->AddField("", "Vec4f", "face", "");
	field->offset = (char*)&(pMaterial->face) - (char*)pMaterial;
	field = desc->AddField("", "float", "power", "");
	field->offset = (char*)&(pMaterial->power) - (char*)pMaterial;
	field = desc->AddField("", "Vec3f", "specular", "");
	field->offset = (char*)&(pMaterial->specular) - (char*)pMaterial;
	field = desc->AddField("", "Vec3f", "emissive", "");
	field->offset = (char*)&(pMaterial->emissive) - (char*)pMaterial;
	db->RegisterDesc(desc);
	
	MeshFace* pMeshFace = NULL;
	desc = DBG_NEW UTTypeDesc("MeshFace");
	desc->size = sizeof(MeshFace);
	desc->access = DBG_NEW UTAccess<MeshFace>;
	field = desc->AddField("", "int", "nFaceVertexIndices", "");
	field->offset = (char*)&(pMeshFace->nFaceVertexIndices) - (char*)pMeshFace;
	field = desc->AddField("vector", "int", "faceVertexIndices", "");
	field->offset = (char*)&(pMeshFace->faceVertexIndices) - (char*)pMeshFace;
	db->RegisterDesc(desc);
	
	Mesh* pMesh = NULL;
	desc = DBG_NEW UTTypeDesc("Mesh");
	desc->size = sizeof(Mesh);
	desc->access = DBG_NEW UTAccess<Mesh>;
	field = desc->AddField("", "int", "nVertices", "");
	field->offset = (char*)&(pMesh->nVertices) - (char*)pMesh;
	field = desc->AddField("vector", "Vec3f", "vertices", "");
	field->offset = (char*)&(pMesh->vertices) - (char*)pMesh;
	field = desc->AddField("", "int", "nFaces", "");
	field->offset = (char*)&(pMesh->nFaces) - (char*)pMesh;
	field = desc->AddField("vector", "MeshFace", "faces", "");
	field->offset = (char*)&(pMesh->faces) - (char*)pMesh;
	db->RegisterDesc(desc);
	
	MeshNormals* pMeshNormals = NULL;
	desc = DBG_NEW UTTypeDesc("MeshNormals");
	desc->size = sizeof(MeshNormals);
	desc->access = DBG_NEW UTAccess<MeshNormals>;
	field = desc->AddField("", "int", "nNormals", "");
	field->offset = (char*)&(pMeshNormals->nNormals) - (char*)pMeshNormals;
	field = desc->AddField("vector", "Vec3f", "normals", "");
	field->offset = (char*)&(pMeshNormals->normals) - (char*)pMeshNormals;
	field = desc->AddField("", "int", "nFaceNormals", "");
	field->offset = (char*)&(pMeshNormals->nFaceNormals) - (char*)pMeshNormals;
	field = desc->AddField("vector", "MeshFace", "faceNormals", "");
	field->offset = (char*)&(pMeshNormals->faceNormals) - (char*)pMeshNormals;
	db->RegisterDesc(desc);
	
	MeshMaterialList* pMeshMaterialList = NULL;
	desc = DBG_NEW UTTypeDesc("MeshMaterialList");
	desc->size = sizeof(MeshMaterialList);
	desc->access = DBG_NEW UTAccess<MeshMaterialList>;
	field = desc->AddField("", "int", "nMaterials", "");
	field->offset = (char*)&(pMeshMaterialList->nMaterials) - (char*)pMeshMaterialList;
	field = desc->AddField("", "int", "nFaceIndexes", "");
	field->offset = (char*)&(pMeshMaterialList->nFaceIndexes) - (char*)pMeshMaterialList;
	field = desc->AddField("vector", "int", "faceIndexes", "");
	field->offset = (char*)&(pMeshMaterialList->faceIndexes) - (char*)pMeshMaterialList;
	db->RegisterDesc(desc);
	
	MeshTextureCoords* pMeshTextureCoords = NULL;
	desc = DBG_NEW UTTypeDesc("MeshTextureCoords");
	desc->size = sizeof(MeshTextureCoords);
	desc->access = DBG_NEW UTAccess<MeshTextureCoords>;
	field = desc->AddField("", "int", "nTextureCoords", "");
	field->offset = (char*)&(pMeshTextureCoords->nTextureCoords) - (char*)pMeshTextureCoords;
	field = desc->AddField("vector", "Vec2f", "textureCoords", "");
	field->offset = (char*)&(pMeshTextureCoords->textureCoords) - (char*)pMeshTextureCoords;
	db->RegisterDesc(desc);
