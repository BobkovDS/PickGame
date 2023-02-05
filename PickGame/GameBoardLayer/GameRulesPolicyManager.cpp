#include "GameRulesPolicyManager.h"

GameRulesPolicyManager::GameRulesPolicyManager()
{
	buildDefaultPolicy();
}

GameRulesPolicyManager::~GameRulesPolicyManager()
{
	m_rulesPolicy.clear();
}

void GameRulesPolicyManager::loadPolicies(const std::string& folderName)
{
	m_rulesPolicy.clear();
	m_reader.readPolicies(folderName, m_rulesPolicy);
}

const GameRulesPolicy& GameRulesPolicyManager::getPolicy(u8 policyID)
{
	if ((policyID >= m_rulesPolicy.size()) | m_rulesPolicy.size() == 0)
		return m_defaultPolicy;
	else
		return m_rulesPolicy[policyID];
}

void GameRulesPolicyManager::buildDefaultPolicy()
{
	m_defaultPolicy.DifficultyLevel = 1;
	m_defaultPolicy.LinesCount = 7;
	m_defaultPolicy.PicksInLineCount = 5;
	m_defaultPolicy.SameColorCount = 1;
	m_defaultPolicy.Name = "Default rules Policy";
}

const std::vector<GameRulesPolicy>& GameRulesPolicyManager::getAllPolicies()
{
	return m_rulesPolicy;
}