#!/bin/bash
annotationFiles=/sb/data/faces/FDDB/annotationFiles.txt
outputAnnotationsDirectory=/sb/data/faces/FDDB/images

while read fileLine
do
	echo $fileLine
	exec 5< $fileLine
	
	while read filePath <&5 ; do
		read numberOfLines <&5
		 COUNTER=0
		echo "$filePath $numberOfLines"
		fileWithoutExtension=${filePath%.*}
		outputFile=$outputAnnotationsDirectory/$fileWithoutExtension.txt
        echo $outputFile
        touch $outputFile
         while [  $COUNTER -lt $numberOfLines ]; do
             echo The counter is $COUNTER
             let COUNTER=COUNTER+1
             read line <&5
             echo $line >> $outputFile
         done
	done
	
	# Close file handle 5
	exec 5<&-
done < $annotationFiles