/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
///////////////////////////////////////////////////////
#ifndef CRFLANIMALQL_H
#define CRFLANIMALQL_H
///////////////////////////////////////////////////////

#include <Springhead.h>
#include <Creature/CRFourLegsAnimalBody.h>
#include <vector>
#include <ctime>
#include <cmath>

#include "CRController.h"
#include "CRFLAnimalGene.h"

////////////////////////////////////////////////////////
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief Q学習を行うクラス
*/
class CRFLAnimalQL: public UTRefCount{

private:
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//大域変数の定義	
	CRCreatureIf*			creature;
	std::vector<CRBodyIf*>  crBody;
	
	std::vector<int>					action;			//< 各関節の指令が入る配列
	std::vector<int>					actionNumber;	//< 各関節の取り得るアクション数が入る配列
	std::vector<CRFLAnimalGeneData>		thisTermGene;	//< あるStep()での遺伝子
	
	double					learningRate;	//<	学習率
	double					decreaseRate;	//< 割引率（エージェントの忘却率）
	double					reward;			//< 報酬
	double					qValue;			//< Q値
	double					qValueDash;		//< 規格化されたQ値
	double					penalty;		//< 規格化の際に使用する罰の値
	double					moveRatio;		//< 行動の幅（degree）

	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//CRTryStandingUpControllerからは呼べない関数
	void		Init();			//< 初期化を行う

public:
	//コンストラクタ
	CRFLAnimalQL(CRCreatureIf* charactors){
		creature = charactors;
		Init();
	}
	//デストラクタ
	~CRFLAnimalQL(){
	}
		
	//インタフェースの実装
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//CRTryStandingUpControllerから呼べるインタフェース関数
	void		BoltzmannSelection();										//< ボルツマン選択
	void		EpsilonGreedySelection();									//< ε-greedy選択
	void		SetActionNumber(std::vector<CRFLAnimalGeneData> *aGene);	//< ある遺伝子の各関節が取り得る行動のパターン総数を設定する
	void		SelectAction(std::vector<CRFLAnimalGeneData> *aGene);		//< 行動を決定する
	void		TakeAction(std::vector<CRFLAnimalGeneData> *aGene);			//< アクションを実行する		
	void		EvaluativeFunc();											//< 評価関数
	void		CalcQValueMax();											//< actionを変動させた時のQ値の最大値を調べる
	void		CalcQValueMin();											//< actionを変動させた時のQ値の最小値を調べる
	void		UpdateQValues();											//< Q値の更新を行う
	void		StateValueMax();											//< 各状態の価値の最大値を求める
	void		StateValueMin();											//< 各状態の価値の最小値を求める
	void		CalcQValueDash();											//< Q値の規格化をすることでQ'を求める
	void		FitnessFromQValue();										//< Q値の高いものの適合度を高くする
	void		FitnessFromTimesOfUse();									//< 使用頻度の高いものの適合度を高くする
	
};

#endif