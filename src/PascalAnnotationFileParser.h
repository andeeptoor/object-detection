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

using namespace std;

struct AnnotatedObject {
	string label;
	pair<int, int> centerPoint;
	pair<int,int> boundingBoxMin;
	pair<int,int> boundingBoxMax;
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
};


#endif /* PASCALANNOTATIONFILEPARSER_H_ */
