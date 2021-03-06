/*
 * common.cpp
 *
 *  Created on: Oct 30, 2013
 *      Author: atoor
 */

#include "common.h"
#include <iostream>
#include <fstream>

Rect unionOf(Rect r1, Rect r2) {
	int x1 = min(r1.x, r2.x);
	int x2 = max(r1.x + r1.width, r2.x + r2.width);
	int y1 = min(r1.y, r2.y);
	int y2 = max(r1.y + r1.height, r2.y + r2.height);
	return Rect(x1, y1, x2 - x1, y2 - y1);
}


XMLElement* getChild(const string& childName, XMLElement* parentElement) {
	XMLElement* childElement = parentElement->FirstChildElement(childName.c_str());
	if (childElement == NULL) {
		printf("XML ERROR: Element [%s->%s] not found.\n", parentElement->Name(), childName.c_str());
		exit(EXIT_FAILURE);
	}
	return childElement;
}

string getChildText(const string& childName, XMLElement* parentElement) {
	return getChild(childName, parentElement)->GetText();
}

Config readConfigFile(char* configFile) {
	Config config;
	XMLDocument doc;
	doc.LoadFile(configFile);
	XMLElement* configElement = doc.FirstChildElement("config");
	XMLElement* imageElement = getChild("image", configElement);
	config.imageFileExtension = getChildText("fileExtension", imageElement);
	config.imageDirectory = getChildText("directory", imageElement);
	XMLElement* directoryFilterFile = imageElement->FirstChildElement("directoryFilterFile");
	if (directoryFilterFile != NULL) {
		config.imageDirectoryFilterFile = directoryFilterFile->GetText();
	}
	XMLElement* annotationsElement = getChild("annotations", configElement);
	config.annotationsDirectory = getChildText("directory", annotationsElement);
	config.annotationsFileExtension = getChildText("fileExtension", annotationsElement);
	config.annotationsFileFormat = getChildText("fileFormat", annotationsElement);

	XMLElement* detectionElement = getChild("detection", configElement);
	XMLElement* outputImagesElement = detectionElement->FirstChildElement("outputImages");
	if (outputImagesElement != NULL){
		config.detectionOutputImages=true;
		config.detectionOutputImagesDirectory=getChildText("directory", outputImagesElement);
	}

	XMLElement* detectors = getChild("detectors", configElement);
	XMLElement* detectorElement = detectors->FirstChildElement("detector");

	while (detectorElement != NULL) {
		string type = getChildText("type", detectorElement);
		if (utils::equals(type, "HOGObjectDetector")) {
			config.objectDetectors.push_back(new HOGObjectDetector());
		} else if (utils::equals(type, "LatentSVMObjectDetector")) {
			string model = getChildText("model", detectorElement);
			double overlapThreshold = utils::stringToDouble(getChildText("overlapThreshold", detectorElement));
			int numberOfThreads = utils::stringToInt(getChildText("numberOfThreads", detectorElement));
			config.objectDetectors.push_back(new LatentSVMObjectDetector(model, overlapThreshold, numberOfThreads));
		} else if (utils::equals(type, "CascadeObjectDetector")) {
			string model = getChildText("model", detectorElement);
			config.objectDetectors.push_back(new CascadeObjectDetector(model));
		}
		detectorElement = detectorElement->NextSiblingElement("detector");
	}

	return config;
}

void showProgress(int index) {
	cout << ".";
	if (index % 100 == 0) {
		cout << endl;
	}

	cout << flush;
}

void endProgress() {
	cout << endl;
}

void printConfusionMatrix(const vector<vector<int> >& confusionMatrix, map<int, string>& classLabels, const int totalCount) {
	cout << endl << endl;
	cout << "<confusionMatrix>" << endl;
	int correctCount = 0;
	for (int actual = 0; actual < confusionMatrix.size(); ++actual) {
		cout << "\t<" << classLabels[actual] << ">" << endl;
		for (int predicted = 0; predicted < confusionMatrix[actual].size(); ++predicted) {
			cout << "\t\t<" << classLabels[predicted] << ">";
			cout << confusionMatrix[actual][predicted];
			cout << "<\\" << classLabels[predicted] << ">" << endl;
			if (predicted == actual) {
				correctCount += confusionMatrix[actual][predicted];
			}
		}
		cout << "\t</" << classLabels[actual] << ">" << endl;
	}
	cout << "\t<accuracy>" << (double(correctCount) / double(totalCount) * 100.0) << "%</accuracy>" << endl;
	cout << "\t<accuracyCount>" << correctCount << "/" << totalCount << "</accuracyCount>" << endl;
	cout << "</confusionMatrix>" << endl;
}

void readFilesFromFilterFile(const string directory, const string fileExtension, vector<string> *files, const string filterFile) {
	ifstream inputFile(filterFile.c_str());
	if (!inputFile) {
		printf("Cannot open file: [%s]\n", filterFile.c_str());
		exit(EXIT_FAILURE);
	}

	string line, file;
	while (getline(inputFile, line)) {
		istringstream iss(line);
		iss >> file;
		stringstream stream1;
		stream1 << directory << "/" << file << "." << fileExtension;
		files->push_back(stream1.str());
//		printf("Found file: [%s]\n", stream1.str().c_str());
	}
	inputFile.close();
}

