// datamanagement.cpp
// manage mouse light data with dvid
// developed by Yang Yu (gnayuy@gmail.com)

#include "datamanagement.h"

// class image
Image::Image()
{
    p = NULL;
}

Image::~Image()
{
    if(p)
    {
        delete []p;
        p = NULL;
    }
}

void Image::getOffset(string octreepath, double ox, double oy, double oz, double vx, double vy, double vz, long dimx, long dimy, long dimz, double &offx, double &offy, double &offz)
{

}

void Image::setOrigin(double x, double y, double z)
{

}

void Image::setDimension(long x, long  y, long z)
{

}

void Image::setResolution(double x, double y, double z)
{

}

// class DataManager
DataManager::DataManager()
{

}

DataManager::~DataManager()
{

}

void DataManager::upstreaming(unsigned char *buffer, long offx, long offy, long offz)
{

}

// CLI
DEFINE_string(tiles, "", "a json file of the tile list");
DEFINE_string(server, "", "server address (url:port)");
DEFINE_string(uuid, "", "uuid");
DEFINE_string(name, "", "data name");
DEFINE_bool(methods, false, "GET/POST"); // GET(true)/POST(false)

// main
int main(int argc, char *argv[])
{
    //
    gflags::SetUsageMessage("dataManager -tiles <tilelist.json> -server <url:port> -uuid <uuid> -name <dataname> -methods <GET/POST>");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    //
    std::cout<<"Parameters:"<<std::endl;
    std::cout<<"a json file of the tile list: "<<FLAGS_tiles<<std::endl;
    std::cout<<"server address: "<<FLAGS_server<<std::endl;
    std::cout<<"uuid: "<<FLAGS_uuid<<std::endl;
    std::cout<<"data name: "<<FLAGS_name<<std::endl;

    string requestMethods = FLAGS_methods?"GET":"POST";
    //std::cout<<"methods: "<<std::boolalpha<<FLAGS_methods<<std::endl;
    std::cout<<"request methods: "<< requestMethods <<std::endl;

    //
    return 0;
}
