#pragma once
#include <fstream>
#include "..\Include\datatypedef.h"

// Auto shift Incrementer in Log output
struct LogAutoInc
{
	static u8 Shift;
	LogAutoInc() { Shift++; }
	~LogAutoInc() { Shift--; }
};

struct LogTextBlock
{
	static bool TextBlockFlag;
	static bool NewBlockFlag; // Text block cannot be enclosed
	
	LogTextBlock()
	{
		TextBlockFlag = true;
		NewBlockFlag = true;
	}

	~LogTextBlock()
	{
		TextBlockFlag = false;
		NewBlockFlag = false;
	}
};

class Logger
{
	static Logger m_logger;
	std::ofstream m_logFile;

	static bool m_newLineFlag;
	LogTextBlock m_block;
	bool m_isNewLine;

	Logger(const std::string& logFile);
	void nonStaticlog(const std::string& msg, u8 shiftCount = 0);

	COPY_FORBID(Logger)
public:
	~Logger();
	static Logger& getLogger();
	static void log(const std::string& msg);
	static void logln(const std::string& msg);
	static void errorMsg(const std::string& msg);
	static void warningMsg(const std::string& msg);
	static void done();

	static std::string value(const std::string& msg);
	static std::string tag(const std::string& msg);
};


// Define Level of logging
/*
	-Log name of FBX Nodes
*/
#define LOGL1

/*
*	Show FBX Texture loading information
*/
#define LOGL2