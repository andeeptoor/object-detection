#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fstream>

#include "common.h"
#include "math.h"
#include "PascalAnnotationFileParser.h"

using namespace cv;
using namespace std;

vector<Rect> findObjects(const HOGDescriptor hog, const Mat image) {
	vector<Rect> found, foundFiltered;
	int i, j;
	Rect r;

	fflush(stdout);
	double t = (double) getTickCount();
	// run the detector with default parameters. to get a higher hit-rate
	// (and more false alarms, respectively), decrease the hitThreshold and
	// groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
	hog.detectMultiScale(image, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);
	t = (double) getTickCount() - t;
	printf("tdetection time = %gms\n", t * 1000. / cv::getTickFrequency());
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

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: peopledetect (<image_filename> | <image_list>.txt)\n");
		return EXIT_FAILURE;
	}

	Config config = readConfigFile(argv[1]);
	PascalAnnotationFileParser parser;

	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	namedWindow("people detector", 1);

	vector<string> files;
	utils::recursivelySearchDirectoryForFiles(config.imageDirectory, config.fileExtension, &files);

	Mat image;
	string filename;
	int i;
	Rect r, r2;
	Point actualCenter, compareCenter;
	for (int f = 0; f < files.size(); f++) {
		filename = files[f];

		string annotationFile = utils::convertToFileExtension(filename, "txt");
		AnnotatedImage annotatedImage = parser.parseAnnotationFile(annotationFile);

		if (annotatedImage.numberOfColors == 1) {
			image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
		} else {
			image = imread(filename, CV_LOAD_IMAGE_COLOR);
		}

		resize(image, image, Size(annotatedImage.imageWidth, annotatedImage.imageHeight));
		printf("%s:\n", filename.c_str());
		if (!image.data) {
			continue;
		}

		vector<Rect> predictedObjects = findObjects(hog, image);

//		for (i = 0; i < predictedObjects.size(); i++) {
//			Rect r = predictedObjects[i];
//			rectangle(image, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
//
//			compareCenter.x = (r.tl().x + r.br().x) / 2;
//			compareCenter.y = (r.tl().y + r.br().y) / 2;
//			circle(image, compareCenter, 3, cv::Scalar(0, 255, 0), 3);
//		}

		//Matching criteria: http://groups.inf.ed.ac.uk/calvin/ethz_pascal_stickmen/downloads/README.txt
//
		for (i = 0; i < annotatedImage.objects.size(); i++) {
			Rect r = annotatedImage.objects[i].boundingBox;
			rectangle(image, r.tl(), r.br(), cv::Scalar(255, 0, 0), 3);

			actualCenter.x = (r.tl().x + r.br().x) / 2;
			actualCenter.y = (r.tl().y + r.br().y) / 2;
			circle(image, actualCenter, 3, cv::Scalar(255, 0, 0), 3);
		}

		int bestObject;
		double bestDistance, currentDistance;
		for (int p = 0; p < annotatedImage.objects.size(); p++) {
			r = annotatedImage.objects[i].boundingBox;
			actualCenter.x = (r.tl().x + r.br().x) / 2;
			actualCenter.y = (r.tl().y + r.br().y) / 2;
			bestObject = -1;
			bestDistance = INFINITY;
			for (int a = 0; a < predictedObjects.size(); a++) {
				r2 = predictedObjects[a];
				compareCenter.x = (r2.tl().x + r2.br().x) / 2;
				compareCenter.y = (r2.tl().y + r2.br().y) / 2;
				currentDistance = sqrt(pow(double(actualCenter.x - compareCenter.x), 2) + pow(double(actualCenter.y - compareCenter.y), 2));
				if (currentDistance < bestDistance) {
					bestDistance = currentDistance;
					bestObject = a;
				}
			}

			if (bestObject >= 0) {
				r = predictedObjects[bestObject];
				rectangle(image, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
			}
		}

		imshow("people detector", image);

		int c = waitKey(0) & 255;
		if (c == 'q' || c == 'Q' || !f) {
			break;
		}
	}
	return EXIT_SUCCESS;
}
