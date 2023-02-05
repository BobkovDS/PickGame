#include "..\Others\Logger.h"
#include "GameRulesPolicyReader.h"
#include <filesystem>
#include <sstream>
#include <iostream>

using namespace std;

GameRulesPolicyReader::GameRulesPolicyReader()
{
}

GameRulesPolicyReader::~GameRulesPolicyReader()
{
}


void GameRulesPolicyReader::readPolicies(const std::string& folderName, std::vector<GameRulesPolicy>& policies)
{
	namespace fs = std::filesystem;
	string msg;
	
	LogTextBlock textBlock;

	msg = "Reading rules policy from folder: " + Logger::value(folderName);
	Logger::logln(msg);

	m_tmpPolicesStorePtr = &policies;

	vector<string> policyFileNames;

	try
	{
		auto fileIterator = fs::directory_iterator(folderName);
		if (fileIterator._At_end())
			Logger::logln("No rules policies files found");
		else
		{
			for (auto& entry : fileIterator)
			{
				if (entry.path().string().find(FILE_NAME_TEMPLATE) != string::npos)
					policyFileNames.push_back(entry.path().string());
			}
		}

	}
	catch (fs::filesystem_error& err)
	{

		Logger::log(err.what());
	}
	
	
	for (string& fileName : policyFileNames)
		readPolicyFile(fileName);
}

void GameRulesPolicyReader::readPolicyFile(const string& fileName)
{
	LogAutoInc autoInc;
	Logger::logln("reading policy file: " + Logger::value(fileName));

	ifstream inFile(fileName);
	
	if (inFile)
	{		
		char buf[100];

		while (inFile.getline(buf, 100))
		{
			istringstream fileLineContent(buf);
			LogAutoInc lai;
			Logger::logln(fileLineContent.str());

			int pickCount =-1;
			int lineCount =-1;
			int sameColor =-1;
			int level =-1;
			string name = "defname";

			fileLineContent >> pickCount;
			fileLineContent >> lineCount;
			fileLineContent >> sameColor;
			fileLineContent >> level;
			fileLineContent >> name;

			if ((pickCount == -1) | (lineCount == -1) | (sameColor == -1) | (level == -1) | (name == "defname"))
			{
				LogAutoInc lai;
				Logger::log("<-wrong line");
			}
			else
			{
				GameRulesPolicy rulesPolicy;
				rulesPolicy.PicksInLineCount = pickCount;
				rulesPolicy.LinesCount = lineCount;
				rulesPolicy.SameColorCount = sameColor;
				rulesPolicy.DifficultyLevel = level;
				rulesPolicy.Name = name;
				m_tmpPolicesStorePtr->push_back(rulesPolicy);
			}
		}		
	}
}