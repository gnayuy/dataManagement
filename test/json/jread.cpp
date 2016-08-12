// test read meta info from json file
// Yang Yu (gnayuy@gmail.com)

//g++ -std=c++11 -o jread jread.cpp

#include "json.hpp"
using nlohmann::json;

#include "metainfo.h"

int main(int argc, char* argv[])
{
    //
    if(argc<1)
    {
        std::cout<<"usage: jread metainfo.json"<<std::endl;
        return -1;
    }
    
    std::ifstream f(argv[1]);
    
    //
    json j;
    
    j << f;
    
    //
    MetaInfo mi;
    
    mi.name = j["name"];
    
    cout<<mi.name<<endl;
    
    mi.unit = j["unit"];
    
    cout<<mi.unit<<endl;
    
    mi.voxelsize_x = j["voxelsize"]["x"];
    mi.voxelsize_y = j["voxelsize"]["y"];
    mi.voxelsize_z = j["voxelsize"]["z"];
    
    cout<<"voxelsize: "<<mi.voxelsize_x<<" "<<mi.voxelsize_y<<" "<<mi.voxelsize_z<<endl;
    
    mi.dimx = j["dimension"]["x"];
    mi.dimy = j["dimension"]["y"];
    mi.dimz = j["dimension"]["z"];
    
    cout<<"dimension: "<<mi.dimx<<" "<<mi.dimy<<" "<<mi.dimz<<endl;
    
    
    //
    return 0;
}