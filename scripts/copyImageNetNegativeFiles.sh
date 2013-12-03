#!/bin/bash
filename=/sb/data/person/ImageNet/train/train_neg_124.txt
outputImageDirectory=/sb/data/person/ImageNet/train/neg-final/
inputImageDirectory=/sb/data/person/ImageNet/train/neg
cd $inputImageDirectory
while read line
do
    directory=`echo $line| cut -d'_' -f 1`
	tarPath=$inputImageDirectory/$directory.tar
	if [ -f $tarPath ];
	then
   		tar xvf $tarPath
   		rm -rf $tarPath
	else
   		echo "File $tarPath does not exist."
	fi
	untarredPath=$inputImageDirectory/$directory
	if [ -d $untarredPath ];
	then
		untarredFile=$untarredPath/$line.JPEG
   		cp $untarredFile $outputImageDirectory
	else
   		echo "Directory $untarredPath does not exist."
	fi
done < $filename