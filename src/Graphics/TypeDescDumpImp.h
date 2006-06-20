/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
//	Do not edit. MakeTypeDesc.bat will update this file.
	
	GRFrameTransformMatrix* pGRFrameTransformMatrix = NULL;
	desc = DBG_NEW UTTypeDesc("GRFrameTransformMatrix");
	desc->size = sizeof(GRFrameTransformMatrix);
	desc->access = DBG_NEW UTAccess<GRFrameTransformMatrix>;
	field = desc->AddField("", "Affinef", "transform", "");
	field->offset = (char*)&(pGRFrameTransformMatrix->transform) - (char*)pGRFrameTransformMatrix;
	db->RegisterDesc(desc);
	
	GRFrameDesc* pGRFrameDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRFrameDesc");
	desc->size = sizeof(GRFrameDesc);
	desc->ifInfo = GRFrameIf::GetIfInfoStatic();
	((IfInfo*)GRFrameIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRFrameDesc>;
	field = desc->AddField("", "Affinef", "transform", "");
	field->offset = (char*)&(pGRFrameDesc->transform) - (char*)pGRFrameDesc;
	db->RegisterDesc(desc);
	
	GRMeshDesc* pGRMeshDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRMeshDesc");
	desc->size = sizeof(GRMeshDesc);
	desc->ifInfo = GRMeshIf::GetIfInfoStatic();
	((IfInfo*)GRMeshIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRMeshDesc>;
	field = desc->AddField("vector", "Vec3f", "positions", "");
	field->offset = (char*)&(pGRMeshDesc->positions) - (char*)pGRMeshDesc;
	field = desc->AddField("vector", "Vec3f", "normals", "");
	field->offset = (char*)&(pGRMeshDesc->normals) - (char*)pGRMeshDesc;
	field = desc->AddField("vector", "Vec4f", "colors", "");
	field->offset = (char*)&(pGRMeshDesc->colors) - (char*)pGRMeshDesc;
	field = desc->AddField("vector", "Vec2f", "texCoords", "");
	field->offset = (char*)&(pGRMeshDesc->texCoords) - (char*)pGRMeshDesc;
	field = desc->AddField("vector", "size_t", "faces", "");
	field->offset = (char*)&(pGRMeshDesc->faces) - (char*)pGRMeshDesc;
	db->RegisterDesc(desc);
	
	GRLightDesc* pGRLightDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRLightDesc");
	desc->size = sizeof(GRLightDesc);
	desc->ifInfo = GRLightIf::GetIfInfoStatic();
	((IfInfo*)GRLightIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRLightDesc>;
	field = desc->AddField("", "Vec4f", "ambient", "");
	field->offset = (char*)&(pGRLightDesc->ambient) - (char*)pGRLightDesc;
	field = desc->AddField("", "Vec4f", "diffuse", "");
	field->offset = (char*)&(pGRLightDesc->diffuse) - (char*)pGRLightDesc;
	field = desc->AddField("", "Vec4f", "specular", "");
	field->offset = (char*)&(pGRLightDesc->specular) - (char*)pGRLightDesc;
	field = desc->AddField("", "Vec4f", "position", "");
	field->offset = (char*)&(pGRLightDesc->position) - (char*)pGRLightDesc;
	field = desc->AddField("", "float", "range", "");
	field->offset = (char*)&(pGRLightDesc->range) - (char*)pGRLightDesc;
	field = desc->AddField("", "float", "attenuation0", "");
	field->offset = (char*)&(pGRLightDesc->attenuation0) - (char*)pGRLightDesc;
	field = desc->AddField("", "float", "attenuation1", "");
	field->offset = (char*)&(pGRLightDesc->attenuation1) - (char*)pGRLightDesc;
	field = desc->AddField("", "float", "attenuation2", "");
	field->offset = (char*)&(pGRLightDesc->attenuation2) - (char*)pGRLightDesc;
	field = desc->AddField("", "Vec3f", "spotDirection", "");
	field->offset = (char*)&(pGRLightDesc->spotDirection) - (char*)pGRLightDesc;
	field = desc->AddField("", "float", "spotFalloff", "");
	field->offset = (char*)&(pGRLightDesc->spotFalloff) - (char*)pGRLightDesc;
	field = desc->AddField("", "float", "spotInner", "");
	field->offset = (char*)&(pGRLightDesc->spotInner) - (char*)pGRLightDesc;
	field = desc->AddField("", "float", "spotCutoff", "");
	field->offset = (char*)&(pGRLightDesc->spotCutoff) - (char*)pGRLightDesc;
	db->RegisterDesc(desc);
	
	GRMaterialDesc* pGRMaterialDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRMaterialDesc");
	desc->size = sizeof(GRMaterialDesc);
	desc->ifInfo = GRMaterialIf::GetIfInfoStatic();
	((IfInfo*)GRMaterialIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRMaterialDesc>;
	field = desc->AddField("", "Vec4f", "ambient", "");
	field->offset = (char*)&(pGRMaterialDesc->ambient) - (char*)pGRMaterialDesc;
	field = desc->AddField("", "Vec4f", "diffuse", "");
	field->offset = (char*)&(pGRMaterialDesc->diffuse) - (char*)pGRMaterialDesc;
	field = desc->AddField("", "Vec4f", "specular", "");
	field->offset = (char*)&(pGRMaterialDesc->specular) - (char*)pGRMaterialDesc;
	field = desc->AddField("", "Vec4f", "emissive", "");
	field->offset = (char*)&(pGRMaterialDesc->emissive) - (char*)pGRMaterialDesc;
	field = desc->AddField("", "float", "power", "");
	field->offset = (char*)&(pGRMaterialDesc->power) - (char*)pGRMaterialDesc;
	field = desc->AddField("", "string", "texture", "");
	field->offset = (char*)&(pGRMaterialDesc->texture) - (char*)pGRMaterialDesc;
	db->RegisterDesc(desc);
	
	GRCameraDesc* pGRCameraDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRCameraDesc");
	desc->size = sizeof(GRCameraDesc);
	desc->ifInfo = GRCameraIf::GetIfInfoStatic();
	((IfInfo*)GRCameraIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRCameraDesc>;
	field = desc->AddField("", "Vec2f", "size", "");
	field->offset = (char*)&(pGRCameraDesc->size) - (char*)pGRCameraDesc;
	field = desc->AddField("", "Vec2f", "center", "");
	field->offset = (char*)&(pGRCameraDesc->center) - (char*)pGRCameraDesc;
	field = desc->AddField("", "float", "front", "");
	field->offset = (char*)&(pGRCameraDesc->front) - (char*)pGRCameraDesc;
	field = desc->AddField("", "float", "back", "");
	field->offset = (char*)&(pGRCameraDesc->back) - (char*)pGRCameraDesc;
	db->RegisterDesc(desc);
	
	GRSceneDesc* pGRSceneDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRSceneDesc");
	desc->size = sizeof(GRSceneDesc);
	desc->ifInfo = GRSceneIf::GetIfInfoStatic();
	((IfInfo*)GRSceneIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRSceneDesc>;
	db->RegisterDesc(desc);
	
	GRSdkDesc* pGRSdkDesc = NULL;
	desc = DBG_NEW UTTypeDesc("GRSdkDesc");
	desc->size = sizeof(GRSdkDesc);
	desc->ifInfo = GRSdkIf::GetIfInfoStatic();
	((IfInfo*)GRSdkIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<GRSdkDesc>;
	db->RegisterDesc(desc);
	
	GRVertexElement* pGRVertexElement = NULL;
	desc = DBG_NEW UTTypeDesc("GRVertexElement");
	desc->size = sizeof(GRVertexElement);
	desc->access = DBG_NEW UTAccess<GRVertexElement>;
	field = desc->AddField("", "short", "stream", "");
	field->offset = (char*)&(pGRVertexElement->stream) - (char*)pGRVertexElement;
	field = desc->AddField("", "short", "offset", "");
	field->offset = (char*)&(pGRVertexElement->offset) - (char*)pGRVertexElement;
	field = desc->AddField("", "char", "type", "");
	field->offset = (char*)&(pGRVertexElement->type) - (char*)pGRVertexElement;
	field = desc->AddField("", "char", "method", "");
	field->offset = (char*)&(pGRVertexElement->method) - (char*)pGRVertexElement;
	field = desc->AddField("", "char", "usage", "");
	field->offset = (char*)&(pGRVertexElement->usage) - (char*)pGRVertexElement;
	field = desc->AddField("", "char", "usageIndex", "");
	field->offset = (char*)&(pGRVertexElement->usageIndex) - (char*)pGRVertexElement;
	db->RegisterDesc(desc);
	
	GRVertexArray* pGRVertexArray = NULL;
	desc = DBG_NEW UTTypeDesc("GRVertexArray");
	desc->size = sizeof(GRVertexArray);
	desc->access = DBG_NEW UTAccess<GRVertexArray>;
	field = desc->AddField("", "GRVertexElement", "format", "");
	field->offset = (char*)&(pGRVertexArray->format) - (char*)pGRVertexArray;
	field = desc->AddField("pointer", "void", "buffer", "");
	field->offset = (char*)&(pGRVertexArray->buffer) - (char*)pGRVertexArray;
	db->RegisterDesc(desc);
