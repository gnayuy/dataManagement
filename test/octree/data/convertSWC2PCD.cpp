// g++ -o convertSWC2PCD convertSWC2PCD.cpp

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

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
            std::cerr << "No line ending character found, not a valid ITK Transform TXT file"<<endl;
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
        
        //
        
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

//
bool writePCD(const char* fn, PointCloud pc)
{
    string filename = const_cast<char*>(fn);
    
    //
    std::ofstream out(fn, std::ios_base::out | std::ios_base::app);
    
    //
    if(out.is_open())
    {
        out.seekp (0, ios::beg);
        
        out << "# .PCD v.5 - Point Cloud Data file format" << std::endl;
        out << "VERSION .5" << std::endl;
        out << "FIELDS x y z" << std::endl;
        out << "SIZE 4 4 4" << std::endl;
        out << "TYPE F F F" << std::endl;
        out << "COUNT 1 1 1" << std::endl;
        out << "WIDTH "<< pc.size() << std::endl;
        out << "HEIGHT 1" << std::endl;
        out << "POINTS " << pc.size() << std::endl;
        out << "DATA ascii" << std::endl;

        for(long i=0; i<pc.size(); i++)
        {
            out << pc[i].x << " " << pc[i].y << " " << pc[i].z << std::endl;
        }
        out << std::endl;
        
        //
        out.close();
    }
    else
    {
        cout<<"Unable to open the output file"<<endl;
        return false;
    }
    
    //
    return true;
}

//
int main (int argc, char* argv[])
{
    // const
    float OFFX=72022.286767;
    float OFFY=15966.638635;
    float OFFZ=32203.173689;
    
    float OX=68097.320;
    float OY=13754.192;
    float OZ=27557.100;
    
    float SX=15984.70639534884;
    float SY=16031.19140625;
    float SZ=64066.66666666666;
    
    //
    if(argc<3)
    {
        std::cout<<"need at least 2 inputs"<<endl;
        return -1;
    }
    
    //
    const char* swcFile = (const char *) (argv[1]); // swc
    const char * pcdFile = (const char *) (argv[2]); // pcd
    
    PointCloud pc;
    
    //
    if(readSWC(swcFile, pc) == false)
    {
        cout<<"fail to read swc"<<endl;
        return -1;
    }
    
    //
    for(int i=0; i< pc.size(); i++)
    {
        pc[i].x = (pc[i].x + OFFX - OX) / SX;
        pc[i].y = (pc[i].y + OFFY - OY) / SY;
        pc[i].z = (pc[i].z + OFFZ - OZ) / SZ;
    }
    
    //
    if(writePCD(pcdFile, pc) == false)
    {
        cout<<"fail in writing pcd"<<endl;
        return -1;
    }
    
    
    //
    return 0;
    
}