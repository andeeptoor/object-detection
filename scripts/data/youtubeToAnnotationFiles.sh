#!/bin/bash
annotationFiles=/sb/data/faces/youtube/annotationFiles.txt
outputAnnotationsDirectory=/sb/data/faces/youtube/images

while read fileLine
do
	echo $fileLine
	while read -r annotationLine
	do
		echo $annotationLine
		filePath=`echo $annotationLine| cut -d ',' -f 1`
		filePath=`echo "$filePath" | tr '\\' '/'`
		fileWithoutExtension=${filePath%.*}
		echo $outputAnnotationsDirectory/$fileWithoutExtension.txt
		everythingElse=`echo $annotationLine| cut -d ',' -f 2-`
		echo $everythingElse > $outputAnnotationsDirectory/$fileWithoutExtension.txt
	done < $fileLine
done < $annotationFiles