#!/bin/bash

positiveExamples=$1
negativeExamples=$2
featureType=$3
negativeNumber=28121
positiveNumber=2601
bufferSize=4048
width=19
height=19

opencv_traincascade -data . -vec $positiveExamples -bg $negativeExamples -numPos $positiveNumber -numNeg $negativeNumber -numStages 20 -precalcValBufSize $bufferSize -precalcIdxBufSize $bufferSize -stageType BOOST -featureType $featureType -h $height -w $width -minHitRate 0.999 -maxFalseAlarmRate 0.5