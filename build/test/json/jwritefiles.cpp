// test write tiles to json file
// Yang Yu (gnayuy@gmail.com)

// g++ -std=c++11 -o jwritefiles jwritefiles.cpp -L/usr/local/lib -lcpprest -lboost_filesystem -lboost_system -lboost_chrono -lboost_thread -lboost_random -lboost_regex -lssl -lcrypto

#include "metainfo.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

//
void printJSONValue(json::value v, string indent, bool isArray)
{
    string indented=indent;

    //
    if(isArray)
    {
        cout << indent << "[" <<endl;
    }
    else
    {
        cout << indent << "{" <<endl;
    }

    //
    if (!v.is_null())
    {
        indented.append("    ");

        //
        if(isArray)
        {
            for (auto &value : v.as_array())
            {
                if (value.is_object() || value.is_array())
                {
                    printJSONValue(value, indented, value.is_array());
                }
                else
                {
                    //
                    if(value.is_double())
                    {
                        cout.precision(numeric_limits<double>::digits10 + 1);
                        cout << value.as_double();
                    }
                    else if(v.is_integer())
                    {
                         cout << value.as_integer();
                    }
                    else
                    {
                        ucout << value.serialize();
                    }
                    cout << endl;
                }
            }
        }
        else
        {
            for (auto iter = std::begin(v.as_object()); iter != std::end(v.as_object()); ++iter)
            {
                //
                auto key = iter->first;
                auto& value = iter->second;

                if (value.is_object() || value.is_array())
                {
                    ucout << indented << key << U(" : ") << endl;
                    printJSONValue(value, indented, value.is_array());
                }
                else
                {
                    //
                    ucout << indented << key << U(" : ");

                    if(value.is_double())
                    {
                        cout.precision(numeric_limits<double>::digits10 + 1);
                        cout << value.as_double();
                    }
                    else if(v.is_integer())
                    {
                         cout << value.as_integer();
                    }
                    else
                    {
                        ucout << value.serialize();
                    }
                    cout << endl;
                }
            }
        }


    }

    //
    if(isArray)
    {
        cout << indent << "]" <<endl;
    }
    else
    {
        cout << indent << "}" <<endl;
    }
}

//
int main(int argc, char* argv[])
{
    //
    json::value j;
    
    //
    if(argc<3)
    {
        std::cout<<"usage: ./jfilelist filelist.txt nameofspecimen"<<std::endl;
        return -1;
    }
    
    //
    std::ifstream input(argv[1]);
    string name(argv[2]);
    
    j[U("name")] = json::value(name);
    
    json::value ja;
    json::value jo;
    
    //
    string line;
    int i=0,n=0,count;
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
                count = (n++)/2;
                ss << setw( 9 ) << setfill( '0' ) << count;
                string id = ss.str();
                
                if((i++)%2==0)
                {
                    jo[U("uuid")] = json::value(id);
                    jo[U("octreepath")] = json::value(octree_path);
                    jo[U("ch1")] = json::value(line);
                }
                else
                {
                    jo[U("ch2")] = json::value(line);
                    
                    //
                    ja[count] = jo;
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
    j[U("tile")] = ja;

    //
    //cout<<j.serialize()<<endl;

    //
    printJSONValue(j, "", j.is_array());
    
    //
    return 0;
}
