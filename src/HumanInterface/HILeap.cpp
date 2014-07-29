/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <HumanInterface/HILeap.h>
//#include <Foundation/UTTimer.h>
#include <Foundation/UTSocket.h>
#include <Windows.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{

bool HILeap::Init(const void* desc) {
	#ifdef USE_LEAP
	
	// Leapmotionを初期化
	leap = new Leap::Controller();
	#endif
	return true;
}

void HILeap::Update(float dt) {
#ifdef USE_LEAP
	if (leap) {
		// Leapmotionからセンシング結果を取得
		Leap::Frame frame = leap->frame(0);

		// Skeletonの不足分を用意
		PrepareSkeleton(frame.hands().count());

		for (int h=0; h<frame.hands().count(); ++h) {
			Leap::Hand hand = frame.hands()[h];

			HISkeleton* skel = skeletons[h]->Cast();

			// 手全体の位置姿勢をセット
			skel->pose.Ori() = ToSpr(hand.basis()) * rotation;
			skel->pose.Pos() = ToSpr(hand.palmPosition()) + center;

			// ボーンを準備
			skel->PrepareBone(20);

			// 各指の位置と方向をセット
			int cnt = 0;

			for(int f=0; f<hand.fingers().count(); f++){
				for(int b = 0; b < 4; b++) {
					Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
					Leap::Bone bone = hand.fingers()[f].bone(boneType);
					DCAST(HIBone,skel->bones[cnt])->position  = ToSpr(bone.center()) + center;
					DCAST(HIBone,skel->bones[cnt])->direction = ToSpr(bone.direction());
					DCAST(HIBone,skel->bones[cnt])->length    = bone.length() * scale;

					cnt++;
				}
			}
		}
	}
#endif
}


UDPInit::UDPInit() {
	gimite::startup_socket();
}

UDPInit::~UDPInit() {
	gimite::cleanup_socket();
}


bool HILeapUDP::Init(const void* desc) {
#ifdef USE_LEAP	
	ProtocolPC::getInstance();
#endif
	return true;
}

bool ProtocolPC::isSame(LeapHand* L1, LeapHand* L2, double distance) {
	return false;
	double l1x = L1->position.x;
	double l2x = L2->position.x;
	int l1id = L1->leapID;
	int l2id = L2->leapID;

	l1x += l1id * DISTANCE;
	l2x += l2id * DISTANCE;

	std::cout << "L1x : " << l1id << std::endl;
	std::cout << "L2x : " << l2id << std::endl;
	//return false;
	double diff = pow(l1x - l2x, 2) + pow(L1->position.y - L2->position.y, 2) + pow(L1->position.z - L2->position.z, 2);
	
	std::cout << "distance : " << diff << std::endl;

	if(diff < distance * distance) {
		std::cout << "true" << std::endl;
		return true;
	}
	else { return false; }
}


void HILeapUDP::Update(float dt) {
#ifdef USE_LEAP
	using namespace std;
	ProtocolPC* ppc = ProtocolPC::getInstance();
	
	std::list<int> currentUsingLeapHandIdList;

	std::list<int> usedLeapHandIdList;
	std::list<int> newLeapHandIdList;
	std::list<int> usingLeapHandIdList;

	//ppc->bufIdLHIds[i].push_back(*it);
	/*
	ppc->usingLeapHandIdList.clear();
	for(int i = 0; i < ppc->bufIdLHIds.size(); i++) {
		for( list<int>::iterator it = ppc->bufIdLHIds[i].begin(); it != ppc->bufIdLHIds[i].end(); it++) {
			ppc->usingLeapHandIdList.push_back(*it);
		}
	}
	*/

	//for(int i = 0; i < ppc->mapIdLeapData.size(); i++) {
	for each ( pair<int, LeapData*> c in ppc->mapIdLeapData ) {
//		LeapData* ld = ppc->mapIdLeapData[i];
		LeapData* ld = c.second;
		for(int j = 0; j < ld->leapFrameBufs[ld->read].recHandsNum; j++) {
			LeapHand* lh = &ld->leapFrameBufs[ld->read].leapHands[j];
			currentUsingLeapHandIdList.push_back(lh->originalLeapHandID);
		}
	}


	std::list<int> cpyCurList = currentUsingLeapHandIdList;


	if(currentUsingLeapHandIdList.size() == 0) {
		//ppc->mapLHIdLeapHand.clear();
		for each( int var in ppc->usingLeapHandIdList ){
			usedLeapHandIdList.push_back(var);
		}
	}
	else {
		for(list<int>::iterator it = ppc->usingLeapHandIdList.begin(); it != ppc->usingLeapHandIdList.end(); it++) {
			list<int>::iterator ii = find(currentUsingLeapHandIdList.begin(), currentUsingLeapHandIdList.end(), *it);


			if(ii == currentUsingLeapHandIdList.end()){
				//not found
				usedLeapHandIdList.push_back(*it);
				//ppc->mapLHIdLeapHand.erase(*it);
			}
			else {
				//found
				usingLeapHandIdList.push_back(*it);
				cpyCurList.remove(*it);
			}
		}
	}

	newLeapHandIdList = cpyCurList;

	//新しいIDを追加する　同じ手なら同じリストに追加する　リストが空なら飛ばす
	
	for(list<int>::iterator it = newLeapHandIdList.begin(); it != newLeapHandIdList.end(); it++) {
		LeapHand* nlh = ppc->mapLHIdLeapHand[*it];
		bool found = false;
		for(int i = 0; i < ppc->bufIdLHIds.size(); i++) {
			if(ppc->bufIdLHIds[i].size()){
				LeapHand* dlh = ppc->mapLHIdLeapHand[ ppc->bufIdLHIds[i].front() ];
				if(ppc->isSame(nlh, dlh, 50)) {
					ppc->bufIdLHIds[i].push_back(*it);
					found = true;
					break;
				}
			}
		}
		if(!found) {
			//同じ手が見つからなかったら空のリストに追加する。
			//空のリストが無かったら新しくリストを作る。
			bool swEmpty = false;
			for(int i = 0; i < ppc->bufIdLHIds.size(); i++) {
				if(ppc->bufIdLHIds[i].size() == 0) {
					ppc->bufIdLHIds[i].push_back(*it);
					swEmpty = true;
					break;
				}
			}
			if(!swEmpty) {
				//リスト追加
				list<int> l;
				l.push_back(*it);
				ppc->bufIdLHIds.push_back(l);
				if(ppc->bufIdLHIds.size() >= 3){ 
				}
			}
		}
	}


	//使い終わったIDを取り除く
	for(list<int>::iterator it = usedLeapHandIdList.begin(); it != usedLeapHandIdList.end(); it++) {
		
		for(int i = 0; i < ppc->bufIdLHIds.size(); i++) {
			//for(int j = 0; j < ppc->bufIdLHIds[i].size(); j++) {
			//if(!ppc->bufIdLHIds[i].empty()){
				//for each(int val in ppc->bufIdLHIds[i]) {
					//if(*it == val) {
						//ppc->bufIdLHIds[i].remove(val);
						//break;
					//}
				//}
				for(list<int>::iterator ii = ppc->bufIdLHIds[i].begin(); ii != ppc->bufIdLHIds[i].end();) {
					if(*it == *ii) {
						ii = ppc->bufIdLHIds[i].erase(ii);
						continue;
					}
					ii++;
				}
			//}
		}
	}

	ppc->usingLeapHandIdList = currentUsingLeapHandIdList;

	
	//for(int i = 0; i < ppc->bufIdLHIds.size(); i++) {

	//int handsCount = readBuf->recHandsNum; 
	int handsCount =  ppc->bufIdLHIds.size();

	// Skeletonの不足分を用意
	PrepareSkeleton(handsCount);

	for(int h = 0; h < handsCount; h++) {
		HISkeleton* skel = skeletons[h]->Cast();
		
		if(ppc->bufIdLHIds[h].size() == 0) {
			//リストが空だった場合ダミーハンド
			skel->PrepareBone(20);
			TVec3<double> dummy(10000, 10000, 10000);
			skel->pose.Pos() = dummy + center;
			int cnt = 0;
			for(int f=0; f < 5; f++){
				for(int b = 0; b < 4; b++) {
					Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
					DCAST(HIBone,skel->bones[cnt])->position  = dummy + center;
					cnt++;
				}
			}
		}
		else {
			//リストから最もconfidence値の高いLeapHandIDを選ぶ
			LeapHand* mostConfLH = ppc->mapLHIdLeapHand[ ppc->bufIdLHIds[h].front() ];
			for each( int val in ppc->bufIdLHIds[h] ) {
//			for(list<int>::iterator it = ppc->bufIdLHIds[h].begin(); it != ppc->bufIdLHIds[h].end(); it++) {
				LeapHand* lh = ppc->mapLHIdLeapHand[val];
				if(mostConfLH->confidence < lh->confidence) {
					mostConfLH = lh;
				}
			}


			// 手全体の位置姿勢をセット
			//skel->pose.Pos() = ToSpr(readBuf->leapHands[h].position) + center;
			//skel->pose.Ori() = ToSprQ(readBuf->leapHands[h].direction) * rotation;

			skel->pose.Pos() = ToSpr(mostConfLH->position) + center;
			skel->pose.Ori() = ToSprQ(mostConfLH->direction) * rotation;

			// ボーンを準備
			skel->PrepareBone(20);

			// 各指の位置と方向をセット
			int cnt = 0;
			Vec3d offset;
			for(int f=0; f<mostConfLH->recFingersNum; f++){
				for(int b = 0; b < 4; b++) {
					Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
					//Leap::Bone bone = readBuf->leapHands[h].leapFingers[f].bone(boneType);
					LeapBone* lb = &mostConfLH->leapFingers[f].bones[b];
					offset.y = lb->position.y;
					offset.z = lb->position.z;
					offset.x = lb->position.x + (mostConfLH->leapID - 1) * DISTANCE;
					DCAST(HIBone,skel->bones[cnt])->position  = ToSpr(offset) + center;
					DCAST(HIBone,skel->bones[cnt])->direction = ToSpr(lb->direction);
					DCAST(HIBone,skel->bones[cnt])->length    = lb->length * scale;
					cnt++;
				}
			}
		}
	}

		
	//for(int i = 0; i < ppc->bufsNum; i++) {
	for(map<int, LeapData*>::iterator it = ppc->mapIdLeapData.begin();
		it != ppc->mapIdLeapData.end();
		it++) {
			LeapData* ld = it->second;
			ld->writeMode = LeapData::WRITING;
			ld->readMode = LeapData::READ_COMP;
	}

	#endif	//USE_LEAP
}

LeapBone::LeapBone(){}
LeapFinger::LeapFinger(){}
LeapHand::LeapHand() : recFingersNum(0), bufID(-1) {}
LeapFrame::LeapFrame() : recHandsNum(0) {}

ProtocolPC::ProtocolPC() {
	UDPInit::getInstance();

	recvPort = 2233;
	nRecv = 0;
	

//	write = 0;
//	keep = 1;
//	read = 2;

//	writeMode = WRITING;
//	readMode = READING;

	/// 受信開始
	recvThread = UTTimerIf::Create();
	recvThread->SetMode(UTTimerIf::THREAD);
	recvThread->SetCallback(ProtocolPC::recvThreadFunc, this);
}

LeapData::LeapData() {
	write = 0;
	keep = 1;
	read = 2;

	writeMode = WRITING;
	readMode = READING;
}

void ProtocolPC::recvThreadFunc(int id, void* arg) {
	using namespace std;
	ProtocolPC* ppc = ProtocolPC::getInstance();
	ppc = (ProtocolPC*)arg;

	std::cout << "Start Listening " << ppc->recvPort << std::endl;
	gimite::socket_address addr;
	gimite::diagram_socket sock(ppc->recvPort);
	static std::vector<char> buff(4096);
	while(true) {
		if(int size = sock.recvfrom(&buff[0], 4096, &addr)) {
			if(size > 0) {
				int leapID = buff[0] - '0';
				if(ppc->mapIdLeapData[leapID] == 0){
					ppc->mapIdLeapData[leapID] = new LeapData;
				}
				LeapData* ld = ppc->mapIdLeapData[leapID];

				ppc->unpackData(buff, ld->leapFrameBufs[ld->write]);
				if(ld->readMode == LeapData::READ_COMP) {
					int k = ld->keep;
					ld->keep = ld->write;
					ld->write = ld->read;
					ld->read = k;
					ld->readMode = LeapData::READING;
				}
				else {
					int w = ld->write;
					ld->write = ld->keep;
					ld->keep = w;

				}
				ld->writeMode = LeapData::WRITE_COMP;
				ppc->nRecv++;
			}
			else {
				DSTR << GetLastError() << std::endl;
			}
		}
		
	}
}

LeapHand::~LeapHand(){}

void ProtocolPC::unpackData(std::vector<char>& buff, LeapFrame& frame) {
	using namespace std;
	/*
	
	データ構造
	
	char leapID

	char 取得した手の数handNum　→　boneの数は4*5*handNum
	int  取得した手のID 
	char 取得した1つ目の手のfingerNum
	float hand1->posX
	float hand1->posY
	float hand1->posZ
	float hand1->dirX
	float hand1->dirY
	float hand1->dirZ
	float hand1->finger1->bone1->posX
	float hand1->finger1->bone1->posY
	float hand1->finger1->bone1->posZ
	float hand1->finger1->bone1->dirX
	float hand1->finger1->bone1->dirY
	float hand1->finger1->bone1->dirZ
	float hand1->finger1->bone1->length

	float hand1->finger1->bone2->posX
	float hand1->finger1->bone2->posY
	float hand1->finger1->bone2->posZ
	float hand1->finger1->bone2->dirX
	float hand1->finger1->bone2->dirY
	float hand1->finger1->bone2->dirZ
	float hand1->finger1->bone2->length


	char 取得した2つ目の手のfingerNum
	float hand2->posX
	float hand2->posY
	float hand2->posZ
	float hand2->dirX
	float hand2->dirY
	float hand2->dirZ
	float hand2->finger1->bone1->posX
	float hand2->finger1->bone1->posY
	float hand2->finger1->bone1->posZ
	float hand2->finger1->bone1->dirX
	float hand2->finger1->bone1->dirY
	float hand2->finger1->bone1->dirZ
	float hand2->finger1->bone1->length

	...

	float hand(handNum)->finger(fingerNum)->bone4->length

	*/
	int offset = 0;
	
	frame.leapID = buff[0] - '0';

	int handsNum = buff[++offset];
	frame.recHandsNum = handsNum;
	if(handsNum > frame.leapHands.size()) { frame.leapHands.resize(handsNum); }
	using namespace std;

	int fingerNum = 0;
	unsigned char ch[12];
	Vec3d v3;

	
	for(int h = 0; h < handsNum; h++) {
		
		for(int c = 0; c < 4; c++) {
			ch[c] = buff[++offset];
		}
		int originalLeapHandID = charToInt(&ch[0]);

		LeapHand* lh = &frame.leapHands[h];

		lh->originalLeapHandID = originalLeapHandID;
		lh->leapID = frame.leapID;

		ProtocolPC* ppc = ProtocolPC::getInstance();
		//LeapHandID -> LeapHand* のマップを作成
		ppc->mapLHIdLeapHand[originalLeapHandID] = lh;

		cout << "leapID: " << lh->leapID << " handPosX: " << lh->position.x << endl;

		fingerNum = buff[++offset];
		lh->recFingersNum = fingerNum;

		for(int c = 0; c < 4 * 3; c++) {
			ch[c] = buff[++offset];
		}
		

		v3 = charToVec3d(&ch[0]);
		
		lh->position = v3;

		
		for(int c = 0; c < 4 * 3; c++) { ch[c] = buff[++offset]; }
		v3 = charToVec3d(ch);
		lh->direction = v3;

		
		for(int f = 0; f < fingerNum; f++){
			for(int b = 0; b < 4; b++){
				for(int c = 0; c < 4 * 3; c++) { ch[c] = buff[++offset]; }
				v3 = charToVec3d(ch);
				lh->leapFingers[f].bones[b].position = v3;

				for(int c = 0; c < 4 * 3; c++) { ch[c] = buff[++offset]; }
				v3 = charToVec3d(ch);
				lh->leapFingers[f].bones[b].direction = v3;

				for(int c = 0; c < 4; c++) { ch[c] = buff[++offset]; }
				lh->leapFingers[f].bones[b].length = charToFloat(ch);
			}
		}	
	}
}

float charToFloat(unsigned char* c) {
	Uni un;

	for(int i = 0; i < 4; i++) {
		un.ch[i] = c[3-i];
	}
	return un.fl;
}

int charToInt(unsigned char* c) {
	Uni un;

	for(int i = 0; i < 4; i++) {
		un.ch[i] = c[3-i];
	}
	return un.i;
}

Vec3d charToVec3d(unsigned char* c) {
	Uni un[3];

	for(int i = 0; i < 4; i++) {
		un[0].ch[i] = c[3-i];
	}


	for(int i = 0; i < 4; i++) { un[1].ch[i] = c[7-i]; }


	for(int i = 0; i < 4; i++) { un[2].ch[i] = c[11-i]; }

	return Vec3d(un[0].fl, un[1].fl, un[2].fl);
}

}	//spr
