// dataio.h
// read/write .tif, .json, ...
// developed by Yang Yu (gnayuy@gmail.com)

#ifndef __DATAIO_H__
#define __DATAIO_H__

//
#include "biomedicaldataio.h"

//
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

#include <cpprest/http_listener.h>              // HTTP server
#include <cpprest/json.h>                       // JSON library
#include <cpprest/uri.h>                        // URI library
#include <cpprest/ws_client.h>                  // WebSocket client
#include <cpprest/containerstream.h>            // Async streams backed by STL containers
#include <cpprest/interopstream.h>              // Bridges for integrating Async streams with STL and WinRT streams
#include <cpprest/rawptrstream.h>               // Async streams backed by raw pointer to memory
#include <cpprest/producerconsumerstream.h>     // Async streams for producer consumer scenarios

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

//using namespace web::http::experimental::listener;          // HTTP server
//using namespace web::experimental::web_sockets::client;     // WebSockets client
//using namespace web::json;                                  // JSON library

//
/// metainfo
//
class MetaInfo
{
public:
    MetaInfo();
    ~MetaInfo();
    
public:
    string name; // name of the specimen
    string unit; // "micrometer"/"nanometer"
    double voxelsize_x, voxelsize_y, voxelsize_z;
    double origin_x, origin_y, origin_z;
    double size_x, size_y, size_z;
    unsigned int dim_x, dim_y, dim_z;
    unsigned int bits_per_voxel;
    unsigned int number_of_channels;
};

//
/// tile
//
class Tile
{
public:
    Tile();
    ~Tile();
    
public:
    string uuid;
    string octreepath;
    string ch1, ch2, ch3;
    long offTileX, offTileY, offTileZ;
    bool visited;
};

typedef std::vector<Tile> tileListType;

#endif // __DATAIO_H__
