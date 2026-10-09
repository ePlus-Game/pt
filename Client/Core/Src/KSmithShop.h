#ifndef	KSmithShopH
#define	KSmithShopH

#define CORE_SMITH_SHOP_INVALID_ID -1
#define CORE_SMITH_SHOP_MAX_SHOP_COUNT 30

#include <map>

using namespace std;

class KSmithShop
{
public:
	struct SmithRuleInfo
	{
#ifndef _SERVER
		char	type1[COMMON_CLIENT_MSG_LEN_32];
		char	type2[COMMON_CLIENT_MSG_LEN_32];
		char	type3[COMMON_CLIENT_MSG_LEN_32];
		char	type4[COMMON_CLIENT_MSG_LEN_32];
#endif
		int		ruleId;
		SmithRuleInfo()
		{
#ifndef _SERVER
			memset(type1, 0, sizeof(type1));
			memset(type2, 0, sizeof(type2));
			memset(type3, 0, sizeof(type3));
			memset(type4, 0, sizeof(type4));
#endif
			ruleId = CORE_SMITH_SHOP_INVALID_ID;
		}
		SmithRuleInfo(const SmithRuleInfo& other)
		{
#ifndef _SERVER
			strncpy(type1, other.type1, sizeof(type1));
			strncpy(type2, other.type2, sizeof(type2));
			strncpy(type3, other.type3, sizeof(type3));
			strncpy(type4, other.type4, sizeof(type4));
			type1[sizeof(type1) - 1] = 0;
			type2[sizeof(type2) - 1] = 0;
			type3[sizeof(type3) - 1] = 0;
			type4[sizeof(type4) - 1] = 0;
#endif
			ruleId = other.ruleId;
		}
	};

#ifndef _SERVER
	typedef vector<SmithRuleInfo> SmithRules;
#else
	typedef map<int, SmithRuleInfo> SmithRules;
#endif
	struct SmithShop
	{
		SmithRules rules;
	};
private:
	SmithShop _shops[CORE_SMITH_SHOP_MAX_SHOP_COUNT];
	bool addNewRule(int shopId, SmithRuleInfo& rule);
public:
	KSmithShop();
	~KSmithShop();
	bool init();
	SmithShop* getShopByIds(int shopId);
	bool isShopHaveTheRule(int shopId, int ruleId);

	static KSmithShop& getSinglton();
#ifndef _SERVER
	void beginSmith(int shopId);
#else
	void beginSmith(int playerIndex, int shopId);
#endif
};


#endif