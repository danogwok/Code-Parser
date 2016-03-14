#ifndef METRICANALYZER_H
#define METRICANALYZER_H
/////////////////////////////////////////////////////////////////////
//  MetricAnalyzer.h - analyze all files in package                //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Code Parser, Project 2                          //
//  Author:		   Ronak Bhuptani, SUID#429019830, Syracuse        //
//                 University, rmbhupta@syr.edu                    //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module analyze all files which matches given pattern in package
and provides helper functions to analyze the code.

Build Process:
==============
Required files
- MetricAnalyzer.h,MetricAnalyzer.cpp,Parser.h,Parser.cpp
- ConfigureParser.h, ConfigureParser.cpp, AST.h, AST.cpp
- FileMgr.h, FileMgr.cpp, Utilities.h ,Utilities.cpp

Build commands (either one)
- devenv CodeParser.sln /rebuild debug

Maintenance History:
====================

ver 1.0 : 7 Mar 16
- first release
- added function to get functions with max lines and complexity in a file
- added function to provide module summary
- added function to provide function summary

*/
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <tuple>
#include <io.h>
#include "..\\Utilities\Utilities.h"
#include "..\\Parser\Parser.h"
#include "..\\Parser\AST.h"
#include "..\\Parser\ConfigureParser.h"
#include "..\\FileMgr\FileMgr.h"
class MetricAnalyzer {
public:
	MetricAnalyzer() {
		totalFComplexity = 0;
		totalFLines = 0;
		totalLines = 0;
		maxFLines = 0;
		maxFComplexity = 0;
		if (_access(outputDir.c_str(), 0) != 0)
			outputDir = "." + outputDir;
	}
	~MetricAnalyzer() {} // blank destructor
	// functions : detail explanation in implemtation file
	size_t getTotalFComplexity();
	size_t getTotalFLines();
	size_t getTotalLines();
	size_t getMaxFComplexity();
	size_t getMaxFLines();
	std::set<std::tuple<size_t, size_t, std::string>> getFaultyFunction();
	void setRepository(Repository *Repos);
	void calcTotalFlines_Complexity();
	void calcTotalLines();
	void functionSummary();
	void moduleSummary();
	void showAST();
	void updateRoot();
	bool Analyze(std::string filePath);
protected:
	//private data members
	Repository *_Repos;
	std::vector<ASTNode> allFunctions;
	size_t totalFComplexity;
	size_t totalFLines;
	size_t totalLines;
	size_t maxFLines;
	size_t maxFComplexity;
	std::set<std::tuple<size_t,size_t,std::string>> faultyFunctions;
	static std::string outputDir;
};
#endif 
