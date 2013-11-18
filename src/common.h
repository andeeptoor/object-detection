/*
 * common.h
 *
 *  Created on: Oct 25, 2013
 *      Author: atoor
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <vector>
#include <map>
#include "utils.h"
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

struct TimeSeries {
	vector<double> values;
	double mean;
	double standardDeviation;
	int classification;
};

struct Config {
	string fileExtension;
	string imageDirectory;
	string imageAnnotationsDirectory;
	string detectionLatentSVMModel;
};

Config readConfigFile(char* configFile);
void showProgress(int index);
void endProgress();
void printConfusionMatrix(const vector<vector<int> >& confusionMatrix, map<int, string>& classLabels, const int totalCount) ;

#endif /* COMMON_H_ */
