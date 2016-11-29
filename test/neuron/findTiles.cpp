//

// g++ -std=c++11 -o findTiles findTiles.cpp -L/usr/local/lib -lcpprest -lboost_system -lboost_chrono -lboost_thread -lboost_random -lboost_regex -lssl -lcrypto

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include "../json/metainfo.h"

// global const
float OFFX=72022.286767;
float OFFY=15966.638635;
float OFFZ=32203.173689;

float OX=68097.320;
float OY=13754.192;
float OZ=27557.100;

float SX=15984.70639534884;
float SY=16031.19140625;
float SZ=64066.66666666666;

float VX=0.249761;
float VY=0.250487;
float VZ=1.00104;

float DX=688;
float DY=512;
float DZ=228;

// delete 1d pointer
template <class T>
void del1dp(T *&p)
{
    if(p) {delete []p; p=NULL;}
    return;
}

// new 1d pointer
template<class T, class Tidx>
void new1dp(T *&p, Tidx n)
{
    //
    del1dp<T>(p);
    
    //
    try
    {
        p = new T [n];
    }
    catch(...)
    {
        cout<<"Attempt to allocate memory failed!"<<endl;
        del1dp<T>(p);
        return;
    }
    return;
}

//
class Point
{
public:
    float x, y, z;
};

//
typedef vector<Point> PointCloud;

//
std::string trim(std::string const& source, char const* delims = " \t\r\n")
{
    std::string result(source);
    std::string::size_type index = result.find_last_not_of(delims);
    if(index != std::string::npos)
    {
        result.erase(++index);
    }
    
    index = result.find_first_not_of(delims);
    if(index != std::string::npos)
    {
        result.erase(0, index);
    }
    else
    {
        result.erase();
    }
    return result;
}

bool readSWC(const char* fn, PointCloud &pc)
{
    string filename = const_cast<char *>(fn);
    
    std::ifstream in;
    
    in.open (fn, std::ios::in | std::ios::binary);
    if ( in.fail() )
    {
        in.close();
        std::cerr<< "The file could not be opened for read access "<< std::endl << "Filename: " << fn <<endl;
    }
    
    std::ostringstream InData;
    
    //
    std::filebuf *pbuf;
    pbuf = in.rdbuf();
    
    // get file size using buffer's members
    int size = pbuf->pubseekoff (0, std::ios::end, std::ios::in);
    pbuf->pubseekpos (0, std::ios::in);
    
    // allocate memory to contain file data
    char *buffer = NULL;
    new1dp<char, size_t>(buffer, size + 1);
    
    // get file data
    pbuf->sgetn (buffer, size);
    buffer[size] = '\0';
    InData << buffer;
    
    del1dp<char>(buffer);
    std::string data = InData.str();
    in.close();
    
    // Read line by line
    std::string::size_type position = 0;
    
    // check for line end convention
    std::string line_end("\n");
    
    if ( data.find('\n') == std::string::npos )
    {
        if ( data.find('\r') == std::string::npos )
        {
            std::cerr << "No line ending character found, not a valid SWC file"<<endl;
        }
        line_end = "\r";
    }
    
    long count;
    while ( position != std::string::npos && position < data.size() )
    {
        // Find the next string
        std::string::size_type end = data.find (line_end, position);
        
        if( end == std::string::npos )
        {
            std::cerr << "Couldn't find end of line in " << data <<endl;
        }
        
        std::string line = trim ( data.substr (position, end - position) );
        position = end + 1;
        
        if ( line.length() == 0 )
        {
            continue;
        }
        if ( line[0] == '#' || std::string::npos == line.find_first_not_of (" \t") )
        {
            // Skip lines beginning with #, or blank lines
            continue;
        }
        
        // Push back
        std::istringstream parse (line);
        
        Point p;
        
        float tmp;
        
        parse >> tmp; // n
        parse >> tmp; // type
        parse >> p.x; // x
        parse >> p.y; // y
        parse >> p.z; // z
        
        pc.push_back(p);
    }
    
    //
    return true;
}

bool insideBB(string octreepath, float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
    //
    std::vector<int> vect;
    
    std::stringstream ss(octreepath);
    
    int i;
    
    while (ss >> i)
    {
        vect.push_back(i);
        
        if (ss.peek() == '/')
            ss.ignore();
    }
    
    //
    int ioff,joff,koff;
    
    for (i=0; i<vect.size(); i++)
    {
        if(i==0)
        {
            switch(vect.at(i))
            {
                case 1:
                    ioff = 0;
                    joff = 0;
                    koff = 0;
                    break;
                case 2:
                    ioff = 1;
                    joff = 0;
                    koff = 0;
                    break;
                case 3:
                    ioff = 0;
                    joff = 1;
                    koff = 0;
                    break;
                case 4:
                    ioff = 1;
                    joff = 1;
                    koff = 0;
                    break;
                case 5:
                    ioff = 0;
                    joff = 0;
                    koff = 1;
                    break;
                case 6:
                    ioff = 1;
                    joff = 0;
                    koff = 1;
                    break;
                case 7:
                    ioff = 0;
                    joff = 1;
                    koff = 1;
                    break;
                case 8:
                    ioff = 1;
                    joff = 1;
                    koff = 1;
                    break;
                default:
                    cout<<"Invalid octree node!"<<endl;
                    break;
            }
        }
        else
        {
            switch(vect.at(i))
            {
                case 1:
                    ioff *= 2;
                    joff *= 2;
                    koff *= 2;
                    break;
                case 2:
                    ioff = 2*ioff + 1;
                    joff *= 2;
                    koff *= 2;
                    break;
                case 3:
                    ioff *= 2;
                    joff = 2*joff + 1;
                    koff *= 2;
                    break;
                case 4:
                    ioff = 2*ioff + 1;
                    joff = 2*joff + 1;
                    koff *= 2;
                    break;
                case 5:
                    ioff *= 2;
                    joff *= 2;
                    koff = 2*koff + 1;
                    break;
                case 6:
                    ioff = 2*ioff + 1;
                    joff *= 2;
                    koff = 2*koff + 1;
                    break;
                case 7:
                    ioff *= 2;
                    joff = 2*joff + 1;
                    koff = 2*koff + 1;
                    break;
                case 8:
                    ioff = 2*ioff + 1;
                    joff = 2*joff + 1;
                    koff = 2*koff + 1;
                    break;
                default:
                    cout<<"Invalid octree node!"<<endl;
                    break;
            }
        }
    }
    
    //
    float bx=ioff*DX*VX;
    float ex=(ioff+1)*DX*VX;
    
    float by=joff*DY*VY;
    float ey=(joff+1)*DY*VY;
    
    float bz=koff*DZ*VZ;
    float ez=(koff+1)*DZ*VZ;
    
    //cout<<"test tile "<<bx<<" "<<ex<<" "<<by<<" "<<ey<<" "<<bz<<" "<<ez<<endl;
    
    //
    if( (bx < maxX && ex > minX) && (by < maxY && ey > minY) && (bz < maxZ && ez > minZ) )
    {
        return true;
    }else{
        return false;
    }
}

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
                    ucout << indented << U("\"") << key << U("\"") << U(" : ") << endl;
                    printJSONValue(value, indented, value.is_array());
                }
                else
                {
                    //
                    ucout << indented << U("\"") << key << U("\"") << U(" : ");
                    
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
                    if(iter+1 != std::end(v.as_object()))
                        cout << U(",") << endl;
                    else
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
        cout << indent << "}," <<endl;
    }
}

//
int main (int argc, char* argv[])
{
    //
    float offsetX = OFFX - OX;
    float offsetY = OFFY - OY;
    float offsetZ = OFFZ - OZ;
    
    cout<<"offset ... "<<offsetX<<" "<<offsetY<<" "<<offsetZ<<endl;
    
    //
    if(argc<3)
    {
        std::cout<<"need at least 2 inputs"<<endl;
        return -1;
    }
    
    // read neuron file
    const char* swcFile = (const char *) (argv[1]); // swc
    
    PointCloud pc;
    
    //
    if(readSWC(swcFile, pc) == false)
    {
        cout<<"fail to read swc"<<endl;
        return -1;
    }
    
    if(pc.size()<1)
    {
        cout<<"Empty neuron (.swc). \n";
        return -1;
    }
    
    float minX, minY, minZ, maxX, maxY, maxZ;
    
    //
    minX = maxX = pc[0].x + offsetX;
    minY = maxY = pc[0].y + offsetY;
    minZ = maxZ = pc[0].z + offsetZ;
    
    //
    for(int i=1; i<pc.size(); i++)
    {
        pc[i].x = pc[i].x + offsetX;
        pc[i].y = pc[i].y + offsetY;
        pc[i].z = pc[i].z + offsetZ;
        
        if(pc[i].x < minX)
            minX = pc[i].x;
        if(pc[i].x > maxX)
            maxX = pc[i].x;
        
        if(pc[i].y < minY)
            minY = pc[i].y;
        if(pc[i].y > maxY)
            maxY = pc[i].y;
        
        if(pc[i].z < minZ)
            minZ = pc[i].z;
        if(pc[i].z > maxZ)
            maxZ = pc[i].z;
    }
    
    minX -= DX*VX;
    minY -= DY*VY;
    minZ -= DZ*VZ;
    maxX += DX*VX;
    maxY += DY*VY;
    maxZ += DZ*VZ;
    
    cout<< "test ... min "<<minX<<" "<<minY<<" "<<minZ<<endl;
    cout<< "test ... max "<<maxX<<" "<<maxY<<" "<<maxZ<<endl;
    
    // read tiles
    std::ifstream f(argv[2]); // json
    
    //
    json::value j = json::value::parse(f);
    //
    json::value jout;
    jout[U("name")] = j[U("name")];
    
    //
    json::value ja = j[U("tile")];
    //
    json::value joutarray;
    
    //
    int count=0;
    for(auto val : ja.as_array())
    {
        string octreepath = val[U("octreepath")].as_string();
        
        if(insideBB(octreepath, minX, minY, minZ, maxX, maxY, maxZ))
        {
            joutarray[count] = val;
            count++;
        }
    }
    
    //
    cout<<"find "<<count<<" tiles"<<endl;
    
    //
    jout[U("tile")] = joutarray;
    
    //
    printJSONValue(jout, "", jout.is_array());
    
    //
    return 0;
    
}