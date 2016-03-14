/////////////////////////////////////////////////////////////////////
//  MetricExecutive.cpp - analyze all files in package             //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Code Parser, Project 2                          //
//  Author:		   Ronak Bhuptani, SUID#429019830, Syracuse        //
//                 University, rmbhupta@syr.edu                    //
/////////////////////////////////////////////////////////////////////
#include "MetricExecutive.h"
#include <conio.h>
#include <string>
#include "..\\FileMgr\FileMgr.h"
#include "..\\FileMgr\FileSystem.h"
using SH = Utilities::StringHelper;
//initialize static objects
size_t MetricExecutive::totalFComplexity = 0;
size_t MetricExecutive::totalFLines = 0;
size_t MetricExecutive::totalLines =0;
size_t MetricExecutive::maxFLines =0;
size_t MetricExecutive::maxFComplexity =0;

//this function returns total function comlexity
size_t MetricExecutive::getTotalFComplexity()
{
	return totalFComplexity;
}
//this function returns total function lines
size_t MetricExecutive::getTotalFLines()
{
	return totalFLines;
}
//this function returns max function comlexity
size_t MetricExecutive::getMaxFComplexity()
{
	return maxFComplexity;
}
//this function returns max function lines
size_t MetricExecutive::getMaxFLines()
{
	return maxFLines;
}
//this function returns total lines
size_t MetricExecutive::getTotalLines()
{
	return totalLines;
}
//this function returns complexity per line
double MetricExecutive::getCPL()
{
	return CPL;
}
//this function adds analyzer to map
void MetricExecutive::addAnalyzer(std::string fileSpec, MetricAnalyzer * analyzer)
{
	analyzers.insert(std::make_pair(fileSpec, analyzer));
}
//this function analyze code
void MetricExecutive::analyzeCode()
{
	std::map<std::string, MetricAnalyzer*>::iterator it;
	for (it = analyzers.begin(); it != analyzers.end(); it++) {
		totalFComplexity += it->second->getTotalFComplexity();
		totalFLines += it->second->getTotalFLines();
		totalLines += it->second->getTotalLines();
		if (maxFLines < it->second->getMaxFLines())
			maxFLines = it->second->getMaxFLines();
		if (maxFComplexity < it->second->getMaxFComplexity())
			maxFComplexity = it->second->getMaxFComplexity();
		std::set<std::tuple<size_t, size_t, std::string>> ff = it->second->getFaultyFunction();
		std::set<std::tuple<size_t, size_t, std::string>>::iterator sit;
		for (sit = ff.begin(); sit != ff.end(); sit++) {
			std::stringstream ss;
			int t = static_cast<int>(std::get<0>(*sit));
			ss << std::left << std::setw(5) << std::setfill(' ') << t;
			t = static_cast<int>(std::get<1>(*sit));
			ss << std::left << std::setw(5) << std::setfill(' ') << t;
			ss << std::right << std::setw(20) << std::setfill(' ') << std::get<2>(*sit);
			ss << " : " << std::left << std::setw(26) << std::setfill(' ') << it->first;
			faultyFunctions.insert(ss.str());
		}
	}	
	CPL = static_cast<double>(totalFComplexity) / static_cast<double>(totalFLines);
}
//this functions displays paclage summary
void MetricExecutive::packageSummary()
{
	SH::title("Package Summary");
	std::cout << std::endl;
	std::cout << std::endl << std::left << std::setw(55) << std::setfill('*') << "";
	std::cout << std::endl << "* " << std::left << std::setw(37) << std::setfill(' ') << " Total Cyclometic Complexity" << " : "
		<< std::left << std::setw(11) << std::setfill(' ') << getTotalFComplexity() << " *";
	std::cout << std::endl << "* " << std::left << std::setw(37) << std::setfill(' ') << " Total Function Lines" << " : "
		<< std::left << std::setw(11) << std::setfill(' ') << getTotalFLines() << " *";
	std::cout << std::endl << "* " << std::left << std::setw(37) << std::setfill(' ') << " Max Cyclometic Complexity" << " : "
		<< std::left << std::setw(11) << std::setfill(' ') << getMaxFComplexity() << " *";
	std::cout << std::endl << "* " << std::left << std::setw(37) << std::setfill(' ') << " Max Function Lines" << " : "
		<< std::left << std::setw(11) << std::setfill(' ') << getMaxFLines() << " *";
	std::cout << std::endl << "* " << std::left << std::setw(37) << std::setfill(' ') << " Total Lines" << " : "
		<< std::left << std::setw(11) << std::setfill(' ') << getTotalLines() << " *";
	std::cout << std::endl << "* " << std::left << std::setw(37) << std::setfill(' ') << " Complexity Per Line" << " : "
		<< std::left << std::setw(11) << std::setfill(' ') << getCPL() << " *";
	std::cout << std::endl << std::left << std::setw(55) << std::setfill('*') << "" << std::endl;
	std::cout << std::endl;
}
//this functions shows faulty functions
void MetricExecutive::showfaultyFunctions()
{
	SH::title("Functions excedding CC or lines limits : ");
	std::cout << std::endl;
	if (faultyFunctions.size() <= 0)
		std::cout << std::endl << " This Package doesn't have any functions exceeding lines or complexity. \n";
	else {
		
		std::cout << std::endl << std::left << std::setw(65) << std::setfill('*') << "" << std::endl;
		std::set<std::string>::iterator it;
		for (it = faultyFunctions.begin(); it != faultyFunctions.end(); it++)
			std::cout << "* " << *it << " *" << std::endl;
		std::cout << std::left << std::setw(65) << std::setfill('*') << "" << std::endl;
	}
}
//this functions execute analyzer on all files which matches patters found in package 
void MetricExecutive::Exceute(std::string dir, std::vector<std::string> patterns, std::string stopFlag)
{
	if (_access(dir.c_str(), 0) != 0)
		dir = "." + dir;
	std::vector<std::string> files = getFiles(dir, patterns);
	std::cout << files.size();
	if (files.size() > 0) {
		for (size_t i = 0; i < files.size(); i++) {
			MetricAnalyzer * analyzer = new MetricAnalyzer();
			addAnalyzer(files[i], analyzer);
			analyzer->Analyze(files[i]);
			if (stopFlag == "true" &&  i < files.size() -1) {
				std::cout << "Press any key to process next file... (This feature is added to view all output lines in case output is very long.)" << std::endl;
				_getch();
			}
		}
		analyzeCode();
		packageSummary();
		showfaultyFunctions();
	}
}

//this function getFiles from directory
std::vector<std::string> MetricExecutive::getFiles(std::string dir, std::vector<std::string> patterns)
{
	DataStore ds;
	std::vector<std::string> files;
	FileMgr fm(dir, ds);
	for (std::string s : patterns)
		fm.addPattern(s);
	fm.search();
	for (auto fs : ds)
		files.push_back(fs);
	return files;
}
//this function shows requirements
void MetricExecutive::showReq()
{
	SH::title("Requirement 3");
	std::cout << std::endl;
	std::cout << std::endl << " It provides C++ packages for analyzing function size and complexity metrics for a set of specified packages. Please check MetricExecutive, MetricAnalyzer,"
		<< "Parser,SemiExp, and Tokenizer packages." << std::endl;
	SH::title("Requirement 4");
	std::cout << std::endl;
	std::cout << std::endl << " It provide a Parser package with a Parser class that is a container for Rules and that provides the interfaces IRule and IAction for rules contained in the Parser and actions contained in each rule. Please check Parser.h file." << std::endl;
	SH::title("Requirement 5");
	std::cout << std::endl;
	std::cout << std::endl << " It provides rules in ActionAndRules.h file to detect functions, types, control statements, anonymous scopes and lambdas. It also provides respective actions that supports "
		<< "building Abstract Syntax Tree which identifies type, lines and complexity." << std::endl;
	SH::title("Requirement 6");
	std::cout << std::endl;
	std::cout << std::endl << " It provides Abstract Syntax Tree that provides an interface for adding scope nodes to the tree and an methods to analyze the contents of the tree. Please check AST.h and AST.cpp" << std::endl;
	SH::title("Requirement 7");
	std::cout << std::endl;
	std::cout << std::endl << " It provides provide a FileMgr package that supports finding files and/or directories in a directory tree rooted at a specified path. Please check FileMgr package." << std::endl;
	SH::title("Requirement 8");
	std::cout << std::endl;
	std::cout << std::endl << " It provides MetricAnalyzer which runs parser on single .cpp or .h file and construct AST with function summary, and module summary. Please check MetricAnalyzer.h and MetricAnalyzer.cpp in MetricExecutive package." << std::endl;
	SH::title("Requirement 9");
	std::cout << std::endl;
	std::cout << std::endl << " It provides MetricExecutive.h and MetricExecutive.cpp files in MetricExecutive package that enables collecting metrics for all the packages with names that match a specified pattern in a directory tree rooted at a specified path."
			<< " And it also provides package summary." << std::endl;
	SH::title("Requirement 10");
	std::cout << std::endl;
	std::cout << std::endl << " It provides automated unit test suite that exercises all of the packages provided in this project and demonstrates that it meets all requirement."<<std::endl<<std::endl;
	std::cout << std::endl << "Press any key to start the automated unit test suite..." << std::endl;
	_getch();
}

#ifdef TEST_METRIC_EXECUTIVE

int main(int argc, char* argv[]) {
	std::string dir, pattern,stopFlag;
	SH::getDirPattern(argc, argv, dir, pattern, stopFlag);
	std::string dirSpec = FileSystem::Path::getFullFileSpec(dir);
	MetricExecutive ME;
	ME.showReq();
	std::string msg = "Processing " + pattern + " files on Directory : " + dirSpec;
	std::cout << std::endl;
	SH::title(msg);
	std::cout << std::endl;
	std::vector<std::string> patterns = SH::split(pattern);
	ME.Exceute(dir, patterns, stopFlag);
	std::cout << "\n";
}

#endif
