// test write tiles to json file
// Yang Yu (gnayuy@gmail.com)

// g++ -std=c++11 -o jwritefiles jwritefiles.cpp -lboost_filesystem -lboost_system


#include "json.hpp"
using nlohmann::json;

#include "metainfo.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

//
int main(int argc, char* argv[])
{
    //
    json j;
    
    //
    if(argc<3)
    {
        std::cout<<"usage: ./jfilelist filelist.txt nameofspecimen"<<std::endl;
        return -1;
    }
    
    //
    std::ifstream input(argv[1]);
    string name(argv[2]);
    
    j["name"] = name;
    
    json::array_t ja;
    json::object_t jo;
    
    //
    string line;
    int i=0,n=0;
    if (input.is_open())
    {
        while( getline (input,line) )
        {
            if(strstr(line.c_str(),"#"))
            {
                cout << " skip comments" << endl;
            }
            else
            {
                path p(line);
                
                string parent_path = p.parent_path().string();
                
                string octree_path = parent_path.substr(parent_path.find(name) + name.length() + 1);
                
                std::stringstream ss;
                ss << std::setw( 9 ) << std::setfill( '0' ) << (n++)/2;
                string id = ss.str();
                
                if((i++)%2==0)
                {
                    jo["uuid"] = id;
                    jo["octreepath"] = octree_path;
                    jo["ch1"] = line;
                }
                else
                {
                    jo["ch2"] = line;
                    
                    //std::cout<< i << " "<< std::setw(4) << jo <<std::endl;
                    
                    ja.push_back(jo);
                }
                
                
            }
        }
        input.close();
    }
    else
    {
        cout << "Unable to open file"<<argv[1]<<endl;
    }
    
    //
    //std::cout<< std::setw(4) << ja <<std::endl;
    
    //
    j["tile"] = ja;
    std::cout<<j.dump(4)<<std::endl;
    
    //
    return 0;
}
