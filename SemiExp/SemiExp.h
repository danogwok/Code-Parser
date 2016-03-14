#ifndef SEMIEXPRESSION_H
#define SEMIEXPRESSION_H
///////////////////////////////////////////////////////////////////////
// SemiExpression.h - collect tokens for analysis                    //
// ver 3.3                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Code Parser, Project 2                               //
// Author:      Ronak Bhuptani, SUID#429019830, Syracuse             //
//              University, rmbhupta@syr.edu                         //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public SemiExp class that collects and makes
* available sequences of tokens.  SemiExp uses the services of a Toker
* class to acquire tokens.  Each call to SemiExp::get() returns a
* sequence of tokens that ends in {.  This will be extended to use the

* full set of terminators: {, }, ;, and '\n' if the line begins with #.
* Build Process:
* --------------
* Required Files:
*   SemiExpression.h, SemiExpression.cpp, Tokenizer.h, Tokenizer.cpp
*
* Build Command: devenv CodeParser.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 3.3 : 08 Frb 2016
* - Implemented all the functions of ITokCollection
* - Handled (#,newline), (:,public-pivate-protected) , (for(;;)) exceptions
* - Handled termination of { , } , ;
* - Handled comment as termination token
* ver 3.2 : 02 Feb 2016
* - declared SemiExp copy constructor and assignment operator = delete
* - added default argument for Toker pointer to nullptr so SemiExp
*   can be used like a container of tokens.
* - if pToker is nullptr then get() will throw logic_error exception
* ver 3.1 : 30 Jan 2016
* - changed namespace to Scanner
* - fixed bug in termination due to continually trying to read
*   past end of stream if last tokens didn't have a semiExp termination
*   character
* ver 3.0 : 29 Jan 2016
* - built in help session, Friday afternoon
*
* Planned Additions and Changes:
* ------------------------------
* - add public :, protected :, private : as terminators
* - move creation of tokenizer into semiExp constructor so
*   client doesn't have to write that code.
*/

#include <vector>
#include <map>
#include "itokcollection.h"
#include "../Tokenizer/Tokenizer.h"

namespace Scanner
{
	using Token = std::string;

	class SemiExp : public ITokCollection
	{
	public:
		//detailed comments in .cpp file
		SemiExp(Toker* pToker = nullptr);
		SemiExp(const SemiExp&) = delete;
		SemiExp& operator=(const SemiExp&) = delete;
		bool get(bool clear = true);
		Token& operator[](size_t n);
		size_t length();
		void printSemiExp(bool showNewLines = false);
		std::string show(bool showNewLines = false);
		size_t find(const Token& tok);
		void push_back(const std::string& tok);
		bool merge(const Token& firstTok, const Token& secondTok);
		bool remove(const Token& tok);
		bool remove(size_t i);
		void toLower();
		void trimFront();
		void clear();
		void setDefaultFlags();
		bool printAllExpression(Toker* toker,std::string input,std::string getComments = "false", bool storeText = false);
		bool exception2(Token currToken); // : after public,private,protected
		bool exception3(Token currToken,int &count); // for ( ; ; ) 
		bool isComment(Token currToken);
	private:
		std::vector<Token> _tokens;
		Toker* _pToker;
		static std::string inputDir;
		static std::string outputDir;
		static std::map<Token, int>  _terminationtokens;
		static Token _firstToken;
		static Token _prevToken;
		static bool _startFlag;
		static bool _forFlag;
		static bool _hashFlag;
	};
}
#endif
