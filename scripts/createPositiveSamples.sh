#!/bin/bash

inputFile=$1
outputFile=$2
numberOfSamples=614
width=20
height=20

opencv_createsamples -info $inputFile -vec $outputFile -w 20 -h 20 -num $numberOfSamples