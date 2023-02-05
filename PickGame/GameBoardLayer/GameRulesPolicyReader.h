#pragma once
#include "GameRulesPolicy.h"
#include <vector>

#define FILE_NAME_TEMPLATE "_RulesPolicies"

class GameRulesPolicyReader
{
	std::vector<GameRulesPolicy>* m_tmpPolicesStorePtr;
public:
	GameRulesPolicyReader();
	~GameRulesPolicyReader();

	void readPolicies(const std::string& folderName, std::vector<GameRulesPolicy>& polices);
	void readPolicyFile(const std::string& fileName);	
};

