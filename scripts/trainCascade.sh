#!/bin/bash

positiveExamples=$1
negativeExamples=$2
featureType=$3
negativeNumber=1218
positiveNumber=614
bufferSize=2048
width=20
height=20

opencv_traincascade -data . -vec $positiveExamples -bg $negativeExamples -numPos $positiveNumber -numNeg $negativeNumber -numStages 20 -precalcValBufSize $bufferSize -precalcIdxBufSize $bufferSize -stageType BOOST -featureType $featureType -h $height -w $width -minHitRate 0.999 -maxFalseAlarmRate 0.5