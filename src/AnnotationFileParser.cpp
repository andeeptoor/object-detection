#include "AnnotationFileParser.h"
#include "utils.h"
#include "common.h"
#include <fstream>
#include <stdio.h>

using namespace tinyxml2;

AnnotationFileParser::AnnotationFileParser() {

}

AnnotationFileParser::~AnnotationFileParser() {
}

AnnotatedImage AnnotationFileParser::parseAnnotationFile(string filename, string format) {
	if (utils::equals(format, "pascal")) {
		if (utils::matchesFileExtension(filename, "txt")) {
			return parsePascalTextAnnotationFile(filename);
		} else if (utils::matchesFileExtension(filename, "xml")) {
			return parsePascalXmlAnnotationFile(filename);
		}
	} else if (utils::equals(format, "caltech")) {
		return parseCaltechAnnotationFile(filename);
	} else if (utils::equals(format, "fddb")) {
		return parseFDDBAnnotationFile(filename);
	} else if (utils::equals(format, "youtube")) {
		return parseYouTubeAnnotationFile(filename);
	} else if (utils::equals(format, "picasa")) {
		return parsePicasaAnnotationFile(filename);
	}

	printf("Unknown format/file extension for annotation file [%s/%s]", format.c_str(), filename.c_str());
	exit(EXIT_FAILURE);

}

AnnotatedImage AnnotationFileParser::parsePicasaAnnotationFile(string filename) {
	AnnotatedImage image;
	ifstream inputFile(filename.c_str());
	if (!inputFile) {
		printf("Cannot open data file [%s]\n", filename.c_str());
		exit(EXIT_FAILURE);
	}

	string line;
	int x1, x2, y1, y2, width, height;

	//imageWidth imageHeight faceX1 faceY1 faceX2 faceY2

	while (getline(inputFile, line)) {
		istringstream ss(line);
		ss >> width >> height;
		ss >> x1 >> y1 >> x2 >> y2;

		AnnotatedObject a;
//		Rect r(Point(x1 * width, y1 * height), Point(x2 * width, y2 * height));
		Rect r(Point(x1, y1), Point(x1 + x2, y1 + y2));
		a.boundingBox = r;
		image.objects.push_back(a);
	}

	return image;
}

AnnotatedImage AnnotationFileParser::parseYouTubeAnnotationFile(string filename) {
	AnnotatedImage image;
	ifstream inputFile(filename.c_str());
	if (!inputFile) {
		printf("Cannot open data file [%s]\n", filename.c_str());
		exit(EXIT_FAILURE);
	}

	string line;
	int centerX, width, height, centerY, ignore;
	char comma;

	//[ignore],x,y,width,height,[ignore],[ignore]

	while (getline(inputFile, line)) {
		istringstream ss(line);
		ss >> ignore >> comma;
		ss >> centerX >> comma >> centerY >> comma;
		ss >> width >> comma >> height >> comma;

		AnnotatedObject a;
		Rect r(Point(centerX - width, centerY - height), Point(centerX + width, centerY + height));
		a.boundingBox = r;
		image.objects.push_back(a);
	}

	return image;
}

AnnotatedImage AnnotationFileParser::parseCaltechAnnotationFile(string filename) {
	AnnotatedImage image;
	ifstream inputFile(filename.c_str());
	if (!inputFile) {
		printf("Cannot open data file [%s]\n", filename.c_str());
		exit(EXIT_FAILURE);
	}

	string line;
	double x1, x2, y1, y2, ignore;
	getline(inputFile, line);
	istringstream ss(line);
	char comma;

	ss >> ignore >> comma >> ignore >> comma;
	ss >> x1 >> comma >> y1 >> comma;
	ss >> ignore >> comma >> ignore >> comma;
	ss >> x2 >> comma >> y2;
	AnnotatedObject a;
	Rect r(Point(x1, y1), Point(x2, y2));
	a.boundingBox = r;
	image.objects.push_back(a);
	return image;
}

AnnotatedImage AnnotationFileParser::parseFDDBAnnotationFile(string filename) {
	AnnotatedImage image;
	ifstream inputFile(filename.c_str());
	if (!inputFile) {
		printf("Cannot open data file [%s]\n", filename.c_str());
		exit(EXIT_FAILURE);
	}

	//<major_axis_radius minor_axis_radius angle center_x center_y 1>
	string line;
	double majorAxisRadius, minorAxisRadius, angle, centerX, centerY;

	while (getline(inputFile, line)) {
		istringstream ss(line);
		ss >> majorAxisRadius >> minorAxisRadius;
		ss >> angle;
		ss >> centerX >> centerY;

		AnnotatedObject a;
		Rect r(Point(centerX - minorAxisRadius, centerY - majorAxisRadius), Point(centerX + minorAxisRadius, centerY + majorAxisRadius));
		a.boundingBox = r;
		image.objects.push_back(a);
	}

	return image;
}

AnnotatedImage AnnotationFileParser::parsePascalXmlAnnotationFile(string filename) {
	AnnotatedImage image;

	XMLDocument doc;
	doc.LoadFile(filename.c_str());
	XMLElement* annotationElement = doc.FirstChildElement("annotation");
	XMLElement * sizeElement = annotationElement->FirstChildElement("size");
	if (sizeElement != NULL) {
		image.imageWidth = utils::stringToInt(getChildText("width", sizeElement));
		image.imageHeight = utils::stringToInt(getChildText("height", sizeElement));
		XMLElement* depthElement = sizeElement->FirstChildElement("depth");
		if (depthElement != NULL) {
			image.numberOfColors = utils::stringToInt(depthElement->GetText());
		} else {
			image.numberOfColors = 3;
		}
	}

	int x1, x2, y1, y2;
	XMLElement * objectElement = annotationElement->FirstChildElement("object");
	while (objectElement != NULL) {
		AnnotatedObject a;
		a.label = getChild("name", objectElement)->GetText();
		XMLElement * boundingBox = getChild("bndbox", objectElement);
		x1 = utils::stringToInt(getChildText("xmin", boundingBox));
		x2 = utils::stringToInt(getChildText("xmax", boundingBox));
		y1 = utils::stringToInt(getChildText("ymin", boundingBox));
		y2 = utils::stringToInt(getChildText("ymax", boundingBox));

		Rect r(Point(x1, y1), Point(x2, y2));
		a.boundingBox = r;

		image.objects.push_back(a);
		objectElement = annotationElement->NextSiblingElement("object");
	}
	return image;
}

AnnotatedImage AnnotationFileParser::parsePascalTextAnnotationFile(string filename) {
	AnnotatedImage image;

	string imageSizeString = "Image size (X x Y x C) : %d x %d x %d";
	string numberOfObjectsString = "Objects with ground truth : %d";
	string objectLabelString = "Original label for object %d \"%[^\"]\" : \"%[^\"]\"";
	string objectCenterPointString = "Center point on object %d \"%[^\"]\" (X, Y) : (%d, %d)";
	string objectBoundingBoxString = "Bounding box for object %d \"%[^\"]\" (Xmin, Ymin) - (Xmax, Ymax) : (%d, %d) - (%d, %d)";

	ifstream inputFile(filename.c_str());
	if (!inputFile) {
		printf("Cannot open data file [%s]\n", filename.c_str());
		exit(EXIT_FAILURE);
	}

	string line;
	int objectNumber;
	char label[1024], objectName[1024];
	int x1, x2, y1, y2;
	while (getline(inputFile, line)) {
		if (line.substr(0, 20) == numberOfObjectsString.substr(0, 20)) {
			int numberOfObjects;
			sscanf(line.c_str(), numberOfObjectsString.c_str(), &numberOfObjects);
			for (int i = 0; i < numberOfObjects; i++) {
				AnnotatedObject a;
				image.objects.push_back(a);
			}
		} else if (line.substr(0, 10) == imageSizeString.substr(0, 10)) {
			sscanf(line.c_str(), imageSizeString.c_str(), &image.imageWidth, &image.imageHeight, &image.numberOfColors);
		} else if (line.substr(0, 20) == objectLabelString.substr(0, 20)) {
			sscanf(line.c_str(), objectLabelString.c_str(), &objectNumber, &objectName, &label);
			image.objects[objectNumber - 1].label = string(label);
		} else if (line.substr(0, 20) == objectCenterPointString.substr(0, 20)) {
			sscanf(line.c_str(), objectCenterPointString.c_str(), &objectNumber, &objectName, &x1, &y1);
			image.objects[objectNumber - 1].centerPoint = Point(x1, y1);
		} else if (line.substr(0, 20) == objectBoundingBoxString.substr(0, 20)) {
			sscanf(line.c_str(), objectBoundingBoxString.c_str(), &objectNumber, &objectName, &x1, &y1, &x2, &y2);
			Rect r(Point(x1, y1), Point(x2, y2));
			image.objects[objectNumber - 1].boundingBox = r;
		} else {
//			printf("%s:\n", line.c_str());
		}
	}

//	printf("Image Width: %d\n", image.imageWidth);
//	printf("Image Height: %d\n", image.imageHeight);
//	printf("Number of colors: %d\n", image.numberOfColors);
//	for (int o = 0; o < image.objects.size(); o++) {
//		printf("Object %d:\n", (o + 1));
//		printf("\tLabel: %s\n", image.objects[o].label.c_str());
//		printf("\tCenter: (%d,%d)\n", image.objects[o].centerPoint.x, image.objects[o].centerPoint.y);
//		printf("\tBounding min: (%d,%d)\n", image.objects[o].boundingBox.tl().x, image.objects[o].boundingBox.tl().y);
//		printf("\tBounding max: (%d,%d)\n", image.objects[o].boundingBox.br().x, image.objects[o].boundingBox.br().y);
//	}

	return image;
}

