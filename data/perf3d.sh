# test 3D retrieval

SX=$1
SIZE=$2

time ./src/datamanagement -server http://tem-dvid:7400 -uuid 0dd -name grayscale -methods true -x 16128 -y 17664 -z 5100 -sx $SX -sy $SIZE -sz $SIZE -sc 2 -vsx 0.25 -vsy 0.25 -vsz 1.00 -output ./test3D.tif
