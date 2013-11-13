/*
 * PascalAnnotationFileParser.cpp
 *
 *  Created on: Nov 13, 2013
 *      Author: atoor
 */

#include "PascalAnnotationFileParser.h"
#include <fstream>
#include <stdio.h>

PascalAnnotationFileParser::PascalAnnotationFileParser() {

}

PascalAnnotationFileParser::~PascalAnnotationFileParser() {
}

vector<AnnotatedObject> PascalAnnotationFileParser::parseAnnotationFile(string filename) {
	vector<AnnotatedObject> objects;

	string numberOfObjectsString = "Objects with ground truth : %d";
	string objectLabelString = "Original label for object %d \"%[^\"]\" : \"%[^\"]\"";
	string objectCenterPointString = "Objects with ground truth : %d";
	string objectBoundingBoxString = "Objects with ground truth : %d";

	ifstream inputFile(filename.c_str());
	if (!inputFile) {
		printf("Cannot open data file [%s]", filename.c_str());
		exit(EXIT_FAILURE);
	}

	string line;
	int objectNumber;
	char label[1024], objectName[1024];
	while (getline(inputFile, line)) {
		if (line.substr(0, 20) == numberOfObjectsString.substr(0, 20)) {
			int numberOfObjects;
			sscanf(line.c_str(), numberOfObjectsString.c_str(), &numberOfObjects);
			printf("Number of objects: %d\n", numberOfObjects);
			for (int i = 0; i < numberOfObjects; i++) {
				AnnotatedObject a;
				objects.push_back(a);
			}
		} else if (line.substr(0, 20) == objectLabelString.substr(0, 20)) {
			sscanf(line.c_str(), objectLabelString.c_str(), &objectNumber, &objectName, &label);
			printf("Object %d label: %s\n", objectNumber, label);
			objects[objectNumber-1].label = string(label);
		} else {
			printf("%s:\n", line.c_str());
		}
	}

	for (int o = 0; o < objects.size(); o++) {
		printf("Object %d:\n" , (o+1));
		printf("\tLabel: %s\n" , objects[o].label.c_str());
	}

	return objects;
}

