#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fstream>

#include "common.h"
#include "PascalAnnotationFileParser.h"

using namespace cv;
using namespace std;

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
	int i, j;
	vector<Rect> found, found_filtered;
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

		found.clear();
		found_filtered.clear();

		fflush(stdout);
		double t = (double) getTickCount();
		// run the detector with default parameters. to get a higher hit-rate
		// (and more false alarms, respectively), decrease the hitThreshold and
		// groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
		hog.detectMultiScale(image, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);
		t = (double) getTickCount() - t;
		printf("tdetection time = %gms\n", t * 1000. / cv::getTickFrequency());
		for (i = 0; i < found.size(); i++) {
			Rect r = found[i];
			for (j = 0; j < found.size(); j++) {
				if (j != i && (r & found[j]) == r) {
					break;
				}
			}
			if (j == found.size()) {
				found_filtered.push_back(r);
			}
		}
		for (i = 0; i < found_filtered.size(); i++) {
			Rect r = found_filtered[i];
			// the HOG detector returns slightly larger rectangles than the real objects.
			// so we slightly shrink the rectangles to get a nicer output.
//			r.x += cvRound(r.width * 0.1);
//			r.width = cvRound(r.width * 0.8);
//			r.y += cvRound(r.height * 0.07);
//			r.height = cvRound(r.height * 0.8);
			rectangle(image, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
		}

		for (i = 0;  i< annotatedImage.objects.size(); i++) {
			Point p1,p2;
			p1.x = annotatedImage.objects[i].boundingBoxMin.first;
			p1.y = annotatedImage.objects[i].boundingBoxMin.second;
			p2.x = annotatedImage.objects[i].boundingBoxMax.first;
			p2.y = annotatedImage.objects[i].boundingBoxMax.second;
			rectangle(image, p1, p2, cv::Scalar(255, 0, 0), 3);
		}

		imshow("people detector", image);

		int c = waitKey(0) & 255;
		if (c == 'q' || c == 'Q' || !f) {
			break;
		}
	}
	return EXIT_SUCCESS;
}
