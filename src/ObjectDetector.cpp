/*
 * ObjectDetector.cpp
 *
 *  Created on: Nov 18, 2013
 *      Author: atoor
 */

#include "ObjectDetector.h"
#include "utils.h"

ObjectDetector::~ObjectDetector() {

}

HOGObjectDetector::HOGObjectDetector() {
	printf("Detector: %s\n", name().c_str());
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
}

vector<DetectedObject> HOGObjectDetector::detectObjects(Mat image) {
	vector<Rect> found;
	vector<DetectedObject> foundFiltered;
	int i, j;
	Rect r;

	fflush(stdout);
	// run the detector with default parameters. to get a higher hit-rate
	// (and more false alarms, respectively), decrease the hitThreshold and
	// groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
	hog.detectMultiScale(image, found, 0.5, Size(8, 8), Size(32, 32), 1.05, 2);
	for (i = 0; i < found.size(); i++) {
		r = found[i];
		for (j = 0; j < found.size(); j++) {
			if (j != i && (r & found[j]) == r) {
				break;
			}
		}
		if (j == found.size()) {
			DetectedObject o;
			o.boundingBox = r;
			foundFiltered.push_back(o);
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

vector<DetectedObject> LatentSVMObjectDetector::detectObjects(Mat image) {
	vector<DetectedObject> foundFiltered;

	vector<LatentSvmDetector::ObjectDetection> detections;
	detector.detect(image, detections, overlapThreshold, numberOfThreads);
	for (int i = 0; i < detections.size(); i++) {
		const LatentSvmDetector::ObjectDetection& od = detections[i];
		DetectedObject o;
		o.boundingBox = od.rect;
		o.confidenceLevel = od.score;
		foundFiltered.push_back(o);
	}
	return foundFiltered;
}

CascadeObjectDetector::CascadeObjectDetector(string _model) {
	DetectionBasedTracker::Parameters param;
//	param.maxObjectSize = 400;
//	param.maxTrackLifetime = 20;
//	param.minDetectionPeriod = 7;
//	param.minNeighbors = 4;
//	param.minObjectSize = 20;
	param.scaleFactor = 1.2;

	detector = new DetectionBasedTracker(_model, param);
	detector->run();
	string fileWithoutParentDirectory = utils::getFileWithoutParentDirectory(_model);
	this->nameAndModel = utils::append(utils::append("CascadeObjectDetector [", fileWithoutParentDirectory), "]");
	printf("Detector: %s\n", name().c_str());
}

vector<DetectedObject> CascadeObjectDetector::detectObjects(Mat image) {
	Mat gray;
	if (image.channels() > 1) {
		cvtColor(image, gray, CV_RGB2GRAY);
	} else {
		gray = image;
	}
	equalizeHist(gray, gray);
	detector->process(gray);
	vector<Rect> found;
	detector->getObjects(found);
	vector<DetectedObject> objects;
	for (int i = 0; i < found.size(); i++) {
		DetectedObject o;
		o.boundingBox = found[i];
		objects.push_back(o);
	}
	return objects;
}

//
//CascadeObjectDetector::CascadeObjectDetector(string _model) {
////	DetectionBasedTracker::Parameters param;
////	param.maxObjectSize = 400;
////	param.maxTrackLifetime = 20;
////	param.minDetectionPeriod = 7;
////	param.minNeighbors = 3;
////	param.minObjectSize = 20;
////	param.scaleFactor = 1.2;
//
////	detector = new DetectionBasedTracker(_model, param);
////	detector->run();
//	if (!cascade.load(_model)) {
//		exit(EXIT_FAILURE);
//	}
//	string fileWithoutParentDirectory = utils::getFileWithoutParentDirectory(_model);
//	this->nameAndModel = utils::append(utils::append("CascadeObjectDetector [", fileWithoutParentDirectory), "]");
//	printf("Detector: %s\n", name().c_str());
//}
//
////From http://haoxiang.org/2013/11/opencv-detectmultiscale-output-detection-score/
//vector<DetectedObject> CascadeObjectDetector::detectObjects(Mat image) {
//	Mat gray;
//	cvtColor(image, gray, CV_RGB2GRAY);
//	equalizeHist(gray, gray);
//
//	const float scale_factor(1.2f);
//	const int min_neighbors(3);
////	vector<int> reject_levels;
////	vector<double> levelWeights;
//	vector<Rect> found;
////	cascade.detectMultiScale(gray, found, reject_levels, levelWeights, scale_factor, min_neighbors, 0, Size(), Size(), true);
//	cascade.detectMultiScale(gray, found, scale_factor, min_neighbors, 0, Size(), Size());
////	detector->getObjects(found);
//	vector<DetectedObject> objects;
//	for (int i = 0; i < found.size(); i++) {
//		DetectedObject o;
//		o.boundingBox = found[i];
////		o.confidenceLevel=levelWeights[i];
//		objects.push_back(o);
//	}
//	return objects;
//}
