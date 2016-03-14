#ifndef TOKENIZER_H
#define TOKENIZER_H
///////////////////////////////////////////////////////////////////////
// Tokenizer.h - read words from a std::stream                       //
// ver 3.5                                                           //
// Language:  C++, Visual Studio 2015                                //
// Application: Code Parser, Project 2                               //
// Author:		Ronak Bhuptani, SUID#429019830, Syracuse             //
//              University, rmbhupta@syr.edu                         //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a public Toker class and private ConsumeState class.
 * Toker reads words from a std::stream, throws away whitespace and comments
 * and returns words from the stream in the order encountered.  Quoted
 * strings and certain punctuators and newlines are returned as single tokens.
 *
 * This is a new version, based on the State Design Pattern.  Older versions
 * exist, based on an informal state machine design.
 *
 * Build Process:
 * --------------
 * Required Files: Tokenizer.h, Tokenizer.cpp
 * Build Command: devenv CodeParser.sln /rebuild debug
 *
 * Maintenance History:
 * --------------------
 * ver 3.5 : 08 Feb 2016
 * - added functionality to add ned or reset special character list
 * - added  functinality to set Comments flag
 * - added new states in Consumestate class for handling quotes, special chars
 * - added functionality to print all tokens by specifying fileName in input folder
 * ver 3.4 : 03 Feb 2016
 * - fixed bug that prevented reading of last character in source by
 *   clearing stream errors at beginning of ConsumeState::nextState()
 * ver 3.3 : 02 Feb 2016
 * - declared ConsumeState copy constructor and assignment operator = delete
 * ver 3.2 : 28 Jan 2016
 * - added feature to take comments as a token if "true" is specified as first argument.
 * ver 3.1 : 27 Jan 2016
 * - fixed bug in EatCComment::eatChars()
 * - removed redundant statements assigning _pState in derived eatChars() 
 *   functions
 * - removed calls to nextState() in each derived eatChars() and fixed
 *   call to nextState() in ConsumeState::consumeChars()
 * ver 3.0 : 11 Jun 2014
 * - first release of new design
 *
 * Planned Additions and Changes:
 * ------------------------------
 * - Return quoted strings as single token.  This must handle \" and \'
 *   correctly.
 * - Consider converting eatNewLine() to eatSpecialTokens() - see below
 * - Return [, ], {, }, (, ), <, >, :, ; as single character tokens
 * - Return <<, >>, +=, -=, *=, /=, :: as two character tokens
 */
#include <iosfwd>
#include <string>
namespace Scanner {

	
	class ConsumeState;

	class Toker
	{
	public:
		//detailed comments in .cpp file
		Toker();
		Toker(const Toker&) = delete;
		~Toker();
		Toker& operator=(const Toker&) = delete;
		bool attach(std::istream* pIn);
		std::string getTok();
		bool canRead();
		void setComments(const std::string value);
		bool printAllTokens(std::string input, std::string getComments = "false", bool storeText = false);
		void setSpecialSingleChars(std::string ssc, bool appendFlag = true);
		void setSpecialCharPairs(std::string scp, bool appendFlag = true);
		size_t currentLineCount();
		void reset();
	private:
		ConsumeState* pConsumer;
		static std::string inputDir;
		static std::string outputDir;
	};
}
#endif