/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "FileIO.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

#include "FISaveContext.h"
#include "FINodeHandler.h"
#include <fstream>
#include <sstream>

namespace Spr{;

//---------------------------------------------------------------------------
//	FISaveContext
FISaveContext::FISaveContext(){
	errorStream = &DSTR;
}

void FISaveContext::Open(const char* fn){
	file.open(fn);
}
void FISaveContext::Message(const char* msg){
	*errorStream << msg << std::endl;
}
void FISaveContext::ErrorMessage(const char* msg){
	*errorStream << "error: " << msg << std::endl;
}
UTString FISaveContext::GetNodeTypeName(){
	UTString rv(DCAST(Object, objects.back())->GetTypeInfo()->ClassName());
	return rv;
}
UTString FISaveContext::GetNodeName(){
	NamedObjectIf* n = DCAST(NamedObjectIf, objects.back());
	UTString rv;
	if (n && n->GetName()) rv = n->GetName();
	return rv;
}

};

