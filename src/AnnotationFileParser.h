/*
 * PascalAnnotationFileParser.h
 *
 *  Created on: Nov 13, 2013
 *      Author: atoor
 */

#ifndef PASCALANNOTATIONFILEPARSER_H_
#define ANNOTATIONFILEPARSER_H_

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

class AnnotationFileParser {
public:
	AnnotationFileParser();
	virtual ~AnnotationFileParser();
	AnnotatedImage parseAnnotationFile(string annotationFile, string format);
private:
	AnnotatedImage parsePascalTextAnnotationFile(string annotationFile);
	AnnotatedImage parsePascalXmlAnnotationFile(string annotationFile);
	AnnotatedImage parseCaltechAnnotationFile(string annotationFile);
};


#endif /* ANNOTATIONFILEPARSER_H_ */
