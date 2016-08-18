// test read meta info from json file
// Yang Yu (gnayuy@gmail.com)

// g++ -std=c++11 -o jread jread.cpp -L/usr/local/lib -lcpprest -lboost_system -lboost_chrono -lboost_thread -lboost_random -lboost_regex -lssl -lcrypto

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
    json::value j = json::value::parse(f);
    
    // test
    MetaInfo mi;
    
    mi.name = j[U("name")].as_string();
    
    cout<<mi.name<<endl;
    
    mi.unit = j[U("unit")].as_string();
    
    cout<<mi.unit<<endl;
    
    mi.voxelsize_x = j[U("voxelsize")][U("vx")].as_double();
    mi.voxelsize_y = j[U("voxelsize")][U("vy")].as_double();
    mi.voxelsize_z = j[U("voxelsize")][U("vz")].as_double();
    
    cout<<"voxelsize: "<<mi.voxelsize_x<<" "<<mi.voxelsize_y<<" "<<mi.voxelsize_z<<endl;
    
    mi.dim_x = j[U("dimension")][U("dx")].as_integer();
    mi.dim_y = j[U("dimension")][U("dy")].as_integer();
    mi.dim_z = j[U("dimension")][U("dz")].as_integer();
    
    cout<<"dimension: "<<mi.dim_x<<" "<<mi.dim_y<<" "<<mi.dim_z<<endl;
    
    //
    return 0;
}
