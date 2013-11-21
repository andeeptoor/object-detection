/*
 * PascalAnnotationFileParser.h
 *
 *  Created on: Nov 13, 2013
 *      Author: atoor
 */

#ifndef PASCALANNOTATIONFILEPARSER_H_
#define PASCALANNOTATIONFILEPARSER_H_

#include <string>
#include <vector>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

struct AnnotatedObject {
	string label;
	Point centerPoint;
	Rect boundingBox;
};

struct AnnotatedImage{
	int imageWidth, imageHeight, numberOfColors;
	vector<AnnotatedObject> objects;
};

class PascalAnnotationFileParser {
public:
	PascalAnnotationFileParser();
	virtual ~PascalAnnotationFileParser();
	AnnotatedImage parseAnnotationFile(string annotationFile);
private:
	AnnotatedImage parseTextAnnotationFile(string annotationFile);
	AnnotatedImage parseXmlAnnotationFile(string annotationFile);
};


#endif /* PASCALANNOTATIONFILEPARSER_H_ */
