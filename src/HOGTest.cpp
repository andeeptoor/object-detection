#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fstream>

#include "common.h"
#include "math.h"
#include "PascalAnnotationFileParser.h"
#include "ObjectDetector.h"

using namespace cv;
using namespace std;

struct Evaluation {
	int correctObjects;
	int incorrectObjects;
	int totalObjects;
};

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
		} else {
			evaluation.incorrectObjects++;
		}
	}
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: <configFile>");
		return EXIT_FAILURE;
	}

	Config config = readConfigFile(argv[1]);
	PascalAnnotationFileParser parser;
	vector<ObjectDetector *> detectors = config.objectDetectors;
	vector<Evaluation> evaluations(detectors.size());

	vector<string> files;
	utils::recursivelySearchDirectoryForFiles(config.imageDirectory, config.fileExtension, &files);

	Mat image;
	string filename;
	int i;
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

		for (i = 0; i < detectors.size(); i++) {
			TickMeter tm;
			tm.start();
			vector<Rect> predictedObjects = detectors[i]->detectObjects(image);
			tm.stop();
			printf("Detection time = %f sec\n",tm.getTimeSec());
			evaluatePredictions(evaluations[i], annotatedImage, predictedObjects);
		}
	}

	for (i = 0; i < evaluations.size(); i++) {
		printf("Evaluation %d:\n", i+1);
		printf("\tTrue detection rate:%f%%\n", double(evaluations[i].correctObjects) / double(evaluations[i].totalObjects) * 100.0);
		printf("\tFalse alarm rate:%f%%\n", double(evaluations[i].incorrectObjects) / double(files.size()) * 100.0);
		printf("\tNumber correct:%d\n", evaluations[i].correctObjects);
		printf("\tNumber incorrect:%d\n", evaluations[i].incorrectObjects);
		printf("\tTotal objects:%d\n", evaluations[i].totalObjects);
	}
	return EXIT_SUCCESS;
}
