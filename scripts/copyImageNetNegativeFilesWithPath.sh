#!/bin/bash
filename=/sb/data/person/ImageNet/train/train_pos_124.txt
outputImageDirectory=/sb/data/person/ImageNet/train/pos-final/
inputImageDirectory=/sb/data/person/ImageNet/train/pos
cd $inputImageDirectory
while read line
do
	untarredFile=$inputImageDirectory/$line.JPEG
  	if [ -f $untarredFile ];
	then
   		cp $untarredFile $outputImageDirectory
	else
   		echo "File $untarredFile does not exist."
	fi
done < $filename