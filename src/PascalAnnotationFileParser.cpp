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

AnnotatedImage PascalAnnotationFileParser::parseAnnotationFile(string filename) {
	AnnotatedImage image;

	string imageSizeString = "Image size (X x Y x C) : %d x %d x %d";
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
			for (int i = 0; i < numberOfObjects; i++) {
				AnnotatedObject a;
				image.objects.push_back(a);
			}
		} else if (line.substr(0, 10) == imageSizeString.substr(0, 10)) {
			sscanf(line.c_str(), imageSizeString.c_str(), &image.imageWidth, &image.imageHeight, &image.numberOfColors);
		} else if (line.substr(0, 20) == objectLabelString.substr(0, 20)) {
			sscanf(line.c_str(), objectLabelString.c_str(), &objectNumber, &objectName, &label);
			image.objects[objectNumber - 1].label = string(label);
		} else if (line.substr(0, 20) == objectCenterPointString.substr(0, 20)) {
			sscanf(line.c_str(), objectCenterPointString.c_str(), &objectNumber, &objectName, &x1, &y1);
			image.objects[objectNumber - 1].centerPoint = Point(x1, y1);
		} else if (line.substr(0, 20) == objectBoundingBoxString.substr(0, 20)) {
			sscanf(line.c_str(), objectBoundingBoxString.c_str(), &objectNumber, &objectName, &x1, &y1, &x2, &y2);
			Rect r(Point(x1,y1),Point(x2,y2));
			image.objects[objectNumber - 1].boundingBox = r;
		} else {
//			printf("%s:\n", line.c_str());
		}
	}

	printf("Image Width: %d\n", image.imageWidth);
	printf("Image Height: %d\n", image.imageHeight);
	printf("Number of colors: %d\n", image.numberOfColors);
	for (int o = 0; o < image.objects.size(); o++) {
		printf("Object %d:\n", (o + 1));
		printf("\tLabel: %s\n", image.objects[o].label.c_str());
		printf("\tCenter: (%d,%d)\n", image.objects[o].centerPoint.x, image.objects[o].centerPoint.y);
		printf("\tBounding min: (%d,%d)\n", image.objects[o].boundingBox.tl().x, image.objects[o].boundingBox.tl().y);
		printf("\tBounding max: (%d,%d)\n", image.objects[o].boundingBox.br().x, image.objects[o].boundingBox.br().y);
	}

	return image;
}

