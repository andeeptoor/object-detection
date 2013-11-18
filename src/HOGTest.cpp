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

struct Evaluation {
	int correctObjects;
	int incorrectObjects;
	int totalObjects;
};

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
	printf("\tDetection time = %gms\n", t * 1000. / cv::getTickFrequency());
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

Rect unionOf(Rect r1, Rect r2) {
	int x1 = min(r1.x, r2.x);
	int x2 = max(r1.x + r1.width, r2.x + r2.width);
	int y1 = min(r1.y, r2.y);
	int y2 = max(r1.y + r1.height, r2.y + r2.height);
	return Rect(x1, y1, x2 - x1, y2 - y1);
}

void evaluatePredictions(Evaluation &evaluation, const AnnotatedImage annotatedImage, const vector<Rect> predictedObjects) {
	Rect actual, predicted, intersection, unionRect;
	double bestOverlap, currentOverlap;
	vector<bool> found(annotatedImage.objects.size(), false);
	int bestObject;
	evaluation.totalObjects += annotatedImage.objects.size();
	for (int predictedIndex = 0; predictedIndex < predictedObjects.size(); predictedIndex++) {
		predicted = predictedObjects[predictedIndex];
		bestOverlap = 0;
		currentOverlap = 0;
		for (int actualIndex = 0; actualIndex < annotatedImage.objects.size(); actualIndex++) {
			actual = annotatedImage.objects[actualIndex].boundingBox;
			intersection = (predicted & actual);
			unionRect = unionOf(predicted, actual);
			currentOverlap = double(intersection.area()) / double(unionRect.area());
			if (currentOverlap > 0.5 && currentOverlap > bestOverlap) {
				bestOverlap = currentOverlap;
				bestObject = actualIndex;
				printf("\tBest overlap: %f\n", bestOverlap);
			}
		}

		if (bestOverlap > 0) {
			if (!found[bestObject]) {
				evaluation.correctObjects++;
				found[bestObject] = true;
			} else {
				evaluation.incorrectObjects++;
			}
		}
	}
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
	Evaluation evaluation;
	for (int f = 0; f < files.size(); f++) {
		filename = files[f];

		string annotationFile = utils::convertToParentDirectory(filename, config.imageAnnotationsDirectory);
		annotationFile = utils::convertToFileExtension(annotationFile, "txt");
		AnnotatedImage annotatedImage = parser.parseAnnotationFile(annotationFile);

		if (annotatedImage.numberOfColors == 1) {
			image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
		} else {
			image = imread(filename, CV_LOAD_IMAGE_COLOR);
		}

		resize(image, image, Size(annotatedImage.imageWidth, annotatedImage.imageHeight));
		printf("File[%d/%lu]: %s\n", f + 1, files.size(), filename.c_str());
		if (!image.data) {
			continue;
		}

		vector<Rect> predictedObjects = findObjects(hog, image);
		evaluatePredictions(evaluation, annotatedImage, predictedObjects);
	}

	printf("Evaluation:\n");
	printf("\tPercent correct:%f%%\n", double(evaluation.correctObjects) / double(evaluation.totalObjects) * 100.0);
	printf("\tNumber correct:%d\n", evaluation.correctObjects);
	printf("\tNumber incorrect:%d\n", evaluation.incorrectObjects);
	printf("\tTotal objects:%d\n", evaluation.totalObjects);
	return EXIT_SUCCESS;
}
