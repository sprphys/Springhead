/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "UTLoadContext.h"
#include "UTLoadHandler.h"
#include "UTTypeDesc.h"
#include <Base/Affine.h>
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

namespace Spr{;

//---------------------------------------------------------------------------
//	UTFileMap
bool UTFileMap::IsGood(){
	return start && end && (end != (char*)-1);
}


//---------------------------------------------------------------------------
//	UTLoadedData
UTLoadedData::UTLoadedData(UTLoadContext* fc, UTTypeDesc* t, void* d): parent(NULL), 
	type(t),data(d), haveData(false), man(NULL){
	if (!data) SetType(t);
	if (fc && fc->fileMaps.size()){
		fileInfo = fc->fileMaps.Top();
		filePos = fc->fileMaps.Top()->curr;
	}else{
		fileInfo = NULL;
		filePos = NULL;
	}
}
UTLoadedData::~UTLoadedData(){
	if (haveData){
		type->Delete(data);
		data = NULL;
	}
}
UTString UTLoadedData::GetName() const {
	return GetAttribute("name");
}
bool UTLoadedData::HasAttribute(UTString key) const {
	std::map<UTString, UTString>::const_iterator it;
	it = attributes.find(key);
	return it != attributes.end();
}
UTString UTLoadedData::GetAttribute(UTString key) const {
	std::map<UTString, UTString>::const_iterator it;
	it = attributes.find(key);
	if (it != attributes.end()) return it->second;
	return "";
}
void UTLoadedData::SetAttribute(UTString key, UTString val){
	attributes[key] = val;
}
void UTLoadedData::SetName(UTString n){
	attributes["name"] = n;
	UTLoadedData* ld = this;
	while(ld && !ld->nameMan) ld = ld->parent;
	if (ld){
		ld->nameMan->AddData(this);
		man = ld;
	}
}
void UTLoadedData::SetType(UTTypeDesc* t){
	if (data && type){
		type->Delete(data);
		data = NULL;
		haveData = false;
	}
	type = t;
	if (type){
		data = type->Create();
		haveData = true;
	}
}
void UTLoadedData::SetupNameManager(){
	if (type && type->ifInfo && type->ifInfo->Inherit(NameManagerIf::GetIfInfoStatic())){
		nameMan = DBG_NEW UTNameManagerForData;
		nameMan->data = this;
		UTLoadedData* ld = parent;
		while(ld && !ld->nameMan) ld = ld->parent;
		if (ld){
			nameMan->parent = ld->nameMan;
			ld->nameMan->childManagers.push_back(nameMan);
		}
	}
}
void UTLoadedData::AddLink(UTLoadedData* to){
	linkTo.push_back(to);
	to->linkFrom.push_back(this);
}
void UTLoadedData::AddChild(UTLoadedData* c){
	children.push_back(c);
	c->parent = this;
}
void UTLoadContext::LoadedDatas::Print(std::ostream& os){
	int w = os.width();
	os.width(0);
	os << UTPadding(w) << "--- Loaded data (desc) tree ------------------------" << std::endl;
	for(iterator it = begin(); it!=end(); ++it) (*it)->Print(os);
	os << "----------------------------------------------------" << std::endl;
	os.width(w);
}
void UTLoadedData::Print(std::ostream& os){
	UTString typeStr;
	if (type) typeStr = type->GetTypeName();
	else typeStr = UTString("(") + GetAttribute("type") + UTString(")");

	int w = os.width();
	os.width(0);
	os << UTPadding(w) << "<" << typeStr;
	for(Attributes::iterator it = attributes.begin(); it!=attributes.end(); ++it){
		if (it->first.compare("type")!=0){
			os << " " << it->first << "=" << it->second;
		}
	}
	os << (haveData ? " haveData" : "") << ">" << std::endl;
	if (linkTo.size()){
		os << UTPadding(w+2) << "linkTo = ";
		for(unsigned i=0; i<linkTo.size(); ++i) os << " " << linkTo[i]->GetName();
		os << std::endl;
	}
	if (linkFrom.size()){
		os << UTPadding(w+2) << "linkFrom = ";
		for(unsigned i=0; i<linkFrom.size(); ++i) os << " " << linkFrom[i]->GetName();
		os << std::endl;
	}
	for(unsigned i=0; i<children.size(); ++i){
		os.width(w+2);
		children[i]->Print(os);
		os.width(0);
	}
	if (str.length()){
		os << UTPadding(w+2) << "str=\"" << str << "\"" << std::endl;
	}
	os << UTPadding(w) << "</" << typeStr << ">" << std::endl;
	os.width(w);
}

UTLoadedData* UTLoadedData::FindAncestor(UTString tn){
	UTLoadedData* rv = parent;
	while(rv && rv->type->GetTypeName().compare(tn)!=0) rv = rv->parent;
	return rv;
}
UTLoadedData* UTLoadedData::FindDescendant(UTString tn){
	for(unsigned i=0; i<children.size(); ++i){
		if (children[i]->type->GetTypeName().compare(tn) == 0) return children[i];
	}
	for(unsigned i=0; i<children.size(); ++i){
		UTLoadedData* rv = children[i]->FindDescendant(tn);
		if (rv) return rv;
	}
	return NULL;
}
UTLoadedData* UTLoadedData::FindLinkAncestor(UTString tn){
	for(unsigned i=0; i<linkFrom.size(); ++i){
		if (linkFrom[i]->type->GetTypeName().compare(tn) == 0) return linkFrom[i];
	}
	for(unsigned i=0; i<linkFrom.size(); ++i){
		UTLoadedData* rv = linkFrom[i]->FindLinkAncestor(tn);
		if (rv) return rv;
	}
	return NULL;
}
UTLoadedData* UTLoadedData::FindLinkDescendant(UTString tn){
	for(unsigned i=0; i<linkTo.size(); ++i){
		if (linkTo[i]->type->GetTypeName().compare(tn) == 0) return linkTo[i];
	}
	for(unsigned i=0; i<linkTo.size(); ++i){
		UTLoadedData* rv = linkTo[i]->FindLinkDescendant(tn);
		if (rv) return rv;
	}
	return NULL;
}

void UTLoadedData::EnumLinkAncestor(std::vector<UTLoadedDatas>& res, UTString tn){
	if (type->GetTypeName().compare(tn) == 0){
		res.push_back(UTLoadedDatas());
		res.back().push_back(this);
		return;
	}
	int nRes = res.size();
	if (parent){
		parent->EnumLinkAncestor(res, tn);
		for(unsigned r=nRes; r<res.size(); ++r){
			res[r].push_back(this);
		}
	}
	for(unsigned i=0; i<linkFrom.size(); ++i){
		unsigned nRes = res.size();
		linkFrom[i]->EnumLinkAncestor(res, tn);
		for(unsigned r=nRes; r<res.size(); ++r){
			res[r].push_back(this);
		}
	}
}
void UTLoadedData::EnumLinkDescendant(std::vector<UTLoadedDatas>& res, UTString tn){
	if (type->GetTypeName().compare(tn) == 0){
		res.push_back(UTLoadedDatas());
		res.back().push_back(this);
		return;
	}
	for(unsigned i=0; i<children.size(); ++i){
		unsigned nRes = res.size();
		children[i]->EnumLinkDescendant(res, tn);
		for(unsigned r=nRes; r<res.size(); ++r){
			res[r].push_back(this);
		}
	}
	for(unsigned i=0; i<linkTo.size(); ++i){
		unsigned nRes = res.size();
		linkTo[i]->EnumLinkDescendant(res, tn);
		for(unsigned r=nRes; r<res.size(); ++r){
			res[r].push_back(this);
		}
	}
}
/*
bool UTLoadedData::FindRoute(UTLoadedDatas& res, UTLoadedData* to, bool upward=true){
	if (to == this){
		res.push_back(this);
		return true;
	}
	if (upward){
		if (parent->FindRoute(res, to, upward)){
			res.push_back(this);
			return true;
		}
		for(UTLoadedDatas::iterator it = linkFrom.begin(); it != linkFrom.end(); ++it){
			if ((*it)->FindRoute(res, to, upward)){
				res.push_back(this);
				return true;
			}
		}
	}else{
		for(UTLoadedDataRefs::iterator it = children.begin(); it != children.end(); ++it){
			if ((*it)->FindRoute(res, to, upward)){
				res.push_back(this);
				return true;
			}
		}
		for(UTLoadedDatas::iterator it = linkTo.begin(); it != linkTo.end(); ++it){
			if ((*it)->FindRoute(res, to, upward)){
				res.push_back(this);
				return true;
			}
		}
	}
	return false;
}
*/

//---------------------------------------------------------------------------
//	UTLoadTasks
OBJECT_IMP(UTLoadTask, NamedObject);

void UTLoadTasks::Execute(UTLoadContext* ctx){
	for(iterator it = begin(); it!=end(); ++it){
		(*it)->Execute(ctx);
	}
	clear();
}

//---------------------------------------------------------------------------
//	UTDataLinkTask
///	ノードへの参照を記録しておくクラス．全部ロードできてからリンクする．
class UTDataLinkTask: public UTLoadTask{
public:
	//@name	データファイルのどこのデータか
	//@{
	UTRef<UTFileMap> info;	///<	ファイル
	const char* pos;		///<	位置
	//@}

	UTLoadedData* linkFrom;	///<	リンク元データ
	std::string linkTo;		///<	リンク先データの名前

	UTDataLinkTask(UTLoadedData* from, std::string to, UTFileMap* info, const char* p);
	void Execute(UTLoadContext* ctx);
};

UTDataLinkTask::UTDataLinkTask(UTLoadedData* from, std::string to, UTFileMap* fm, const char* p):
	linkFrom(from), linkTo(to), info(fm), pos(p){
}

void UTDataLinkTask::Execute(UTLoadContext* ctx){
	UTLoadedData* refData = NULL;
	UTLoadedData* ld = linkFrom;
	while (ld && !ld->nameMan){
		ld = ld->parent;
	}
	if (ld) refData = ld->nameMan->FindData(linkTo);
	if (refData){
		linkFrom->AddLink(refData);
	}else{
		std::string err("Referenced data '");
		err.append(linkTo);
		err.append("' not found.");
		ctx->ErrorMessage(info, pos, err.c_str());
	}
}

//---------------------------------------------------------------------------
//	UTNameMangerForData
UTNameManagerForData::UTNameManagerForData():parent(NULL), data(NULL){
}

bool UTNameManagerForData::AddData(UTLoadedData* data){
	if (!data->GetName().length()) return false;
	std::pair<DataSet::iterator, bool> rv = dataSet.insert(data);
	if (rv.second){
		return true;
	}else if (*rv.first == data){
		return false;
	}
	UTString base = data->GetName();
	int i=1;
	do{
		std::ostringstream ss;
		ss << "_" << i << '\0';
		data->SetName( (base + ss.str()).c_str());
		rv = dataSet.insert(data);
		i++;
	} while(!rv.second);
	nameMap[base] = data->GetName();
	return true;
}
UTLoadedData* UTNameManagerForData::FindData(UTString name, UTString cls){
	//	自分と子孫を探す。
	UTLoadedData* rv = FindDataFromDescendant(name, cls);
	if (rv) return rv;
	//	先祖を探す．
	rv = FindDataFromAncestor(name, cls);
	if (rv) return rv;

	//	それでもないならば、namespaceを削って、もう一度検索
	int pos = name.find('/');
	if (pos != UTString::npos){	//	 名前空間の指定がある場合
		UTString n = name.substr(pos+1);
		rv = FindData(n, cls);
	}
	return rv;
}
//	先祖を探す
UTLoadedData* UTNameManagerForData::FindDataFromAncestor(UTString name, UTString cls){
	//	なければ祖先を探す。
	UTNameManagerForData* nm = parent;
	while(nm){
		UTLoadedData* rv = nm->SearchSet(name, cls);	//	まず親を探し、
		if (rv) return rv;
		//	兄弟を探し、
		for(UTNameManagerForData::NameManagers::iterator it = nm->childManagers.begin(); 
			it!=nm->childManagers.end(); ++it){
			if (*it != this){
				rv = (*it)->FindDataFromDescendant(name, cls);
				if (rv) return rv;
			}
		}
		//	なければ、親の親を探す。
		nm = nm->parent;
	}
	return NULL;
}

//	自分と子孫を探す
UTLoadedData* UTNameManagerForData::FindDataFromDescendant(UTString name, UTString cls){
//	DSTR << "UTNameManagerForData(" << GetName() << ")::FindDescendant search " << name << std::endl;
	//	ぴったりのものを探す
	UTLoadedData* rv = FindDataExact(name, cls);
	if (rv) return rv;
	//	なければ，子孫について探す
	for(UTNameManagerForData::NameManagers::iterator it = childManagers.begin();
		it != childManagers.end(); ++it){
		rv = (*it)->FindDataFromDescendant(name, cls);
		if (rv) return rv;
	}
	return rv;
}
//	ネームスペース込みで名前を検索する。検索場所については再帰なし。
UTLoadedData* UTNameManagerForData::FindDataExact(UTString name, UTString cls){
	UTLoadedData* rv = NULL;
	int pos = name.find('/');
	if (pos != UTString::npos){	//	 名前空間の指定がある場合
		UTString n = name.substr(pos+1);
		UTString ns = name.substr(0, pos);
		//	ぴったりのものを探す．
		for(UTNameManagerForData::NameManagers::iterator it = childManagers.begin();
			it != childManagers.end(); ++it){
			if (ns.compare((*it)->data->GetName()) == 0){
				rv = (*it)->FindDataExact(n, cls);
				if (rv) return rv;
			}
		}
	}else{	//	名前空間が無い場合
		//	ぴったりのものを探す
		rv = SearchSet(name, cls);
	}
	return rv;
}
UTLoadedData* UTNameManagerForData::SearchSet(UTString name, UTString cls){
	static UTLoadedData key(NULL, 0, NULL);
	key.SetName(name.c_str());
	UTNameManagerForData::DataSet::iterator it = dataSet.find(&key);
	if (it != dataSet.end()) return *it;
	return NULL;
}

//---------------------------------------------------------------------------
//	UTLoadContext
UTLoadContext::UTLoadContext():errorStream(NULL){
	errorStream=&DSTR;
	rootNameManagerForData = DBG_NEW UTLoadedData(NULL, NULL);
	rootNameManagerForData->nameMan = DBG_NEW UTNameManagerForData;
	rootNameManagerForData->nameMan->data = rootNameManagerForData;
	typeDbs.Push(DBG_NEW UTTypeDescDb);
	handlerDbs.Push(DBG_NEW UTLoadHandlerDb);
}
void UTLoadContext::WriteBool(bool v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	curField.field->WriteBool(datas.Top()->data, v, curField.arrayPos);
}
void UTLoadContext::WriteNumber(double v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	curField.field->WriteNumber(datas.Top()->data, v, curField.arrayPos);
}
void UTLoadContext::WriteString(std::string v){
	UTTypeDescFieldIt& curField = fieldIts.back();
	curField.field->WriteString(datas.Top()->data, v.c_str(), curField.arrayPos);
}
void UTLoadContext::PushType(UTString tn){
	UTTypeDesc* type = typeDbs.Top()->Find(tn);
	if (!type) type = typeDbs.Top()->Find(tn + "Desc");	
	if (!type){
		UTString msg = tn;
		msg.append(" not defined.");
		Message(NULL, NULL, msg.c_str());
	}

	//	ロードすべきtypeとしてセット
	fieldIts.PushType(type);
	//	typeにあったDescのノード(DOMノード)を用意
	UTLoadedData* data = DBG_NEW UTLoadedData(this, type);
	//	型名を設定
	data->SetAttribute("type", type ? type->GetTypeName() : tn);

	//	データツリーに追加
	if (datas.size()){
		datas.Top()->AddChild(data);	//	子データとして追加
	}else{
		loadedDatas.push_back(data);	//	Topのデータとして記録しておく．
		rootNameManagerForData->AddChild(data);
	}
	
	//	名前管理ツリーに追加
	data->SetupNameManager();

	//	データを子ノードのロード用にスタックに積んでおく．
	datas.Push(data);
}
void UTLoadContext::PopType(){
	datas.Pop();
	fieldIts.Pop();
}
bool UTLoadContext::IsGood(){
	if (!fileMaps.size()) return false;
	return fileMaps.Top()->IsGood();
}
void UTLoadContext::AddDataLink(std::string ref, const char* pos){
	dataLinks.push_back(DBG_NEW UTDataLinkTask(datas.Top(), ref, fileMaps.Top(), pos));
}
void UTLoadContext::LinkData(){
	dataLinks.Execute(this);
	dataLinks.clear();
}
void UTLoadContext::LinkNode(UTLoadedData* ld){
	for (ObjectIfs::iterator o1 = ld->loadedObjects.begin(); o1!=ld->loadedObjects.end(); ++o1){
		for(UTLoadedDatas::iterator ld2 = ld->linkTo.begin(); ld2 != ld->linkTo.end(); ++ld2){
			for (ObjectIfs::iterator o2 = (*ld2)->loadedObjects.begin(); o2!=(*ld2)->loadedObjects.end(); ++o2){
				if (  !(*o1)->AddChildObject( (*o2)->Cast() )  ){
					std::string err("Can not add referenced object '");
					err.append((*ld2)->GetName());
					err.append("' into '");
					err.append(ld->GetName());
					err.append("'.");
					ErrorMessage(ld->fileInfo, ld->filePos, err.c_str());
				}				
			}
		}
	}
	for(UTLoadedDataRefs::iterator it = ld->children.begin(); it != ld->children.end(); ++it){
		LinkNode(*it);
	}
}
void UTLoadContext::LinkNode(){
	for(UTLoadedDataRefs::iterator it = loadedDatas.begin(); it!=loadedDatas.end(); ++it){
		LinkNode(*it);
	}
	links.Execute(this);
	links.clear();
}
void UTLoadContext::PostTask(){
	postTasks.Execute(this);
	postTasks.clear();
}

void UTLoadContext::ErrorMessage(UTFileMap* info, const char* pos, const char* msg){
	std::string m("error: ");
	m.append(msg);
	Message(info, pos, m.c_str());
}
void UTLoadContext::Message(UTFileMap* info, const char* pos, const char* msg){
	int lines=0;
	int returns=0;
	if (!info) info = fileMaps.Top();
	const char* ptr = info->start;
	const char* line=ptr;

	if (!pos) pos = info->curr;
	if (pos){
		for(;ptr < pos; ++ptr){
			if (*ptr == '\n'){
				++lines;
				line = ptr+1;
			}
			if (*ptr == '\r'){
				++returns;
				line = ptr+1;
			}
		}
		for(;ptr < info->end; ++ptr){
			if (*ptr == '\n' || *ptr == '\r'){
				break;
			}
		}
		lines = lines > returns ? lines : returns;
	}
	std::ostream& os = *errorStream;
	os << info->name << "(" << lines+1 << ") : ";
	os << msg << std::endl;
	os << std::string(line, ptr) << std::endl;
}
ObjectIf* UTLoadContext::CreateObject(const IfInfo* info,  const void* data, UTString name){
	ObjectIf* obj=NULL;
	for(UTStack<ObjectIf*>::reverse_iterator it = objects.rbegin(); 
		it != objects.rend(); ++it){
		if (*it) obj = (*it)->CreateObject(info, data);
		if (obj) break;
	}
	//	先祖が作れない場合，Sdkの作成をしてみる．
	if (!obj) obj = SdkIf::CreateSdk(info, data);

	if (obj){
		//	オブジェクトに名前を設定
		NamedObjectIf* n = DCAST(NamedObjectIf, obj);
		if (name.length()){
			if (n){
				n->SetName(name.c_str());
			}else{
				UTString err("Can not give name to an object of '");
				err.append(obj->GetIfInfo()->ClassName());
				const IfInfo* i = obj->GetIfInfo();
				const IfInfo* b = NamedObjectIf::GetIfInfoStatic();
				if (i->Inherit(b)){
					DSTR << "i Inherits b.\n";
				}
				err.append("'.");
				ErrorMessage(NULL, NULL, err.c_str());
			}
		}
	}else{
		UTString err("Can not create '");
		err.append(info->ClassName());
		err.append("'. Ancestor objects don't know how to make it.");
		ErrorMessage(NULL, NULL, err.c_str());
	}
	//	親オブジェクトに追加
	if (objects.size() && objects.Top()){
		objects.Top()->AddChildObject(obj);
	}
	return obj;
}

void UTLoadContext::RegisterGroupToDb(const char* gp){
	const char* p = gp;
	while(1){
		const char* end = strchr(p, ' ');
		if (!end) end = gp + strlen(gp);
		if (p < end){
			UTString group(p, end);
			p = end+1;
			*handlerDbs.Top() += *UTLoadHandlerDbPool::Get(group.c_str());
			*typeDbs.Top() += *UTTypeDescDbPool::Get(group.c_str());
		}else{
			break;
		}
	}
}

};
