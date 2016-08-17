// test read tiles from json file
// Yang Yu (gnayuy@gmail.com)

//g++ -std=c++11 -o jreadfiles jreadfiles.cpp -L/usr/local/lib -lcpprest -lboost_system -lboost_chrono -lboost_thread -lboost_random -lboost_regex -lssl -lcrypto

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
    json::value j = json::value::parse(f);
    
    //
    tileList files;
    json::value ja = j[U("tile")];
    
    for(auto val : ja.as_array())
    {
        Tile fl;
        fl.uuid = val[U("uuid")].as_string();
        fl.ch1 = val[U("ch1")].as_string();
        fl.ch2 = val[U("ch2")].as_string();
        fl.octreepath = val[U("octreepath")].as_string();
        
        files.push_back(fl);
    }
    
    // test
    cout<<files.size()<<endl;
    
    Tile fl = files[0];
    
    cout<<fl.octreepath<<" "<<fl.ch1<<endl;
    
    //
    return 0;
}
