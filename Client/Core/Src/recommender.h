//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008-04-18
//      File_base        : recommender
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 推荐人系统
//
//////////////////////////////////////////////////////////////////////

#ifndef _RECOMMENDER_H_
#define _RECOMMENDER_H_

//推荐人系统
class RecommenderSystem
{
public:
	RecommenderSystem();
	~RecommenderSystem();
	static RecommenderSystem& Singleton();//单件
	
	bool Load();//载入配置表
	int Reward(int playerIndex, int rewardCount);

	int CheckMasterRequirements(int masterPlayerIndex);//是否满足推荐人的要求
	int CheckStudentRequirements(int studentPlayerIndex);//是否满足被推荐人的要求
};

#endif// _RECOMMENDER_H_