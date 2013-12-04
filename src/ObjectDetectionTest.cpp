#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fstream>

#include "common.h"
#include "math.h"
#include "AnnotationFileParser.h"
#include "EvaluationCalculator.h"
#include "ObjectDetector.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: <configFile>");
		return EXIT_FAILURE;
	}

	Config config = readConfigFile(argv[1]);
	AnnotationFileParser parser;
	EvaluationCalculator calculator;
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
	utils::recursivelySearchDirectoryForFiles(config.imageDirectory, config.imageFileExtension, &files);

	Mat image;
	string filename;
	for (int f = 0; f < files.size(); f++) {
		filename = files[f];

		string annotationFile = utils::convertToParentDirectory(filename, config.annotationsDirectory);
		annotationFile = utils::convertToFileExtension(annotationFile, "txt");
		AnnotatedImage annotatedImage = parser.parseAnnotationFile(annotationFile, config.annotationsFileFormat)[0];

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
			calculator.evaluatePredictions(evaluations[i], annotatedImage, predictedObjects);
		}
	}

	calculator.printEvaluations(i, evaluations, detectors);
	return EXIT_SUCCESS;
}
