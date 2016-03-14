/////////////////////////////////////////////////////////////////////
//  ConfigureParser.cpp - builds and configures parsers            //
//  ver 2.1                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2008                                 //
//  Platform:      Dell Dimension 9150, Windows XP SP2             //
//  Application:   Code Parser, Project 2                          //
//  Author:		   Ronak Bhuptani, SUID#429019830, Syracuse        //
//                 University, rmbhupta@syr.edu                    //
/////////////////////////////////////////////////////////////////////

#include <fstream>
#include "Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

using namespace Scanner;

//----< destructor releases all parts >------------------------------

ConfigParseToConsole::~ConfigParseToConsole()
{
  // when Builder goes out of scope, everything must be deallocated

  delete pHandlePush;
  delete pBeginningOfScope;
  delete pHandlePop;
  delete pEndOfScope;
  delete pPrintFunction;
  delete pPushFunction;
  delete pFunctionDefinition;
  delete pDeclaration;
  delete pShowDeclaration;
  delete pExecutable;
  delete pShowExecutable;
  delete pRepo;
  delete pParser;
  delete pSemi;
  delete pToker;
  pIn->close();
  delete pIn;
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseToConsole::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  pIn = new std::ifstream(name);
  if (!pIn->good())
    return false;
  return pToker->attach(pIn);
}
//----< Here's where alll the parts get assembled >----------------

//initialize objects
void ConfigParseToConsole::intializeObjects() {
	pToker = new Toker;
	pToker->setComments("false");
	pSemi = new SemiExp(pToker);
	pParser = new Parser(pSemi);
	pRepo = new Repository(pToker);
}

//initalize scope rules and its actions
void ConfigParseToConsole::intializeScopeRules()
{
	pBeginningOfScope = new BeginningOfScope();
	pHandlePush = new HandlePush(pRepo);
	pBeginningOfScope->addAction(pHandlePush);
	pParser->addRule(pBeginningOfScope);

	pEndOfScope = new EndOfScope();
	pHandlePop = new HandlePop(pRepo);
	pEndOfScope->addAction(pHandlePop);
	pParser->addRule(pEndOfScope);

	pTypeDefinition = new TypeDefinition();
	pPushTypes = new PushTypes(pRepo);
	pTypeDefinition->addAction(pPushTypes);
	pParser->addRule(pTypeDefinition);

	pFunctionDefinition = new FunctionDefinition;
	pPushFunction = new PushFunction(pRepo); 
	pPrintFunction = new PrintFunction(pRepo);
	pFunctionDefinition->addAction(pPushFunction);
	pFunctionDefinition->addAction(pPrintFunction);
	pParser->addRule(pFunctionDefinition);

	pControlDefinction = new ControlDefinition();
	pPushControls = new PushControls(pRepo);
	pControlDefinction->addAction(pPushControls);
	pParser->addRule(pControlDefinction);

	
}
//initialize other scope rules and its actions
void ConfigParseToConsole::intializeOtherRules() {
	pDeclaration = new Declaration;
	pShowDeclaration = new ShowDeclaration(pRepo);
	pDeclaration->addAction(pShowDeclaration);
	pParser->addRule(pDeclaration);

	pExecutable = new Executable;
	pShowExecutable = new ShowExecutable;
	pExecutable->addAction(pShowExecutable);
	pParser->addRule(pExecutable);

	pPreproc = new PreprocStatement;
	pPrintPreproc = new	PrintPreproc;
	pPreproc->addAction(pPrintFunction);
	pParser->addRule(pPreproc);
}

//build the parser object
Parser* ConfigParseToConsole::Build()
{
  try
  {
	intializeObjects();
	intializeScopeRules();
	intializeOtherRules();
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}

#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ConfigureParser module\n "
            << std::string(32,'=') << std::endl;

  // collecting tokens from files, named on the command line

  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }

  for(int i=1; i<argc; ++i)
  {
    std::cout << "\n  Processing file " << argv[i];
    std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');

    ConfigParseToConsole configure;
    Parser* pParser = configure.Build();
    try
    {
      if(pParser)
      {
        if(!configure.Attach(argv[i]))
        {
          std::cout << "\n  could not open file " << argv[i] << std::endl;
          continue;
        }
      }
      else
      {
        std::cout << "\n\n  Parser not built\n\n";
        return 1;
      }
      // now that parser is built, use it

      while(pParser->next())
        pParser->parse();
      std::cout << "\n\n";
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
    std::cout << "\n\n";
  }
}

#endif
