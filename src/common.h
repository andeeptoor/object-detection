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

struct AnnotatedObject {
	string label;
	Point centerPoint;
	Rect boundingBox;
};

struct AnnotatedImage {
	int imageWidth, imageHeight, numberOfColors;
	vector<AnnotatedObject> objects;
};

struct EvaluationImage {
	int truePositive;
	int falsePositive;
	int numberOfPositives;
	double recall;
	double precision;
};

struct Evaluation {
	vector<EvaluationImage> evaluations;
	EvaluationImage total;
	double averagePrecision;
	double totalDetectionTime;
};

struct Config {
	string imageFileExtension;
	string imageDirectory;
	string imageDirectoryFilterFile;
	string annotationsDirectory;
	string annotationsFileExtension;
	string annotationsFileFormat;
	vector<ObjectDetector *> objectDetectors;
	bool detectionOutputImages;
	string detectionOutputImagesDirectory;
};

Rect unionOf(Rect r1, Rect r2);
XMLElement* getChild(const string& childName, XMLElement* parentElement);
string getChildText(const string& childName, XMLElement* parentElement);
Config readConfigFile(char* configFile);
void showProgress(int index);
void endProgress();
void printConfusionMatrix(const vector<vector<int> >& confusionMatrix, map<int, string>& classLabels, const int totalCount);
void readFilesFromFilterFile(const string directory, const string fileExtension, vector<string> *files, const string filterFile);

#endif /* COMMON_H_ */
