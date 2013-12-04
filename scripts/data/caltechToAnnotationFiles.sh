#!/bin/bash
annotationsFile=/sb/data/faces/caltech/ImageDataTranspose.csv
filenamesFile=/sb/data/faces/caltech/files.txt
outputAnnotationsDirectory=/sb/data/faces/caltech/annotations

exec 5< $annotationsFile
exec 6< $filenamesFile
while read l1 <&5 && read l2 <&6
do
	fileWithoutExtension=${l2%.*}
    echo "$l1" > $outputAnnotationsDirectory/$fileWithoutExtension.txt
done