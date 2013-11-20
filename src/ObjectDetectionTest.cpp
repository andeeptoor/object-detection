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

struct EvaluationImage {
	int truePositive;
	int falsePositive;
	int numberOfPositives;
	double recall;
	double precision;
};

struct Evaluation {
	vector<EvaluationImage> evaluations;
	EvaluationImage total;
	double averagePrecision;
	double totalDetectionTime;
};

Rect unionOf(Rect r1, Rect r2) {
	int x1 = min(r1.x, r2.x);
	int x2 = max(r1.x + r1.width, r2.x + r2.width);
	int y1 = min(r1.y, r2.y);
	int y2 = max(r1.y + r1.height, r2.y + r2.height);
	return Rect(x1, y1, x2 - x1, y2 - y1);
}

void evaluatePredictions(Evaluation &evaluation, const AnnotatedImage annotatedImage, const vector<DetectedObject> predictedObjects) {
	Rect actual, predicted, intersection, unionRect;
	double bestOverlap, currentOverlap;
	vector<bool> found(annotatedImage.objects.size(), false);
	int bestObject;
	EvaluationImage current;
	current.numberOfPositives = 0;
	current.falsePositive = 0;
	current.truePositive = 0;
	current.numberOfPositives += annotatedImage.objects.size();
	for (int predictedIndex = 0; predictedIndex < predictedObjects.size(); predictedIndex++) {
		predicted = predictedObjects[predictedIndex].boundingBox;
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
				printf("\t\tBest overlap: %f\n", bestOverlap);
			}
		}

		if (bestOverlap > 0) {
			if (!found[bestObject]) {
				current.truePositive++;
				found[bestObject] = true;
			} else {
				current.falsePositive++;
			}
		} else {
			current.falsePositive++;
		}
	}

	current.recall = double(current.truePositive) / double(current.numberOfPositives);
	current.precision = double(current.truePositive) / double(current.truePositive) + double(current.falsePositive);
	evaluation.evaluations.push_back(current);
	evaluation.total.falsePositive += current.falsePositive;
	evaluation.total.truePositive += current.truePositive;
	evaluation.total.numberOfPositives += current.numberOfPositives;
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
	int i;
	for (i = 0; i < evaluations.size(); i++) {
		evaluations[i].total.falsePositive = 0;
		evaluations[i].total.truePositive = 0;
		evaluations[i].total.numberOfPositives = 0;
		evaluations[i].total.precision = 0;
		evaluations[i].total.recall = 0;
	}
	vector<string> files;
	utils::recursivelySearchDirectoryForFiles(config.imageDirectory, config.fileExtension, &files);

	Mat image;
	string filename;
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
			printf("\tDetector %s\n", detectors[i]->name().c_str());
			TickMeter tm;
			tm.start();
			vector<DetectedObject> predictedObjects = detectors[i]->detectObjects(image);
			tm.stop();
			printf("\t\tDetection time = %f sec\n", tm.getTimeSec());
			evaluations[i].totalDetectionTime += tm.getTimeSec();
			evaluatePredictions(evaluations[i], annotatedImage, predictedObjects);
		}
	}

	for (i = 0; i < evaluations.size(); i++) {
		evaluations[i].total.recall = double(evaluations[i].total.truePositive) / double(evaluations[i].total.numberOfPositives);
		evaluations[i].total.precision = double(evaluations[i].total.truePositive) / double(evaluations[i].total.truePositive)
				+ double(evaluations[i].total.falsePositive);
	}

	for (i = 0; i < evaluations.size(); i++) {
		printf("Evaluation %d: (%s)\n", i + 1, detectors[i]->name().c_str());
		printf("\tOverall Recall:%f%%\n", evaluations[i].total.recall * 100.0);
		printf("\tOverall Precision:%f%%\n", evaluations[i].total.precision * 100.0);
		printf("\tNumber true positive:%d\n", evaluations[i].total.truePositive);
		printf("\tNumber false positive:%d\n", evaluations[i].total.falsePositive);
		printf("\tTotal positive:%d\n", evaluations[i].total.numberOfPositives);
		printf("\tTotal detection time = %f sec\n", evaluations[i].totalDetectionTime);
	}
	return EXIT_SUCCESS;
}
