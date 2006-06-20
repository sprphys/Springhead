/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
//	Do not edit. MakeTypeDesc.bat will update this file.
	
	Vec2f* pVec2f = NULL;
	desc = DBG_NEW UTTypeDesc("Vec2f");
	desc->size = sizeof(Vec2f);
	desc->access = DBG_NEW UTAccess<Vec2f>;
	field = desc->AddField("", "float", "x", "");
	field->offset = (char*)&(pVec2f->x) - (char*)pVec2f;
	field = desc->AddField("", "float", "y", "");
	field->offset = (char*)&(pVec2f->y) - (char*)pVec2f;
	db->RegisterDesc(desc);
	
	Vec2d* pVec2d = NULL;
	desc = DBG_NEW UTTypeDesc("Vec2d");
	desc->size = sizeof(Vec2d);
	desc->access = DBG_NEW UTAccess<Vec2d>;
	field = desc->AddField("", "double", "x", "");
	field->offset = (char*)&(pVec2d->x) - (char*)pVec2d;
	field = desc->AddField("", "double", "y", "");
	field->offset = (char*)&(pVec2d->y) - (char*)pVec2d;
	db->RegisterDesc(desc);
	
	Vec3f* pVec3f = NULL;
	desc = DBG_NEW UTTypeDesc("Vec3f");
	desc->size = sizeof(Vec3f);
	desc->access = DBG_NEW UTAccess<Vec3f>;
	field = desc->AddField("", "float", "x", "");
	field->offset = (char*)&(pVec3f->x) - (char*)pVec3f;
	field = desc->AddField("", "float", "y", "");
	field->offset = (char*)&(pVec3f->y) - (char*)pVec3f;
	field = desc->AddField("", "float", "z", "");
	field->offset = (char*)&(pVec3f->z) - (char*)pVec3f;
	db->RegisterDesc(desc);
	
	Vec3d* pVec3d = NULL;
	desc = DBG_NEW UTTypeDesc("Vec3d");
	desc->size = sizeof(Vec3d);
	desc->access = DBG_NEW UTAccess<Vec3d>;
	field = desc->AddField("", "double", "x", "");
	field->offset = (char*)&(pVec3d->x) - (char*)pVec3d;
	field = desc->AddField("", "double", "y", "");
	field->offset = (char*)&(pVec3d->y) - (char*)pVec3d;
	field = desc->AddField("", "double", "z", "");
	field->offset = (char*)&(pVec3d->z) - (char*)pVec3d;
	db->RegisterDesc(desc);
	
	Vec4f* pVec4f = NULL;
	desc = DBG_NEW UTTypeDesc("Vec4f");
	desc->size = sizeof(Vec4f);
	desc->access = DBG_NEW UTAccess<Vec4f>;
	field = desc->AddField("", "float", "x", "");
	field->offset = (char*)&(pVec4f->x) - (char*)pVec4f;
	field = desc->AddField("", "float", "y", "");
	field->offset = (char*)&(pVec4f->y) - (char*)pVec4f;
	field = desc->AddField("", "float", "z", "");
	field->offset = (char*)&(pVec4f->z) - (char*)pVec4f;
	field = desc->AddField("", "float", "w", "");
	field->offset = (char*)&(pVec4f->w) - (char*)pVec4f;
	db->RegisterDesc(desc);
	
	Vec4d* pVec4d = NULL;
	desc = DBG_NEW UTTypeDesc("Vec4d");
	desc->size = sizeof(Vec4d);
	desc->access = DBG_NEW UTAccess<Vec4d>;
	field = desc->AddField("", "double", "x", "");
	field->offset = (char*)&(pVec4d->x) - (char*)pVec4d;
	field = desc->AddField("", "double", "y", "");
	field->offset = (char*)&(pVec4d->y) - (char*)pVec4d;
	field = desc->AddField("", "double", "z", "");
	field->offset = (char*)&(pVec4d->z) - (char*)pVec4d;
	field = desc->AddField("", "double", "w", "");
	field->offset = (char*)&(pVec4d->w) - (char*)pVec4d;
	db->RegisterDesc(desc);
	
	Quaternionf* pQuaternionf = NULL;
	desc = DBG_NEW UTTypeDesc("Quaternionf");
	desc->size = sizeof(Quaternionf);
	desc->access = DBG_NEW UTAccess<Quaternionf>;
	field = desc->AddField("", "float", "x", "");
	field->offset = (char*)&(pQuaternionf->x) - (char*)pQuaternionf;
	field = desc->AddField("", "float", "y", "");
	field->offset = (char*)&(pQuaternionf->y) - (char*)pQuaternionf;
	field = desc->AddField("", "float", "z", "");
	field->offset = (char*)&(pQuaternionf->z) - (char*)pQuaternionf;
	field = desc->AddField("", "float", "w", "");
	field->offset = (char*)&(pQuaternionf->w) - (char*)pQuaternionf;
	db->RegisterDesc(desc);
	
	Quaterniond* pQuaterniond = NULL;
	desc = DBG_NEW UTTypeDesc("Quaterniond");
	desc->size = sizeof(Quaterniond);
	desc->access = DBG_NEW UTAccess<Quaterniond>;
	field = desc->AddField("", "double", "x", "");
	field->offset = (char*)&(pQuaterniond->x) - (char*)pQuaterniond;
	field = desc->AddField("", "double", "y", "");
	field->offset = (char*)&(pQuaterniond->y) - (char*)pQuaterniond;
	field = desc->AddField("", "double", "z", "");
	field->offset = (char*)&(pQuaterniond->z) - (char*)pQuaterniond;
	field = desc->AddField("", "double", "w", "");
	field->offset = (char*)&(pQuaterniond->w) - (char*)pQuaterniond;
	db->RegisterDesc(desc);
	
	Posef* pPosef = NULL;
	desc = DBG_NEW UTTypeDesc("Posef");
	desc->size = sizeof(Posef);
	desc->access = DBG_NEW UTAccess<Posef>;
	field = desc->AddField("", "float", "w", "");
	field->offset = (char*)&(pPosef->w) - (char*)pPosef;
	field = desc->AddField("", "float", "x", "");
	field->offset = (char*)&(pPosef->x) - (char*)pPosef;
	field = desc->AddField("", "float", "y", "");
	field->offset = (char*)&(pPosef->y) - (char*)pPosef;
	field = desc->AddField("", "float", "z", "");
	field->offset = (char*)&(pPosef->z) - (char*)pPosef;
	field = desc->AddField("", "float", "px", "");
	field->offset = (char*)&(pPosef->px) - (char*)pPosef;
	field = desc->AddField("", "float", "py", "");
	field->offset = (char*)&(pPosef->py) - (char*)pPosef;
	field = desc->AddField("", "float", "pz", "");
	field->offset = (char*)&(pPosef->pz) - (char*)pPosef;
	db->RegisterDesc(desc);
	
	Posed* pPosed = NULL;
	desc = DBG_NEW UTTypeDesc("Posed");
	desc->size = sizeof(Posed);
	desc->access = DBG_NEW UTAccess<Posed>;
	field = desc->AddField("", "double", "w", "");
	field->offset = (char*)&(pPosed->w) - (char*)pPosed;
	field = desc->AddField("", "double", "x", "");
	field->offset = (char*)&(pPosed->x) - (char*)pPosed;
	field = desc->AddField("", "double", "y", "");
	field->offset = (char*)&(pPosed->y) - (char*)pPosed;
	field = desc->AddField("", "double", "z", "");
	field->offset = (char*)&(pPosed->z) - (char*)pPosed;
	field = desc->AddField("", "double", "px", "");
	field->offset = (char*)&(pPosed->px) - (char*)pPosed;
	field = desc->AddField("", "double", "py", "");
	field->offset = (char*)&(pPosed->py) - (char*)pPosed;
	field = desc->AddField("", "double", "pz", "");
	field->offset = (char*)&(pPosed->pz) - (char*)pPosed;
	db->RegisterDesc(desc);
	
	Matrix3f* pMatrix3f = NULL;
	desc = DBG_NEW UTTypeDesc("Matrix3f");
	desc->size = sizeof(Matrix3f);
	desc->access = DBG_NEW UTAccess<Matrix3f>;
	field = desc->AddField("", "float", "data", "9");
	field->offset = (char*)&(pMatrix3f->data) - (char*)pMatrix3f;
	db->RegisterDesc(desc);
	
	Matrix3d* pMatrix3d = NULL;
	desc = DBG_NEW UTTypeDesc("Matrix3d");
	desc->size = sizeof(Matrix3d);
	desc->access = DBG_NEW UTAccess<Matrix3d>;
	field = desc->AddField("", "double", "data", "9");
	field->offset = (char*)&(pMatrix3d->data) - (char*)pMatrix3d;
	db->RegisterDesc(desc);
	
	Affinef* pAffinef = NULL;
	desc = DBG_NEW UTTypeDesc("Affinef");
	desc->size = sizeof(Affinef);
	desc->access = DBG_NEW UTAccess<Affinef>;
	field = desc->AddField("", "float", "data", "16");
	field->offset = (char*)&(pAffinef->data) - (char*)pAffinef;
	db->RegisterDesc(desc);
	
	Affined* pAffined = NULL;
	desc = DBG_NEW UTTypeDesc("Affined");
	desc->size = sizeof(Affined);
	desc->access = DBG_NEW UTAccess<Affined>;
	field = desc->AddField("", "double", "data", "16");
	field->offset = (char*)&(pAffined->data) - (char*)pAffined;
	db->RegisterDesc(desc);
