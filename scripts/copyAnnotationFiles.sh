#!/bin/bash
filename=/sb/data/person/ImageNet/ILSVRC2013_devkit/data/det_lists/train_pos_124.txt
originalAnnotationDirectory=/sb/data/person/ImageNet/train/ILSVRC2013_DET_bbox_train
outputAnnotationDirectory=/sb/data/person/ImageNet/train/annotations/
while read line
do
    directory=`echo $line| cut -d'_' -f 1`
	filePath=$originalAnnotationDirectory/$directory/$line.xml
	cp $filePath $outputAnnotationDirectory
done < $filename