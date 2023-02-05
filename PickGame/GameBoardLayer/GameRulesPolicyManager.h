#pragma once
#include "GameRulesPolicy.h"
#include "GameRulesPolicyReader.h"
#include <vector>

class GameRulesPolicyManager
{
	GameRulesPolicyReader m_reader;
	std::vector<GameRulesPolicy> m_rulesPolicy;
	GameRulesPolicy m_defaultPolicy;

	void buildDefaultPolicy();
public:
	GameRulesPolicyManager();
	~GameRulesPolicyManager();

	void loadPolicies(const std::string& folderName);
	const GameRulesPolicy& getPolicy(u8 policyId);
	const std::vector<GameRulesPolicy>& getAllPolicies();
};