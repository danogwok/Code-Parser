///////////////////////////////////////////////////////////////////////
// Tokenizer.cpp - read words from a std::stream                     //
// ver 3.5                                                           //
// Language:  C++, Visual Studio 2015                                //
// Application: Code Parser, Project 2                               //
// Author:		Ronak Bhuptani, SUID#429019830, Syracuse             //
//              University, rmbhupta@syr.edu                         //
///////////////////////////////////////////////////////////////////////
/*

* Maintenance History:
* --------------------
* ver 3.5 : 08 Feb 2016
* - added _takeComments variable and get set methods of the same.
* - all additions are mentioned in Tokenizer.h
*
*/
#define _CRT_SECURE_NO_DEPRECATE
#define NO_WARN_MBCS_MFC_DEPRECATION
#include "Tokenizer.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include<sstream>
#include <iostream>
#include <cctype>
#include <string>
#include <map>
namespace Scanner {
	class ConsumeState
	{
	public:
		ConsumeState();
		virtual ~ConsumeState();
		ConsumeState(const ConsumeState&) = delete;
		ConsumeState& operator=(const ConsumeState&) = delete;
		void attach(std::istream* pIn) { _pIn = pIn; }
		virtual void eatChars() = 0;
		void consumeChars() {
			_pState->eatChars();
			_pState = nextState();
		}
		void reset() { firstC = true; };				//resets firstC flag
		bool canRead() { return _pIn->good(); }
		std::string getTok() { return token; }
		bool hasTok() { return token.size() > 0; }
		ConsumeState* nextState();
		bool getComments() { return _takeComments; }
		void setComments(bool value) { _takeComments = value; }
		size_t currentLineCount() { return _lineCount; }
		int isSpecialChar();
		void setSpecialSingleChars(std::string ssc, bool appendFlag = true);
		void setSpecialCharPairs(std::string scp, bool appendFlag = true);
	protected:
		static std::string token;						//store the current token
		static std::istream* _pIn;						//file handler
		static int prevChar;							//store the previous character
		static int currChar;							//store the current character
		static ConsumeState* _pState;					//store the current state
		static ConsumeState* _pEatCppComment;			//CPP comment state
		static ConsumeState* _pEatCComment;				//C Comment state
		static ConsumeState* _pEatWhitespace;			//WhiteSpace state
		static ConsumeState* _pEatPunctuator;			//Puntuator state
		static ConsumeState* _pEatAlphanum;				//Alphanum state
		static ConsumeState* _pEatSpecialChar;			//Speciak char state
		static ConsumeState* _pEatQuotes;				//Quotes state
		static bool firstC;								//to handle constrctor chaining and running more than one files
		static size_t _lineCount;
		static bool _takeComments;
		static bool rEOF;								//checks if file reached EOF
		static std::map<std::string, int> specialChars; //store all the special characters
	};
}
using namespace Scanner;

std::string Toker::inputDir = "./Inputs/";				//specify the input folder where test files are stored
std::string Toker::outputDir = "./Outputs/";			//specify the output folder where test results are stored
std::string ConsumeState::token;
std::istream* ConsumeState::_pIn;
int ConsumeState::prevChar;
int ConsumeState::currChar;
bool ConsumeState::_takeComments = false;
bool ConsumeState::rEOF = false;
bool ConsumeState::firstC = true;
size_t ConsumeState::_lineCount = 1;
std::map < std::string, int> ConsumeState::specialChars = { { "[",1 } ,{ "]",1 },{ "{",1 },{ "}",1 },{ "(",1 },{ ")",1 },{ "<",1 },{ "+",1 },{ "=",1 },{ "-",1 },
{ ">",1 },{ ":",1 },{ ";",1 },{ "\n",1 },{ "<<",2 } ,{ ">>",2 } ,{ "+=",2 } ,{ "==",2 },{ "*",1 },
{ "-=",2 } ,{ "*=",2 } ,{ "/=",2 },{ "::",2 },{ "\\n",2 } ,{ "/",1 } ,{ "++",2 },{ "--",2 } };  
ConsumeState* ConsumeState::_pState = nullptr;
ConsumeState* ConsumeState::_pEatCppComment = nullptr;
ConsumeState* ConsumeState::_pEatCComment = nullptr;
ConsumeState* ConsumeState::_pEatWhitespace = nullptr;
ConsumeState* ConsumeState::_pEatPunctuator = nullptr;
ConsumeState* ConsumeState::_pEatAlphanum = nullptr;
ConsumeState* ConsumeState::_pEatQuotes = nullptr;
ConsumeState* ConsumeState::_pEatSpecialChar = nullptr;



//function to log messsage
void testLog(const std::string& msg);

//this funcion check if the currChar and nextChar is a Special char token
int ConsumeState::isSpecialChar() {
	std::string tkn = "";
	if (rEOF) {
		tkn += currChar;
		if (specialChars.count(tkn) > 0)  // 1 char special characters
			return 1;
		return 0;
	}
	tkn += currChar;
	tkn += _pIn->peek();
	if (specialChars.count(tkn) > 0)   // 2 char special characters
		return 2;
	tkn = "";
	tkn += currChar;
	if (specialChars.count(tkn) > 0)  // 1 char special characters
		return 1;
	return 0;
}

//this function will return the nextState
ConsumeState* ConsumeState::nextState()
{
	if (!(_pIn->good()))
		return nullptr;
	int chNext = _pIn->peek();
	if (chNext == EOF) {
		_pIn->clear();
		rEOF = true;
	}		
	if (std::isspace(currChar) && currChar != '\n')
	{
		testLog("state: eatWhitespace");
		return _pEatWhitespace;
	}
	if (currChar == '\"' || currChar == '\'')
	{
		testLog("state: EatQuotes");
		return _pEatQuotes;
	}
	if (isSpecialChar() != 0 && (!(currChar == '/' && chNext == '/')) && (!(currChar == '/' && chNext == '*')))
	{
		testLog("state: eatSpecialCharacters");
		return _pEatSpecialChar;
	}
	if (currChar == '/' && chNext == '/')
	{
		testLog("state: eatCppComment");
		return _pEatCppComment;
	}
	if (currChar == '/' && chNext == '*')
	{
		testLog("state: eatCComment");
		return _pEatCComment;
	}
	if (std::isalnum(currChar))
	{
		testLog("state: eatAlphanum");
		return _pEatAlphanum;
	}
	if (ispunct(currChar))
	{
		testLog("state: eatPunctuator");
		return _pEatPunctuator;
	}
	throw(std::logic_error("invalid type"));
}

//class to handle whitespaces
class EatWhitespace : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		do {
			if (!_pIn->good())  // end of stream
				return;
			currChar = _pIn->get();
		} while (std::isspace(currChar) && currChar != '\n'); //continue untill it is not whitespace or newline
	}
};

//class to handle Cpp Comment
class EatCppComment : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		do {
			token += currChar;
			if (!_pIn->good())  // end of stream
				return;
			currChar = _pIn->get();
		} while (currChar != '\n');  //stops when new line comes
		if (!getComments())
			token.clear();
	}
};

//class to handle C Comment
class EatCComment : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		do {
			if (currChar == '\n')
				_lineCount++;
			token += currChar;
			if (!_pIn->good())  // end of stream
				return;
			currChar = _pIn->get();
		} while (currChar != '*' || _pIn->peek() != '/'); //stops when */ is identified
		token += currChar;  //skip *
		token += _pIn->get(); // skip /
		if (!getComments())
			token.clear();
		currChar = _pIn->get();
	}
};

//class to handle Punctuators
class EatPunctuator : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		do {
			token += currChar;
			if (!_pIn->good())  // end of stream
				return;
			currChar = _pIn->get();
		} while (ispunct(currChar) && currChar != '\"' && currChar != '\'' && !isSpecialChar()); //check if it is puntuator and not a special char
	}
};

//class to handle SpecialChar
class EatSpecialChar : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		int token_size = isSpecialChar();
		if (currChar == '\n')
			_lineCount++;
		if (token_size == 1)         //special one char token identified
			token += currChar;
		else if (token_size == 2) {  //special two char token identified
			token += currChar;
			token += _pIn->get();
		}
		if (currChar != -1)
			currChar = _pIn->get();
	}
};

//class to handle Alphanum
class EatAlphanum : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		do {
			token += currChar;
			if (!_pIn->good())  // end of stream
				return;
			currChar = _pIn->get();
		} while (isalnum(currChar)); //check if it still alphanum
	}
};

//class to handle Quotes
class EatQuotes : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		int startChar;
		bool escapeChar = true;  //this flag determines number of escapChar '\' : true - even , false - odd
		startChar = currChar;
		do {
			escapeChar = true;
			if (currChar == '\\')
				eatescapeChars(escapeChar);  //eat all escapecharacters and update escapeChar flag
			token += currChar;
			if (!_pIn->good())
				return;
			currChar = _pIn->get();
		} while (!(currChar == startChar && escapeChar));
		token += currChar;
		currChar = _pIn->get();
	}
	void eatescapeChars(bool &escapeChar) {
		do {
			token += currChar;
			escapeChar = !escapeChar;
			currChar = _pIn->get();
		} while (_pIn->peek() == '\\');
		escapeChar = !escapeChar;
	}
};

//Toker Constructor
Toker::Toker() : pConsumer(new EatWhitespace()) {
	if (_access(inputDir.c_str(), 0) != 0)
		inputDir = "." + inputDir;
	if (_access(outputDir.c_str(), 0) != 0)
		outputDir = "." + outputDir;
}

//Toker Destructor
Toker::~Toker() { delete pConsumer; }
//Consumestate Constructor
ConsumeState::ConsumeState()
{
	if (firstC)
	{
		firstC = false;
		_pEatAlphanum = new EatAlphanum();
		_pEatCComment = new EatCComment();
		_pEatCppComment = new EatCppComment();
		_pEatPunctuator = new EatPunctuator();
		_pEatWhitespace = new EatWhitespace();
		_pEatQuotes = new EatQuotes();
		_pEatSpecialChar = new EatSpecialChar();
		_pState = _pEatWhitespace;
		_lineCount = 1;
	}
}

//ConsumeState Destructor
ConsumeState::~ConsumeState()
{
	static bool first = true;
	firstC = true;
	if (first)
	{
		first = false;
		delete _pEatAlphanum;
		delete _pEatCComment;
		delete _pEatCppComment;
		delete _pEatPunctuator;
		delete _pEatWhitespace;
		delete _pEatQuotes;
		delete _pEatSpecialChar;
	}
}

//function to attach file object to _pIn
bool Toker::attach(std::istream* pIn)
{
	if (pIn != nullptr && pIn->good())
	{
		pConsumer->attach(pIn);
		return true;
	}
	return false;
}

//function to get token of current state
std::string Toker::getTok()
{
	while (true)
	{
		if (!pConsumer->canRead())
			return "";
		pConsumer->consumeChars();
		if (pConsumer->hasTok())
			break;
	}
	return pConsumer->getTok();
}

//check  if file is not reached EOF
bool Toker::canRead() { return pConsumer->canRead(); }

//to set comment flag
void Toker::setComments(const std::string value) {
	if (value == "true")
		pConsumer->setComments(true);
	if (value == "false")
		pConsumer->setComments(false);
}

//This function can be used in two ways
//1. If append flag(optional) is true(by default it is true), ',' separated special characters will be appended to special character list
//2. If append flag(optional) is false(by default it is true), ',' separated special characters will be replaced in special character list 
void Scanner::ConsumeState::setSpecialSingleChars(std::string ssc, bool appendFlag)
{
	if (!appendFlag) {
		std::map<std::string, int>::iterator it;
		for (it = specialChars.begin(); it != specialChars.end(); it++)
			if (it->second == 1)
				specialChars.erase(it);
	}
	std::stringstream ss(ssc);
	std::string item;
	while (getline(ss, item, ','))
		specialChars[item] = 1;
}

//This function can be used in two ways
//1. If append flag(optional) is true(by default it is true), ',' separated special characters will be appended to special character list
//2. If append flag(optional) is false(by default it is true), ',' separated special characters will be replaced in special character list 
void Scanner::ConsumeState::setSpecialCharPairs(std::string scp, bool appendFlag)
{
	if (!appendFlag) {
		std::map<std::string, int>::iterator it;
		for (it = specialChars.begin(); it != specialChars.end(); it++)
			if (it->second == 2)
				specialChars.erase(it);
	}
	std::stringstream ss(scp);
	std::string item;
	while (getline(ss, item, ','))
		specialChars[item] = 2;
}

//This function prints all tokens of input file specified in parameters
//Optional paramete : getComments(by default false) , if true gets all comments as token
//Optional paramete : storeText(by default false) , if true store the output file in Outputs folder
bool Scanner::Toker::printAllTokens(std::string input, std::string getComments, bool storeText)
{
	reset();
	std::string output = outputDir + "Tokens_" + input;
	input = inputDir + input;
	FILE *pfile = nullptr;
	if (storeText)
		pfile = freopen(output.c_str(), "w", stdout);
	std::ifstream in(input);
	if (!in.good())
	{
		std::cout << "\n  can't open " << input << "\n\n";
		return 1;
	}
	attach(&in);
	setComments(getComments);
	std::string tok = "";
	do
	{
		tok = getTok();
		if (tok == "\n")
			tok = "newline";
		std::cout << "\n -- " << tok;
	} while (in.good());
	std::cout << "\n\n";
	if (storeText) {
		pfile = freopen("CON", "w", stdout);
		std::cout << "Read output file from : \"" << output << "\".\n";
	}
	reset();
	return true;
}

//to Set special characters using toker object
void Scanner::Toker::setSpecialSingleChars(std::string ssc, bool appendFlag)
{
	pConsumer->setSpecialSingleChars(ssc, appendFlag);
}

//to Set special characters using toker object
void Scanner::Toker::setSpecialCharPairs(std::string scp, bool appendFlag)
{
	pConsumer->setSpecialCharPairs(scp, appendFlag);
}

size_t Scanner::Toker::currentLineCount()
{
	return pConsumer->currentLineCount();
}

//reset Toker class
void Scanner::Toker::reset()
{
	pConsumer->reset();
	pConsumer = new EatWhitespace();  //to handle nullptr after executing first file
}

//test log function
void testLog(const std::string& msg)
{
#ifdef TEST_LOG
	std::cout << "\n  " << msg;
#endif
}

//----< test stub >--------------------------------------------------

#ifdef TEST_TOKENIZER
#include <fstream>

int main(int argc, char **argv)
{
	std::string fileSpec = "Req4.txt";
	Toker toker;
	toker.setSpecialSingleChars("&,%"); //setting & and % as an single char special char
	toker.setSpecialCharPairs("&&,%%"); //setting && and %% as an single char special char
	toker.printAllTokens(fileSpec, "true", false);
	return 0;
}
#endif