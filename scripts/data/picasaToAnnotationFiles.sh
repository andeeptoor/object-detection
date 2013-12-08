#!/bin/bash
filenamesFile=/sb/data/faces/picasa/faces.csv
outputImagesDirectory=/sb/data/faces/picasa/images
outputAnnotationsDirectory=/sb/data/faces/picasa/annotations

COUNTER=10000
while read fileLine
do
	while IFS=';' read person prefix filename originalImagePath transformedImagePath imageWidth imageHeight faceX1 faceY1 faceWidth faceHeight;
	do
		pathfileName=$(basename "$originalImagePath")
		extension="${pathfileName##*.}"
		if [ "$extension" = "JPG" ]
		then
			if [[ $originalImagePath == /Memories* ]]
			then
				if [ "$imageWidth" -lt "2000" ]
				then
					#cp "$originalImagePath" $outputImagesDirectory/img$COUNTER.JPG
					outputFile=$outputAnnotationsDirectory/img$COUNTER.txt
					echo $imageWidth $imageHeight $faceX1 $faceY1 $faceWidth $faceHeight > $outputFile
					let COUNTER=COUNTER+1 
				fi
			fi
		fi
	done <<< $fileLine
done < $filenamesFile