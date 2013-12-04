#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fstream>
#include <set>

#include "common.h"
#include "math.h"
#include "AnnotationFileParser.h"
#include "ObjectDetector.h"

using namespace cv;
using namespace std;

void readFilesFromFilterFile(const string directory, const string fileExtension, vector<string> *files, const string filterFila) {
	ifstream inputFile(filterFila.c_str());
	if (!inputFile) {
		printf("Cannot open file: [%s]\n", filterFila.c_str());
		exit(EXIT_FAILURE);
	}

	string line, file;
	while (getline(inputFile, line)) {
		istringstream iss(line);
		iss >> file;
		stringstream stream1;
		stream1 << directory << "/" << file << "." << fileExtension;
		files->push_back(stream1.str());
//		printf("Found file: [%s]\n", stream1.str().c_str());
	}
	inputFile.close();
}

bool exclude(const string objectName) {
	return false;
//	return !utils::equals(objectName, "person");
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: <configFile>");
		return EXIT_FAILURE;
	}

	Config config = readConfigFile(argv[1]);
	AnnotationFileParser parser;

	vector<string> files;

	if (config.imageDirectoryFilterFile.length() > 0) {
		readFilesFromFilterFile(config.imageDirectory, config.imageFileExtension, &files, config.imageDirectoryFilterFile);
	} else {
		utils::recursivelySearchDirectoryForFiles(config.imageDirectory, config.imageFileExtension, &files);
	}

	string name = utils::append("/", utils::append(utils::getFileWithoutParentDirectory(config.imageDirectory), ".txt"));
	const string outputFileName = utils::append(utils::getParentDirectory(config.imageDirectory), name);
	ofstream outputFile(outputFileName.c_str());
	if (!outputFile.is_open()) {
		printf("Cannot open data file [%s]\n", outputFileName.c_str());
		return EXIT_FAILURE;
	}

	Mat image;
	string filename;
	int i, objectsCount, totalCount=0;
	set<string> uniqueNames;
	bool addObject;
	for (int f = 0; f < files.size(); f++) {
		showProgress(f);
		filename = files[f];
		string annotationFile = utils::convertToParentDirectory(filename, config.annotationsDirectory);
		annotationFile = utils::convertToFileExtension(annotationFile, config.annotationsFileExtension);
		AnnotatedImage annotatedImage = parser.parseAnnotationFile(annotationFile);

		addObject = false;
		objectsCount = 0;
		stringstream stream1;

		for (i = 0; i < annotatedImage.objects.size(); i++) {
			if (!exclude(annotatedImage.objects[i].label)) {
				stream1 << "  ";
				stream1 << annotatedImage.objects[i].boundingBox.x << " ";
				stream1 << annotatedImage.objects[i].boundingBox.y << " ";
				stream1 << annotatedImage.objects[i].boundingBox.width << " ";
				stream1 << annotatedImage.objects[i].boundingBox.height;
				uniqueNames.insert(annotatedImage.objects[i].label);
				addObject = true;
				objectsCount++;
				totalCount++;
			}
		}
		if (addObject) {
			outputFile << utils::getParentDirectory(filename) << "/" << utils::getFileWithoutParentDirectory(filename);
			outputFile << "  " << objectsCount;
			outputFile << stream1.str();
			outputFile << "\n";
		}
	}

	endProgress();
	outputFile.close();
	for (set<string>::iterator it = uniqueNames.begin(); it != uniqueNames.end(); ++it) {
		printf("name: [%s]\n", (*it).c_str());
	}

	printf("%lu files\n", files.size());
	printf("%d objects\n", totalCount);
	return EXIT_SUCCESS;
}
