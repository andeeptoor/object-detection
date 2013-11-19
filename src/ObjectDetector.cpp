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
	printf("Detector: %s\n", name().c_str());
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

LatentSVMObjectDetector::LatentSVMObjectDetector(string model, double _overlapThreshold, int _numberOfThreads) {
	this->overlapThreshold = _overlapThreshold;
	this->numberOfThreads = _numberOfThreads;

	printf("Detector: %s\n", name().c_str());

	vector<string> modelFilenames;
	modelFilenames.push_back(model);
	detector.load(modelFilenames);
	if (detector.empty()) {
		printf("Model [%s] can't be loaded\n", model.c_str());
		exit(EXIT_FAILURE);
	}

	const vector<string>& classNames = detector.getClassNames();
	printf("\tLoaded %lu models:\n", classNames.size());
	for (size_t i = 0; i < classNames.size(); i++) {
		printf("\t\tModel %lu: [%s]\n", i + 1, classNames[i].c_str());
	}
}

vector<Rect> LatentSVMObjectDetector::detectObjects(Mat image) {
	vector<Rect> foundFiltered;

	vector<LatentSvmDetector::ObjectDetection> detections;
	detector.detect(image, detections, overlapThreshold, numberOfThreads);
	for (size_t i = 0; i < detections.size(); i++) {
		const LatentSvmDetector::ObjectDetection& od = detections[i];
		foundFiltered.push_back(od.rect);
	}
	return foundFiltered;
}

HaarCascadeObjectDetector::HaarCascadeObjectDetector(string model) {
	DetectionBasedTracker::Parameters param;
//	param.maxObjectSize = 400;
//	param.maxTrackLifetime = 20;
//	param.minDetectionPeriod = 7;
//	param.minNeighbors = 3;
//	param.minObjectSize = 20;
//	param.scaleFactor = 1.1;
	detector = new DetectionBasedTracker(model, param);
	detector->run();
	printf("Detector: %s\n", name().c_str());
}

vector<Rect> HaarCascadeObjectDetector::detectObjects(Mat image) {
	Mat gray;
	cvtColor(image,gray,CV_RGB2GRAY);
	detector->process(gray);
	vector<Rect> found;
	detector->getObjects(found);
	return found;
}
