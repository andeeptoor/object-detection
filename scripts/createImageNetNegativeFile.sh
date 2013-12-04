#!/bin/bash
filename=/sb/data/person/ImageNet/train/train_neg_124.txt
while read line
do
	echo neg/$line.jpg
done < $filename