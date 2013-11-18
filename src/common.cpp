/*
 * common.cpp
 *
 *  Created on: Oct 30, 2013
 *      Author: atoor
 */

#include "common.h"
#include <iostream>

Config readConfigFile(char* configFile) {
	Config config;
	XMLDocument doc;
	doc.LoadFile(configFile);
	XMLElement* configElement = doc.FirstChildElement("config");
	XMLElement* imageElement = configElement->FirstChildElement("image");
	config.fileExtension = imageElement->FirstChildElement("fileExtension")->GetText();
	config.imageDirectory = imageElement->FirstChildElement("directory")->GetText();
	config.imageAnnotationsDirectory = imageElement->FirstChildElement("annotationsDirectory")->GetText();

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

