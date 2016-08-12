

#include <deque>
#include <queue>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <functional>
#include <vector>
#include <cmath>
#include <ctime>
#include <limits>
#include <complex>
#include <float.h>
using namespace std;

//
class MetaInfo
{
public:
    string name; // name of the specimen
    string unit; // "micrometer"/"nanometer"
    double voxelsize_x, voxelsize_y, voxelsize_z;
    double origin_x, origin_y, origin_z;
    double size_x, size_y, size_z;
    unsigned int dim_x, dim_y, dim_z;
    unsigned int bits_per_voxel;
    unsigned int number_of_channels;
};

class FileList
{
public:
    string octree_path;
    string channel1, channel2, channel3;
};