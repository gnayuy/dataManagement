# find a specified resolution level 3D tiles' paths
# Yang Yu (gnayuy@gmail.com)

# usage:
# sh octree2list.sh <data_dir> <resolution_level> <file_list>
# e.g. sh octree2list.sh $PWD/data 7 ./branch3tiles.txt

# input
ROOTDIR=$1
LEVEL=$2
FILELIST=$3

#
find $ROOTDIR -maxdepth $LEVEL -mindepth $LEVEL > $FILELIST


