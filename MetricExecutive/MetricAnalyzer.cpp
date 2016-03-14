/////////////////////////////////////////////////////////////////////
//  MetricAnalyzer.cpp - analyze all files in package              //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Code Parser, Project 2                          //
//  Author:		   Ronak Bhuptani, SUID#429019830, Syracuse        //
//                 University, rmbhupta@syr.edu                    //
/////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_DEPRECATE
#define NO_WARN_MBCS_MFC_DEPRECATION
#include "MetricAnalyzer.h"
#include "..\\Utilities\Utilities.h"
#include "..\\Parser\Parser.h"
#include "..\\Parser\AST.h"
#include "..\\Parser\ConfigureParser.h"
#include "..\\Parser\ActionsAndRules.h"
#include "..\\FileMgr\FileMgr.h"
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>


using SH = Utilities::StringHelper;
//initialize static objects
std::string MetricAnalyzer::outputDir = "./Outputs/";

//this function returns total function comlexity
size_t MetricAnalyzer::getTotalFComplexity()
{
	return totalFComplexity;
}
//this function returns total function lines
size_t MetricAnalyzer::getTotalFLines()
{
	return totalFLines;
}
//this function returns total lines
size_t MetricAnalyzer::getTotalLines()
{
	return totalLines;
}
//this function sets repository object and updates metrics
void MetricAnalyzer::setRepository(Repository *Repos)
{
	_Repos = Repos;
	if (_Repos != nullptr) {
		allFunctions = _Repos->getAST()->getAllFunctions();
		calcTotalFlines_Complexity();
		calcTotalLines();
		updateRoot();
	}
}

// this function calculate total lines, function lines and complexity
void MetricAnalyzer::calcTotalFlines_Complexity()
{
	totalFLines = 0;
	for (size_t i = 0; i < allFunctions.size(); i++) {
		size_t lc = allFunctions[i].getLineCount();
		totalFLines += lc;
		if (lc > maxFLines)
			maxFLines = lc;
		size_t cc = allFunctions[i].getCC();
		totalFComplexity += cc;
		if (cc > maxFComplexity)
			maxFComplexity = cc;
		if (lc > 50 || cc > 10)
			faultyFunctions.insert(std::make_tuple(cc, lc, allFunctions[i].getName()));
	}
		
}
// this function calculates total lines of module
void MetricAnalyzer::calcTotalLines()
{
	totalLines = _Repos->lineCount();
}
// this functions displays function summary of the current module
void MetricAnalyzer::functionSummary()
{
	const char separator = ' ';
	const int nameWidth = 25;
	const int numWidth = 10;
	std::vector<ASTNode> functions = allFunctions;
	SH::title("Functions Summary");
	std::cout << std::endl;
	std::cout << std::left << std::setw(49) << std::setfill('*') <<""<< std::endl;
	std::cout <<"* "	<< std::left << std::setw(numWidth) << std::setfill(separator) << "Cyclo"
				<< std::left << std::setw(numWidth) << std::setfill(separator) << "Lines"
				<< std::left << std::setw(nameWidth) << std::setfill(separator) << "Function Name"
				<< " *" << std::endl;
	for (size_t i = 0; i < functions.size(); i++) {
		std::cout << "* " << std::left << std::setw(numWidth) << std::setfill(separator) << functions[i].getCC()
			<< std::left << std::setw(numWidth) << std::setfill(separator) << functions[i].getLineCount() 
			<< std::left << std::setw(nameWidth) << std::setfill(separator) << functions[i].getName()
			<< " *" << std::endl;
	}
	std::cout << std::left << std::setw(49) << std::setfill('*') << "" << std::endl;
}
// this functions displays module summary of the current module
void MetricAnalyzer::moduleSummary()
{ 
	SH::title("Module Summary");
	std::cout << std::endl << std::left << std::setw(38) << std::setfill('*') << "" ;
	std::cout << std::endl << "* " << std::left << std::setw(25) << std::setfill(' ') << " Lines of Function Code" << " : "
		<< std::left << std::setw(6) << std::setfill(' ') << getTotalFLines() << " *";
	std::cout << std::endl << "* " << std::left << std::setw(25) << std::setfill(' ') <<" Lines of Code Text" << " : "
		<< std::left << std::setw(6) << std::setfill(' ') << getTotalLines() << " *";
	std::cout << std::endl << "* " << std::left << std::setw(25) << std::setfill(' ') <<" Cyclometic Complexity" <<" : " 
		<< std::left << std::setw(6) << std::setfill(' ') << getTotalFComplexity() << " *";
	std::cout << std::endl << std::left << std::setw(38) << std::setfill('*') << "" << std::endl;
	std::cout << std::endl;
}
//this function shows Abstract Syntax Tree of current module
void MetricAnalyzer::showAST()
{
	SH::title("Abstract Syntax Tree");
	std::cout << std::endl;
	_Repos->getAST()->showAST();
	std::cout << std::endl;
}
// this function returns max function complexity of a module
size_t MetricAnalyzer::getMaxFComplexity()
{
	return maxFComplexity;
}
// this function returns max function lines of a module
size_t MetricAnalyzer::getMaxFLines()
{
	return maxFLines;
}
// this function returns faulty functions of a module
std::set<std::tuple<size_t, size_t, std::string>> MetricAnalyzer::getFaultyFunction()
{
	return faultyFunctions;
}
// this function updates root of AST
void MetricAnalyzer::updateRoot()
{
	ASTNode *root = _Repos->getAST()->getRoot();
	root->setLineEnd(totalLines);
	root->setLineCount(root->getLineEnd() - root->getLineStart() + 1);
	size_t cc = 1;
	for (size_t i = 0; i < root->getChildren()->size(); i++)
		cc += root->getChildren()->at(i)->getCC();
	root->setCC(cc);
}
// this function analyze file specified in parameter
bool MetricAnalyzer::Analyze(std::string filePath)
{
	std::string fileSpec = FileSystem::Path::getFullFileSpec(filePath);
	std::string name = FileSystem::Path::getName(fileSpec);
	std::string msg = "Processing file " + fileSpec;
	SH::title(msg);
	std::cout << std::endl;
	std::string output = outputDir  + SH::changeExtenssion(name,".txt");
	FILE *pfile = nullptr;
	pfile = freopen(output.c_str(), "w", stdout);
	ConfigParseToConsole configure;
	Parser* pParser = configure.Build();
	try
	{
		if (pParser)
		{
			if (!configure.Attach(filePath))
			{
				pfile = freopen("CON", "w", stdout);
				std::cout << "\n  could not open file " << fileSpec << std::endl;
				return false;
			}
		}
		else
		{
			pfile = freopen("CON", "w", stdout);
			std::cout << "\n\n  Parser not built\n\n";
			return false;
		}
		while (pParser->next())
			pParser->parse();	
			
		freopen("CON", "w", stdout);
		std::cout <<"Intermediate Output is stored at  " << FileSystem::Path::getFullFileSpec(output) <<"\n";
		setRepository(Repository::getInstance());
		showAST();
		functionSummary();
		moduleSummary();
		return true;
	}
	catch (std::exception& ex)
	{
		freopen("CON", "w", stdout);
		std::cout << Repository::getInstance()->lineCount();
		std::cout << "\n\n    " << ex.what() << "\n\n";
	}
	return true;
}


#ifdef TEST_METRIC_ANALYZER

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}
	MetricAnalyzer MA;
	MA.Analyze(argv[1]);
	std::cout << "\n";
}

#endif