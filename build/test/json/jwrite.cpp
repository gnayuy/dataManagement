// test write meta info to json file
// Yang Yu (gnayuy@gmail.com)

// g++ -std=c++11 -o jwrite jwrite.cpp -L/usr/local/lib -lcpprest -lboost_system -lboost_chrono -lboost_thread -lboost_random -lboost_regex -lssl -lcrypto

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
    json::value j;
    
    j[U("name")] = json::value(mi.name);
    j[U("unit")] = json::value(mi.unit);
    
    // {{"vx", mi.voxelsize_x}, {"vy", mi.voxelsize_y}, {"vz", mi.voxelsize_z}}
    utility::stringstream_t ssVoxelSize;
    ssVoxelSize << U("{{\\\"vx\\\",") << mi.voxelsize_x << U("}, {\\\"vy\\\",") << mi.voxelsize_y << U("}, {\\\"vz\\\",") << mi.voxelsize_z << U("}}");

    cout<< ssVoxelSize.str() << endl;

    j[U("voxelsize")] = json::value::parse(ssVoxelSize);

/*
    // {{"ox", mi.origin_x}, {"oy", mi.origin_y}, {"oz", mi.origin_z}}
    utility::stringstream_t ssOrigin;
    ssOrigin << U("{{\"ox\",") << mi.origin_x << U("}, {\"oy\",") << mi.origin_y << U("}, {\"oz\",") << mi.origin_z << U("}}");

    j[U("origin")] = json::value::parse(ssOrigin);

    // {{"ox", mi.dim_x}, {"oy", mi.dim_y}, {"oz", mi.dim_z}}
    utility::stringstream_t ssDimension;
    ssDimension << U("{{\"dx\",") << mi.dim_x << U("}, {\"dy\",") << mi.dim_y << U("}, {\"dz\",") << mi.dim_z << U("}}");

    j[U("dimension")] = json::value::parse(ssDimension);

    // {{"ox", mi.size_x}, {"oy", mi.size_y}, {"oz", mi.size_z}}
    utility::stringstream_t ssSize;
    ssSize << U("{{\"sx\",") << mi.size_x << U("}, {\"sy\",") << mi.size_y << U("}, {\"sz\",") << mi.size_z << U("}}");

    j[U("size")] = json::value::parse(ssSize);
    
    j[U("bits")] = json::value(mi.bits_per_voxel);
    
    j[U("channels")] = json::value(mi.number_of_channels);
   
*/

    //
    cout.precision(numeric_limits<double>::digits10 + 1);
    std::cout<<j.serialize()<<std::endl;
    
    //
    return 0;
}
