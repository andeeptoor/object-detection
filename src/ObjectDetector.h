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

using namespace cv;

class ObjectDetector {
public:
	virtual ~ObjectDetector();
	virtual vector<Rect> detectObjects(Mat image)=0;
	virtual string name()=0;
};

class HOGObjectDetector : public ObjectDetector{

public:
	HOGObjectDetector();
	vector<Rect> detectObjects(Mat image);
	string name(){return "HOGObjectDetector";}
private:
	HOGDescriptor hog;
};

class LatentSVMObjectDetector : public ObjectDetector{

public:
	LatentSVMObjectDetector(string model, double _overlapThreshold, int _numberOfThreads);
	vector<Rect> detectObjects(Mat image);
	string name(){return "LatentSVMObjectDetector";}
private:
	LatentSvmDetector detector;
	double overlapThreshold;
	int numberOfThreads;
};

#endif /* OBJECTDETECTOR_H_ */
