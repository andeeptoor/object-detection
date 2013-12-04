/*
 * EvaluationCalculator.h
 *
 *  Created on: Dec 4, 2013
 *      Author: atoor
 */

#ifndef EVALUATIONCALCULATOR_H_
#define EVALUATIONCALCULATOR_H_

#include "common.h"

class EvaluationCalculator {
public:
	EvaluationCalculator();
	virtual ~EvaluationCalculator();
	void evaluatePredictions(Evaluation &evaluation, const AnnotatedImage annotatedImage, const vector<DetectedObject> predictedObjects);
	double calculateAveragePrecision(Evaluation eval);
	void printEvaluations(vector<Evaluation>& evaluations, vector<ObjectDetector*>& detectors);
};

#endif /* EVALUATIONCALCULATOR_H_ */
