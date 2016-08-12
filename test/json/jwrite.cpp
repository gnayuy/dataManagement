// test write meta info to json file
// Yang Yu (gnayuy@gmail.com)

// g++ -std=c++11 -o jwrite jwrite.cpp


#include "json.hpp"
using nlohmann::json;

#include "metainfo.h"


int main(int argc, char* argv[])
{
    //
    MetaInfo mi;
    
    mi.name = "2015-07-11-johan";
    mi.unit = "micrometer";
    
    mi.voxelsize_x = 0.25002763671875;
    mi.voxelsize_y = 0.25155907495471014;
    mi.voxelsize_z = 1.0010404411764706;
    
    mi.origin_x = 72739.042;
    mi.origin_y = 41644.696;
    mi.origin_z = 14966.719;
    
    mi.dim_x = 640;
    mi.dim_y = 552;
    mi.dim_z = 204;
    
    mi.size_x = 16001.76875;
    mi.size_y = 16099.780797101448;
    mi.size_z = 64066.58823529412;
    
    mi.bits_per_voxel = 16;
    mi.number_of_channels = 2;
    
    //
    json j;
    
    j["name"] = mi.name;
    j["unit"] = mi.unit;
    
    j["voxelsize"] = {{"vx", mi.voxelsize_x}, {"vy", mi.voxelsize_y}, {"vz", mi.voxelsize_z}};
    
    j["origin"] = {{"ox", mi.origin_x}, {"oy", mi.origin_y}, {"oz", mi.origin_z}};
    
    j["dimension"] = {{"dx", mi.dim_x}, {"dy", mi.dim_y}, {"dz", mi.dim_z}};
    
    j["size"] = {{"sx", mi.size_x}, {"sy", mi.size_y}, {"sz", mi.size_z}};
    
    j["bits"] = mi.bits_per_voxel;
    
    j["channels"] = mi.number_of_channels;
    
    //
    cout.precision(numeric_limits<double>::digits10 + 1);
    std::cout<<j.dump(4)<<std::endl;
    
    //
    return 0;
}
