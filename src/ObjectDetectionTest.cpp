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
	int i,p;
	for (i = 0; i < evaluations.size(); i++) {
		evaluations[i].total.falsePositive = 0;
		evaluations[i].total.truePositive = 0;
		evaluations[i].total.numberOfPositives = 0;
		evaluations[i].total.precision = 0;
		evaluations[i].total.recall = 0;
	}
	vector<string> files;
	if (config.imageDirectoryFilterFile.length() > 0) {
		readFilesFromFilterFile(config.imageDirectory, config.imageFileExtension, &files, config.imageDirectoryFilterFile);
	} else {
		utils::recursivelySearchDirectoryForFiles(config.imageDirectory, config.imageFileExtension, &files);
	}

	Mat image;
	string filename;
	for (int f = 0; f < files.size(); f++) {
		filename = files[f];

		string annotationFile = utils::findAndReplace(config.imageDirectory, config.annotationsDirectory, filename);
		annotationFile = utils::convertToFileExtension(annotationFile, config.annotationsFileExtension);
		AnnotatedImage annotatedImage = parser.parseAnnotationFile(annotationFile, config.annotationsFileFormat);

		if (annotatedImage.numberOfColors == 1) {
			image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
		} else {
			image = imread(filename, CV_LOAD_IMAGE_COLOR);
		}

		if (annotatedImage.imageWidth > 0 && annotatedImage.imageHeight > 0) {
			resize(image, image, Size(annotatedImage.imageWidth, annotatedImage.imageHeight));
		}
		printf("File[%d/%lu]: %s\n", f + 1, files.size(), filename.c_str());
		if (!image.data) {
			continue;
		}

		if (config.detectionOutputImages) {
			for (i = 0; i < annotatedImage.objects.size(); i++) {
				rectangle(image, annotatedImage.objects[i].boundingBox, cv::Scalar(255, 255, 255), 3);
			}
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

			if (config.detectionOutputImages) {
				for (p = 0; p < predictedObjects.size(); p++) {
					rectangle(image, predictedObjects[p].boundingBox, cv::Scalar(i*20, i*20, i*20), 3);
				}
			}
		}
		if (config.detectionOutputImages) {
			string taggedImageFile = utils::findAndReplace(config.imageDirectory, config.detectionOutputImagesDirectory, filename);
			printf("Outputting image to [%s]\n", taggedImageFile.c_str());
			imwrite(taggedImageFile, image);
		}
	}

	calculator.printEvaluations(evaluations, detectors);
	return EXIT_SUCCESS;
}
