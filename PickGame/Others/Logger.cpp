#include "Logger.h"

u8 LogAutoInc::Shift = 0;
bool Logger::m_newLineFlag = false;
bool LogTextBlock::TextBlockFlag = false;
bool LogTextBlock::NewBlockFlag = false;

Logger::Logger(const std::string& fileName): m_isNewLine(true)
{
	m_block.TextBlockFlag = false;
	m_block.NewBlockFlag = false;

	m_logFile.open(fileName);
	
	if (m_logFile)
	{
		m_logFile.setf(std::ios::unitbuf);
		m_logFile << "======= Begin of LOG file ========\n";
	}
}

void Logger::log(const std::string& msg)
{
#if defined(_DEBUG)
	m_newLineFlag = false;
	Logger::getLogger().nonStaticlog(msg, LogAutoInc::Shift);
#endif
}


void Logger::logln(const std::string& msg)
{
#if defined(_DEBUG)
	m_newLineFlag = true;
	Logger::getLogger().nonStaticlog(msg, LogAutoInc::Shift);
#endif
}

void Logger::errorMsg(const std::string& msg)
{
#if defined(_DEBUG)
	std::string result = "<span style = \"color:red\" >"
		+ msg
		+ "</span>";
	
	m_newLineFlag = true;
	Logger::getLogger().nonStaticlog(result);
#endif
}

void Logger::done()
{
#if defined(_DEBUG)
	m_newLineFlag = true;
	std::string result = "<span style = \"color:green\" > *...done* </span>";
	Logger::getLogger().nonStaticlog(result);
#endif
}

std::string Logger::value(const std::string& msg)
{
	return "<span style = \"color:#D7C1B2\">*" + msg + "*</span>";
}

std::string Logger::tag(const std::string& msg)
{
	return "```" + msg + "```";
}
// ----------------------- NON STATIC --------------------------------------------------

void Logger::nonStaticlog(const std::string& msg, u8 shiftCount)
{
#if defined(_DEBUG)
	if (m_logFile)
	{
		// If this msg should be in a block and this msg is a first msg in the block
		// lets print this block
		if (m_block.TextBlockFlag && m_block.NewBlockFlag)
		{
			m_logFile << "------\n>";
			m_block.NewBlockFlag = false;
		}
		int xTimes = m_block.TextBlockFlag ? 2 : 1; // Inside block do Tab twice times more


		// Tab we do only on new line
		for (u8 i = 0; m_isNewLine && i < shiftCount * xTimes; i++)
			m_logFile << "\t";
		
		m_logFile << msg.c_str();

		if (m_newLineFlag)
			m_logFile << std::endl;
		
		m_logFile << std::flush;
		
		m_isNewLine = m_newLineFlag; // Note come we on new line or not
		m_newLineFlag = false;
	}
#endif
}

Logger& Logger::getLogger()
{
	return m_logger;
}

Logger::~Logger()
{
	if (m_logFile)
	{
		m_logFile << "\n------\n";
		m_logFile << "======= End of LOG file ========\n";
		m_logFile.close();
	}
}
