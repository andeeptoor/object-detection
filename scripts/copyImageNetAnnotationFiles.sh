#!/bin/bash
filename=/sb/data/person/ImageNet/train/train_pos_124.txt
originalAnnotationDirectory=/sb/data/person/ImageNet/train/ILSVRC2013_DET_bbox_train
outputAnnotationDirectory=/sb/data/person/ImageNet/train/annotations/
imageDirectory=/sb/data/person/ImageNet/train/pos
cd $imageDirectory
while read line
do
    directory=`echo $line| cut -d'_' -f 1`
	filePath=$originalAnnotationDirectory/$directory/$line.xml
	cp $filePath $outputAnnotationDirectory
	tarPath=$imageDirectory/$directory.tar
	if [ -f $tarPath ];
	then
   		tar xvf $tarPath
   		rm -rf $tarPath
	else
   		echo "File $tarPath does not exist."
	fi
	untarredPath=$imageDirectory/$directory
	if [ -d $untarredPath ];
	then
   		cp $untarredPath/* .
   		rm -rf $untarredPath
	else
   		echo "Directory $untarredPath does not exist."
	fi
done < $filename