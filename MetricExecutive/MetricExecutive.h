#ifndef METRICAEXECUTIVE_H
#define METRICAEXECUTIVE_H
/////////////////////////////////////////////////////////////////////
//  MetricExecutive.h - analyze all files in package               //
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
- MetricAnalyzer.h,MetricAnalyzer.cpp,MetricExecutive.h,MetricExecutive.cpp
- FileMgr.h, FileMgr.cpp, Utilities.h ,Utilities.cpp
Build commands (either one)
- devenv CodeParser.sln /rebuild debug

Maintenance History:
====================

ver 1.0 : 7 Mar 16
- first release
- added function to get functions with max lines and complexity in a package
- added function to analyze whole package
- added unction to show faulty functions (lines > 50 || cc > 10)

*/
#include "MetricAnalyzer.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <iomanip>

class MetricExecutive {

public:
	MetricExecutive() { //constructor
		totalFComplexity = 0;
		totalFLines = 0;
		totalLines = 0;
		CPL = 0; 
		maxFLines = 0;
		maxFComplexity = 0;
	}
	~MetricExecutive() {} //destructor
	// functions : detail explanation in implemtation file
	size_t getTotalFComplexity();
	size_t getTotalFLines();
	size_t getMaxFComplexity();
	size_t getMaxFLines();
	size_t getTotalLines();
	double getCPL();
	void addAnalyzer(std::string fileSpec, MetricAnalyzer *analyzer);
	void analyzeCode();
	void packageSummary();
	void showfaultyFunctions();
	void Exceute(std::string dir,std::vector<std::string> pattern,std::string stopFlag);
	std::vector<std::string> getFiles(std::string, std::vector<std::string>);
	void showReq();
private:
	//private data members
	std::map<std::string, MetricAnalyzer*> analyzers;
	static size_t totalFComplexity;
	static size_t totalFLines;
	static size_t totalLines;
	double CPL; //complexity per line
	static size_t maxFLines;
	static size_t maxFComplexity;
	std::set<std::string> faultyFunctions;
};

#endif