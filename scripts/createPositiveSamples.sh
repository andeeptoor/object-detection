#!/bin/bash

inputFile=$1
outputFile=$2
numberOfSamples=$3
width=20
height=20

opencv_createsamples -info $inputFile -vec $outputFile -w $width -h $height -num $numberOfSamples