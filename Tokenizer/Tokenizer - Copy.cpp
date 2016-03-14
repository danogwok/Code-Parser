/////////////////////////////////////////////////////////////////////
// Tokenizer.cpp - read words from a std::stream                   //
// ver 3.2                                                           //
// Language:  C++, Visual Studio 2015                                //
// Application: Parser component, CSE687 - Object Oriented Design    //
// Source:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
// Author:		Ronak Bhuptani, SUID#429019830, Syracuse             //
//              University, rmbhupta@syr.edu                         //
///////////////////////////////////////////////////////////////////////
/*
  Helper code that does not attempt to handle all tokenizing
  special cases like escaped characters.

  * Maintenance History:
  * --------------------
  * ver 3.2 : 28 Jan 2016
  * - added _takeComments variable and get set methods of the same.
  *
*/
#define _CRT_SECURE_NO_DEPRECATE
#define NO_WARN_MBCS_MFC_DEPRECATION
#include "Tokenizer.h"
#include <fstream>
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
		bool canRead() { return _pIn->good(); }
		std::string getTok() { return token; }
		bool hasTok() { return token.size() > 0; }
		ConsumeState* nextState();
		bool getComments() { return _takeComments; }
		void setComments(bool value) { _takeComments = value; }
		int isSpecialChar();
		void setSpecialSingleChars(std::string ssc, bool appendFlag = true);
		void setSpecialCharPairs(std::string scp, bool appendFlag = true);
	protected:
		static std::string token;
		static std::istream* _pIn;
		static int prevChar;
		static int currChar;
		static ConsumeState* _pState;
		static ConsumeState* _pEatCppComment;
		static ConsumeState* _pEatCComment;
		static ConsumeState* _pEatWhitespace;
		static ConsumeState* _pEatPunctuator;
		static ConsumeState* _pEatAlphanum;
		static ConsumeState* _pEatSpecialChar;
		static ConsumeState* _pEatQuotes;
		static bool _takeComments;
		static std::map<std::string, int> specialChars;
	};
}
using namespace Scanner;

std::string Toker::inputDir = "../Inputs/";
std::string Toker::outputDir = "../Outputs/";
std::string ConsumeState::token;
std::istream* ConsumeState::_pIn;
int ConsumeState::prevChar;
int ConsumeState::currChar;
bool ConsumeState::_takeComments = false;

std::map < std::string, int> ConsumeState::specialChars = { {"[",1} ,{ "]",1 },{ "{",1 },{ "}",1 },{ "(",1 },{ ")",1 },{ "<",1 },{"+",1},{"=",1},{ "-",1 },
														  { ">",1 },{ ":",1 },{ ";",1 }, {"\n",1},{ "<<",2 } ,{ ">>",2 } ,{ "+=",2 } , {"==",2},{ "*",1 },
														  { "-=",2 } ,{ "*=",2 } ,{ "/=",2 },{ "::",2 },{ "\\n",2 } ,{ "/",1 } ,{ "++",2 },{ "--",2 } };  //
ConsumeState* ConsumeState::_pState = nullptr;
ConsumeState* ConsumeState::_pEatCppComment = nullptr;
ConsumeState* ConsumeState::_pEatCComment = nullptr;
ConsumeState* ConsumeState::_pEatWhitespace = nullptr;
ConsumeState* ConsumeState::_pEatPunctuator = nullptr;
ConsumeState* ConsumeState::_pEatAlphanum = nullptr;
ConsumeState* ConsumeState::_pEatQuotes = nullptr;
ConsumeState* ConsumeState::_pEatSpecialChar = nullptr;

void testLog(const std::string& msg);

int ConsumeState::isSpecialChar() {
	std::string tkn = "";
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

ConsumeState* ConsumeState::nextState()
{
	if (!(_pIn->good()))
		return nullptr;
	int chNext = _pIn->peek();
	if (chNext == EOF) 
		_pIn->clear();
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
    } while (std::isspace(currChar) && currChar != '\n');
  }
};

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
    } while (currChar != '\n');
	if (!getComments())
		token.clear();
  }
};

class EatCComment : public ConsumeState
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
    } while (currChar != '*' || _pIn->peek() != '/');
	token += currChar;
	token += _pIn->get();
	if (!getComments())
		token.clear();
    currChar = _pIn->get();
  }
};

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
	} while (ispunct(currChar) && currChar != '\"' && currChar != '\'' && !isSpecialChar() );
  }
};

class EatSpecialChar : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		int token_size = isSpecialChar();
		if (token_size == 1) 
			token += currChar;
		else if (token_size == 2) {
			token += currChar;
			token += _pIn->get();
		}
		if (currChar != -1)
			currChar = _pIn->get();
	}
};

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
    } while (isalnum(currChar));
  }
};

class EatNewline : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		//std::cout << "\n  eating alphanum";
		token += currChar;
		if (!_pIn->good())  // end of stream
			return;
		currChar = _pIn->get();
	}
};

ConsumeState::~ConsumeState()
{
  static bool first = true;
  if (first)
  {
    first = false;
    delete _pEatAlphanum;
    delete _pEatCComment;
    delete _pEatCppComment;
    delete _pEatPunctuator;
    delete _pEatWhitespace;
  }
}

Toker::Toker() : pConsumer(new EatWhitespace()) {}

Toker::~Toker() { delete pConsumer; }

bool Toker::attach(std::istream* pIn)
{
  if (pIn != nullptr && pIn->good())
  {
    pConsumer->attach(pIn);
    return true;
  }
  return false;
}

std::string Toker::getTok()
{
  while(true) 
  {
	if (!pConsumer->canRead())
	  return "";
    pConsumer->consumeChars();
    if (pConsumer->hasTok())
      break;
  }
  return pConsumer->getTok();
}

bool Toker::canRead() { return pConsumer->canRead(); }


void Toker::setComments(const std::string value) {
	if(value == "true")
		pConsumer->setComments(true);
	if (value == "false")
		pConsumer->setComments(false);
}
void Scanner::ConsumeState::setSpecialSingleChars(std::string ssc, bool appendFlag)
{
	if (!appendFlag) {
		std::map<std::string, int>::iterator it;
		for (it = specialChars.begin(); it != specialChars.end(); it++)
			if (it->second == 1)
				specialChars.erase(it);
	}
	std::string::size_type i = 0;
	std::string::size_type j = ssc.find(',');
	while (j != std::string::npos) {
		specialChars[ssc.substr(i, j - i)] = 1;
		i = ++j;
		j = ssc.find(',', j);
		if (j == std::string::npos)
			specialChars[ssc.substr(i, ssc.length())] = 1;
	}
}
void Scanner::ConsumeState::setSpecialCharPairs(std::string scp, bool appendFlag)
{
	if (!appendFlag) {
		std::map<std::string, int>::iterator it;
		for (it = specialChars.begin(); it != specialChars.end(); it++)
			if (it->second == 2)
				specialChars.erase(it);
	}
	std::string::size_type i = 0;
	std::string::size_type j = scp.find(',');
	while (j != std::string::npos) {
		specialChars[scp.substr(i, j - i)] = 2;
		i = ++j;
		j = scp.find(',', j);
		if (j == std::string::npos)
			specialChars[scp.substr(i, scp.length())] = 2;
	}
}

bool Scanner::Toker::printAllTokens(std::string input, std::string getComments, bool storeText)
{
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
	std::string tok="";
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
	return true;
}

void Scanner::Toker::setSpecialSingleChars(std::string ssc, bool appendFlag)
{
	pConsumer->setSpecialSingleChars(ssc, appendFlag);
}

void Scanner::Toker::setSpecialCharPairs(std::string scp, bool appendFlag)
{
	pConsumer->setSpecialCharPairs(scp, appendFlag);
}

void testLog(const std::string& msg)
{
#ifdef TEST_LOG
  std::cout << "\n  " << msg;
#endif
}

class EatQuotes : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		int startChar;
		bool escapeChar = true;
		startChar = currChar;
		do {
			escapeChar = true;
			if (currChar == '\\')
				eatescapeChars( escapeChar);
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

ConsumeState::ConsumeState()
{
	static bool first = true;
	if (first)
	{
		first = false;
		_pEatAlphanum = new EatAlphanum();
		_pEatCComment = new EatCComment();
		_pEatCppComment = new EatCppComment();
		_pEatPunctuator = new EatPunctuator();
		_pEatWhitespace = new EatWhitespace();
		_pEatQuotes = new EatQuotes();
		_pEatSpecialChar = new EatSpecialChar();
		_pState = _pEatWhitespace;
	}
}

//----< test stub >--------------------------------------------------

#ifdef TEST_TOKENIZER
#include <fstream>

int main(int argc,char **argv)
{
  freopen("tokenizer_out.txt", "w", stdout);
  //std::string fileSpec = "../Tokenizer/Tokenizer.cpp";
  std::string fileSpec = "../Tokenizer/test.txt";
  std::ifstream in(fileSpec);
  if (!in.good())
  {
    std::cout << "\n  can't open " << fileSpec << "\n\n";
    return 1;
  }
  Toker toker;
  toker.attach(&in);
  
  if(argc>1)
	toker.setComments(argv[1]);
  while (in.good())
  {
    std::string tok = toker.getTok();
    if (tok == "\n")
      tok = "newline";
	std::cout << "\n -- " << tok;
  }
  std::cout << "\n\n";
  return 0;
  for (int i = 0; i < 10; i++);
}
#endif