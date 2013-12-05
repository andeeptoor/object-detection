/*
 * ObjectDetector.h
 *
 *  Created on: Nov 18, 2013
 *      Author: atoor
 */

#ifndef OBJECTDETECTOR_H_
#define OBJECTDETECTOR_H_

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/contrib/detection_based_tracker.hpp"

using namespace cv;

struct DetectedObject {
	Rect boundingBox;
	double confidenceLevel;
};

class ObjectDetector {
public:
	virtual ~ObjectDetector();
	virtual vector<DetectedObject> detectObjects(Mat image)=0;
	virtual string name()=0;
};

class HOGObjectDetector: public ObjectDetector {

public:
	HOGObjectDetector();
	vector<DetectedObject> detectObjects(Mat image);
	string name() {
		return "HOGObjectDetector";
	}
private:
	HOGDescriptor hog;
};

class LatentSVMObjectDetector: public ObjectDetector {

public:
	LatentSVMObjectDetector(string model, double _overlapThreshold, int _numberOfThreads);
	vector<DetectedObject> detectObjects(Mat image);
	string name() {
		return "LatentSVMObjectDetector";
	}
private:
	LatentSvmDetector detector;
	double overlapThreshold;
	int numberOfThreads;
};

class CascadeObjectDetector: public ObjectDetector {
public:
	CascadeObjectDetector(string _model);
	vector<DetectedObject> detectObjects(Mat image);
	string name() {
		return this->nameAndModel;
	}
private:
//	CascadeClassifier cascade;
	DetectionBasedTracker * detector;
	string nameAndModel;
};

#endif /* OBJECTDETECTOR_H_ */
