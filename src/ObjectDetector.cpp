/*
 * ObjectDetector.cpp
 *
 *  Created on: Nov 18, 2013
 *      Author: atoor
 */

#include "ObjectDetector.h"

ObjectDetector::~ObjectDetector() {

}

HOGObjectDetector::HOGObjectDetector() {
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
}

vector<Rect> HOGObjectDetector::detectObjects(Mat image) {
	vector<Rect> found, foundFiltered;
	int i, j;
	Rect r;

	fflush(stdout);
	// run the detector with default parameters. to get a higher hit-rate
	// (and more false alarms, respectively), decrease the hitThreshold and
	// groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
	hog.detectMultiScale(image, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);
	for (i = 0; i < found.size(); i++) {
		r = found[i];
		for (j = 0; j < found.size(); j++) {
			if (j != i && (r & found[j]) == r) {
				break;
			}
		}
		if (j == found.size()) {
			foundFiltered.push_back(r);
		}
	}
	return foundFiltered;
}

LatentSVMObjectDetector::LatentSVMObjectDetector(string model) {
	vector<string> modelFilenames;
	modelFilenames.push_back(model);
	detector.load(modelFilenames);
	if (detector.empty()) {
		printf("Model [%s] can't be loaded\n", model.c_str());
		exit(EXIT_FAILURE);
	}

	const vector<string>& classNames = detector.getClassNames();
	printf("Loaded %lu models:\n", classNames.size());
	for (size_t i = 0; i < classNames.size(); i++) {
		printf("\tModel %lu: [%s]\n", i + 1, classNames[i].c_str());
	}
}

vector<Rect> LatentSVMObjectDetector::detectObjects(Mat image) {
	vector<Rect> foundFiltered;
	return foundFiltered;
}
