#include <Foundation/Foundation.h>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

#include "TypeDescDump.h"

namespace Spr{ 
static UTTypeDesc* desc; 
static UTTypeDesc::Field* field; 

void CDRegisterTypeDescs(){
	static bool bFirst=true;
	if (!bFirst) return;
	bFirst = false;

	UTRegisterTypeDescs();

	UTTypeDescDb* db = UTTypeDescDb::theTypeDescDb;
	assert(db);
	#include "TypeDescDumpImp.h"
	db->Link();
}
}
