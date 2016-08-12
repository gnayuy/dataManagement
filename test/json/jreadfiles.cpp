// test read tiles from json file
// Yang Yu (gnayuy@gmail.com)

//g++ -std=c++11 -o jreadfiles jreadfiles.cpp

#include "json.hpp"
using nlohmann::json;

#include "metainfo.h"

int main(int argc, char* argv[])
{
    //
    if(argc<1)
    {
        std::cout<<"usage: jreadfiles tilelist.json"<<std::endl;
        return -1;
    }
    
    std::ifstream f(argv[1]);
    
    //
    json j;
    
    j << f;
    
    //
    tileList files;
    json ja = j["tile"];
    
    for(json::iterator it=ja.begin(); it<ja.end(); it++)
    {
        Tile fl;
        fl.uuid = (*it)["uuid"];
        fl.ch1 = (*it)["ch1"];
        fl.ch2 = (*it)["ch2"];
        fl.octreepath = (*it)["octreepath"];
        
        files.push_back(fl);
    }
    
    // test
    cout<<files.size()<<endl;
    
    Tile fl = files[0];
    
    cout<<fl.octreepath<<" "<<fl.ch1<<endl;
    
    
    
    //
    return 0;
}