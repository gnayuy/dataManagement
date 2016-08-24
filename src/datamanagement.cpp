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

pplx::task<void> DataManager::httpPostAsync(http_client client, uri_builder builder, concurrency::streams::istream isbuf, utility::size64_t size)
{
    auto path_query_fragment = builder.to_string();

    // Make an HTTP GET request and asynchronously process the response
    return client.request(methods::GET, path_query_fragment).then([](http_response response)
    {
        // Display the status code that the server returned
        std::wostringstream stream;
        stream << U("Server returned returned status code ") << response.status_code() << U('.') << std::endl;
        std::wcout << stream.str();

        stream.str(std::wstring());
        stream << U("Content length: ") << response.headers().content_length() << U(" bytes") << std::endl;
        stream << U("Content type: ") << response.headers().content_type().c_str() << std::endl;
        std::wcout << stream.str();

        auto bodyStream = response.body();
        concurrency::streams::stringstreambuf sbuffer;
        auto& target = sbuffer.collection();

        bodyStream.read_to_end(sbuffer).get();

        stream.str(std::wstring());
        stream << U("Response body: ") << target.c_str();
        std::wcout << stream.str();
    });
}

int DataManager::putData(tileListType tiles, utility::string_t server, utility::string_t uuid, utility::string_t dataName)
{
    //
    return 0;
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

    // tile list
    if(requestMethods == "POST")
    {
        // load tiles info
        tileListType tiles;

        if(FLAGS_tiles.substr(FLAGS_tiles.find_last_of(".") + 1) == "json")
        {
            std::ifstream f(FLAGS_tiles);
            json::value j = json::value::parse(f);

            json::value ja = j[U("tile")];

            for(auto val : ja.as_array())
            {
                Tile t;
                t.uuid = val[U("uuid")].as_string();
                t.ch1 = val[U("ch1")].as_string();
                t.ch2 = val[U("ch2")].as_string();
                t.octreepath = val[U("octreepath")].as_string();

                tiles.push_back(t);
            }
        }
        else
        {
            std::cout<<"Invalid tile list file!"<<endl;
            return -1;
        }
    }
    else
    {
        // GET
    }



    //
    return 0;
}
