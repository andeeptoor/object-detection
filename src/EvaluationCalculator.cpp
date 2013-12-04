/*
 * EvaluationCalculator.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: atoor
 */

#include "EvaluationCalculator.h"

EvaluationCalculator::EvaluationCalculator() {
	// TODO Auto-generated constructor stub

}

EvaluationCalculator::~EvaluationCalculator() {
	// TODO Auto-generated destructor stub
}


void EvaluationCalculator::printEvaluations(vector<Evaluation>& evaluations, vector<ObjectDetector*>& detectors) {
	int i;
	for (i = 0; i < evaluations.size(); i++) {
		if (evaluations[i].total.numberOfPositives > 0) {
			evaluations[i].total.recall = double(evaluations[i].total.truePositive) / double(evaluations[i].total.numberOfPositives);
		}
		if (evaluations[i].total.truePositive > 0 || evaluations[i].total.falsePositive > 0) {
			evaluations[i].total.precision = double(evaluations[i].total.truePositive)
					/ (double(evaluations[i].total.truePositive) + double(evaluations[i].total.falsePositive));
		}
		evaluations[i].averagePrecision = calculateAveragePrecision(evaluations[i]);
	}
	for (i = 0; i < evaluations.size(); i++) {
		printf("Evaluation %d: (%s)\n", i + 1, detectors[i]->name().c_str());
		printf("\tOverall Recall:%f%%\n", evaluations[i].total.recall * 100.0);
		printf("\tOverall Precision:%f%%\n", evaluations[i].total.precision * 100.0);
		printf("\tAverage Precision:%f%%\n", evaluations[i].averagePrecision * 100.0);
		printf("\tNumber true positive:%d\n", evaluations[i].total.truePositive);
		printf("\tNumber false positive:%d\n", evaluations[i].total.falsePositive);
		printf("\tTotal positive:%d\n", evaluations[i].total.numberOfPositives);
		printf("\tTotal detection time = %f sec\n", evaluations[i].totalDetectionTime);
	}
}

void EvaluationCalculator::evaluatePredictions(Evaluation &evaluation, const AnnotatedImage annotatedImage, const vector<DetectedObject> predictedObjects) {
	Rect actual, predicted, intersection, unionRect;
	double bestOverlap, currentOverlap;
	vector<bool> found(annotatedImage.objects.size(), false);
	int bestObject;
	EvaluationImage current;
	current.numberOfPositives = 0;
	current.falsePositive = 0;
	current.truePositive = 0;
	current.precision = 0;
	current.recall = 0;
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

	if (current.numberOfPositives > 0) {
		current.recall = double(current.truePositive) / double(current.numberOfPositives);
	}
	if (current.truePositive > 0 || current.falsePositive > 0) {
		current.precision = double(current.truePositive) / (double(current.truePositive) + double(current.falsePositive));
	}
	evaluation.evaluations.push_back(current);
	evaluation.total.falsePositive += current.falsePositive;
	evaluation.total.truePositive += current.truePositive;
	evaluation.total.numberOfPositives += current.numberOfPositives;
}

bool sortRecallAscending(const EvaluationImage& l, const EvaluationImage& r) {
	return l.recall < r.recall;
}

double EvaluationCalculator::calculateAveragePrecision(Evaluation eval) {
	vector<double> recall;
	vector<double> precision;
	int i;

	recall.push_back(0);
	precision.push_back(0);

	printf("\n");

	vector<EvaluationImage> evals = eval.evaluations;

	sort(evals.begin(), evals.end(), sortRecallAscending);

	for (i = 0; i < evals.size(); i++) {
		recall.push_back(evals[i].recall);
		precision.push_back(evals[i].precision);
//		printf("Sorted Precision[%d]=[%f]\n", i, precision[i]);
	}

	recall.push_back(1);
	precision.push_back(0);

	for (i = precision.size() - 2; i >= 0; i--) {
		precision[i] = max(precision[i], precision[i + 1]);
//			printf("Precision[%d]=[%f]\n", i, precision[i]);
	}

	vector<int> indices;

	double previous = recall[0];
	for (i = 1; i < recall.size(); i++) {
		if (recall[i] != previous) {
			indices.push_back(i + 1);
			previous = recall[i];
			printf("Recall: [%f]\n", recall[i]);
		}
	}

	double averagePrecision = 0;
	int index;
	for (i = 0; i < indices.size(); i++) {
		index = indices[i];
		printf("Index=[%d] Recall[index]=[%f] Recall[index-1]=[%f] Precision[index]=[%f]\n", index, recall[index], recall[index - 1], precision[index]);
		averagePrecision += ((recall[index] - recall[index - 1]) * precision[index]);
	}
	printf("\n");
	/*
	 * i=find(mrec(2:end)~=mrec(1:end-1))+1;
	 ap=sum((mrec(i)-mrec(i-1)).*mpre(i));
	 */

	return averagePrecision;
}

