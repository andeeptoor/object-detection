 directory=$1
 fileExtension=$2
 
 find $directory -name "*.$fileExtension" > negatives.dat