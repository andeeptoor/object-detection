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
};

class HOGObjectDetector : public ObjectDetector{

public:
	HOGObjectDetector();
	vector<Rect> detectObjects(Mat image);
private:
	HOGDescriptor hog;
};

#endif /* OBJECTDETECTOR_H_ */
