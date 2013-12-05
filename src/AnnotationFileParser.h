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
#include "common.h"

using namespace cv;
using namespace std;

class AnnotationFileParser {
public:
	AnnotationFileParser();
	virtual ~AnnotationFileParser();
	AnnotatedImage parseAnnotationFile(string annotationFile, string format);
private:
	AnnotatedImage parsePascalTextAnnotationFile(string annotationFile);
	AnnotatedImage parsePascalXmlAnnotationFile(string annotationFile);
	AnnotatedImage parseCaltechAnnotationFile(string annotationFile);
	AnnotatedImage parseFDDBAnnotationFile(string annotationFile);
};


#endif /* ANNOTATIONFILEPARSER_H_ */
