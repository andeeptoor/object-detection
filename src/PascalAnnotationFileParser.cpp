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
	string objectCenterPointString = "Center point on object %d \"%[^\"]\" (X, Y) : (%d, %d)";
	string objectBoundingBoxString = "Bounding box for object %d \"%[^\"]\" (Xmin, Ymin) - (Xmax, Ymax) : (%d, %d) - (%d, %d)";

	ifstream inputFile(filename.c_str());
	if (!inputFile) {
		printf("Cannot open data file [%s]", filename.c_str());
		exit(EXIT_FAILURE);
	}

	string line;
	int objectNumber;
	char label[1024], objectName[1024];
	int x1, x2, y1, y2;
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
			objects[objectNumber - 1].label = string(label);
		} else if (line.substr(0, 20) == objectCenterPointString.substr(0, 20)) {
			sscanf(line.c_str(), objectCenterPointString.c_str(), &objectNumber, &objectName, &x1, &y1);
			objects[objectNumber - 1].centerPoint = pair<int, int>(x1, y1);
		} else if (line.substr(0, 20) == objectBoundingBoxString.substr(0, 20)) {
			sscanf(line.c_str(), objectBoundingBoxString.c_str(), &objectNumber, &objectName, &x1, &y1, &x2, &y2);
			objects[objectNumber - 1].boundingBoxMin = pair<int, int>(x1, y1);
			objects[objectNumber - 1].boundingBoxMax = pair<int, int>(x2, y2);
		} else {
			printf("%s:\n", line.c_str());
		}
	}

	for (int o = 0; o < objects.size(); o++) {
		printf("Object %d:\n", (o + 1));
		printf("\tLabel: %s\n", objects[o].label.c_str());
		printf("\tCenter: (%d,%d)\n", objects[o].centerPoint.first, objects[o].centerPoint.second);
		printf("\tBounding min: (%d,%d)\n", objects[o].boundingBoxMin.first, objects[o].boundingBoxMin.second);
		printf("\tBounding max: (%d,%d)\n", objects[o].boundingBoxMax.first, objects[o].boundingBoxMax.second);
	}

	return objects;
}

