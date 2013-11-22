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

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: <configFile>");
		return EXIT_FAILURE;
	}

	Config config = readConfigFile(argv[1]);
	PascalAnnotationFileParser parser;

	vector<string> files;
	utils::recursivelySearchDirectoryForFiles(config.imageDirectory, config.imageFileExtension, &files);

	string name =  utils::append("/", utils::append(utils::getFileWithoutParentDirectory(config.imageDirectory), ".txt"));
	const string outputFileName = utils::append(utils::getParentDirectory(config.imageDirectory), name);
	ofstream outputFile(outputFileName.c_str());
	if (!outputFile.is_open()) {
		printf("Cannot open data file [%s]\n", outputFileName.c_str());
		return EXIT_FAILURE;
	}

	Mat image;
	string filename;
	int i;
	for (int f = 0; f < files.size(); f++) {
		showProgress(f);
		filename = files[f];
		string annotationFile = utils::convertToParentDirectory(filename, config.annotationsDirectory);
		annotationFile = utils::convertToFileExtension(annotationFile, config.annotationsFileExtension);
		AnnotatedImage annotatedImage = parser.parseAnnotationFile(annotationFile);

		outputFile << utils::getParentDirectory(filename) << "/" << utils::getFileWithoutParentDirectory(filename);
		outputFile << "  " << annotatedImage.objects.size();
		for (i = 0; i < annotatedImage.objects.size(); i++) {
			outputFile << "  ";
			outputFile << annotatedImage.objects[i].boundingBox.x << " ";
			outputFile << annotatedImage.objects[i].boundingBox.y << " ";
			outputFile << annotatedImage.objects[i].boundingBox.width << " ";
			outputFile << annotatedImage.objects[i].boundingBox.height;
		}
		outputFile << "\n";
	}
	endProgress();
	outputFile.close();
	return EXIT_SUCCESS;
}
