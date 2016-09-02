
BUFFERLIST=$1
OUTPUT=$2

while IFS='' read -r n || [[ -n "$n" ]];
do

echo "time ./src/datamanagement -tiles ../data/tileList.json -server http://tem-dvid:7400 -uuid 0dd -name grayscale -branch 3 -buffer $n" >> $OUTPUT

done < $BUFFERLIST
