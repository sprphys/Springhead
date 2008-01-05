/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRFLANIMALGENEDATA_H
#define CRFLANIMALGENEDATA_H

#include <Springhead.h>
#include <Creature/CRFourLegsAnimalBody.h>

using namespace Spr;

class CRFLAnimalGeneData{

public:
	Quaterniond goalDir;
	
	enum QorD{
		GEN_QUATERNIOND=0,
		GEN_DOUBLE
	} geneType;
};

#endif