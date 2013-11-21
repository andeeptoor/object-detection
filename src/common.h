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
#include "ObjectDetector.h"

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
	string imageDirectoryFilterFile;
	string imageAnnotationsDirectory;
	vector<ObjectDetector *> objectDetectors;
};

XMLElement* getChild(const string& childName, XMLElement* parentElement) ;
string getChildText(const string& childName, XMLElement* parentElement) ;
Config readConfigFile(char* configFile);
void showProgress(int index);
void endProgress();
void printConfusionMatrix(const vector<vector<int> >& confusionMatrix, map<int, string>& classLabels, const int totalCount) ;

#endif /* COMMON_H_ */
