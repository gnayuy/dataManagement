// test rest api using cpprestsdk
// Yang Yu (gnayuy@gmail.com)

// g++ -std=c++11 -I/usr/local/include -o httpclientPost httpclientPost.cpp -L/usr/local/lib -lcpprest -lboost_system -lboost_chrono -lboost_thread -lboost_random -lboost_regex -lssl -lcrypto


//
#include <iostream>
#include <sstream>
using namespace std;

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

// POST <api URL>/node/<UUID>/<data name>/key/<key>
// curl -X POST -d @data/mouselight_2015_07_11_johan.json http://localhost:8000/api/node/cd0/metadata/key/test1 --header "Content-Type:application/json"
// echo '{"text": "Hello **world**!"}' | curl -d @- http://localhost:8000/api/node/cd0/metadata/key/test2 --header "Content-Type:application/json"

//
int main(int argc, char* argv[])
{
    utility::string_t port = U("8000");
    utility::string_t address = U("http://localhost:");
    address.append(port);

    http::uri uri = http::uri(address);
    http::uri_builder builder(uri);
    http_client client(builder.append_path(U("/api/node/cd0/metadata/key/keytest")).to_uri());
    
    

//    json::value j = json::value::array();
//    j[0] = 1;
//    j[2] = 3;

    json::value j;

    j[U("name")] = json::value(U("John"));
    j[U("age")] = json::value(U("66"));
    cout<<j.serialize()<<endl;
    
    try
    {
        auto response = client.request(methods::POST, builder.to_string(), j.serialize(), U("application/json")).get();
        printf("Response code:%d\n", response.status_code());
    }
    catch(http_exception& e)
    {
        printf("Exception:%s\n", e.what());
    }

//    http_request msg(methods::POST);
//    msg.headers().add(U("MyHeader"), U("hehe;blach"));
//    msg.headers().add(U("Yo1"), U("You, Too"));
//    msg.headers().add(U("Yo2"), U("You2"));
//
//    try
//    {
//        auto response = client.request(msg).get();
//        printf("Response code:%d\n", response.status_code());               
//    }
//    catch(http_exception& e)
//    {
//        printf("Exception:%s\n", e.what());                
//    } 

    
    //
    return 0;
}
