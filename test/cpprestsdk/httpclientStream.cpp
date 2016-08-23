// test rest api using cpprestsdk
// Yang Yu (gnayuy@gmail.com)

// g++ -std=c++11 -I/usr/local/include -o httpclientDataStream httpclientDataStream.cpp -L/usr/local/lib -lcpprest -lboost_system -lboost_chrono -lboost_thread -lboost_random -lboost_regex -lssl -lcrypto


//
#include <iostream>
#include <sstream>
#include <codecvt>
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

// POST <api URL>/node/<UUID>/<data name>/raw/0_1_2/<size>/<offset>[?queryopts]

//
int main(int argc, char* argv[])
{
    utility::string_t port = U("8000");
    utility::string_t address = U("http://localhost:");
    address.append(port);

    http::uri uri = http::uri(address);
    http::uri_builder builder(uri);
    http_client client(builder.append_path(U("/api/node/cd0/grayscale/raw/0_1_2/32_32_32/0_0_0")).to_uri());
    
    if(argc<2)
    {
        cout<<"Please enter POST[0]/GET[1] method option!"<<endl;
        return -1;
    }

    int method = atoi(argv[1]);
    
    if(method==0)
    {
        unsigned char *buf = NULL;
        try
        {
            const size_t size = 32*32*32;
            buf = new unsigned char [size];

            for(int i=0; i<size; i++)
            {
                buf[i] = i%255;
            }

            cout<<"test raw buffer"<<endl;
            rawptr_buffer<unsigned char> rawBuf(buf, size, std::ios::in);
            cout<<"success raw buffer"<<rawBuf.size()<<endl;

//            concurrency::streams::istream isbuf;

//            isbuf.read(rawBuf,size);
//            cout<<"success read rawbuf to istream"<<endl;

            try
            {
                //auto response = client.request(methods::POST, builder.to_string(), isbuf, size, U("application/octet-stream"), pplx::cancellation_token::none()).get();
                auto response = client.request(methods::POST, builder.to_string(), rawBuf, U("application/octet-stream")).get();
                printf("Response code:%d\n", response.status_code());
            }
            catch(http_exception& e)
            {
                printf("Exception:%s\n", e.what());
            }
        }
        catch(...)
        {
            cout<<"fail to allocate memory for buf"<<endl;
            return -1;
        }

    }
    else if(method==1)
    {

        client.request(methods::GET).then([](http_response response)
        {
            if(response.status_code() != status_codes::OK)
            {
                // Handle error cases...
                return pplx::task_from_result();
            }

            // Perform actions here reading from the response stream...
            // In this example, we print the first 15 characters of the response to the console.
            concurrency::streams::istream bodyStream = response.body();
            container_buffer<std::string> inStringBuffer;
            bodyStream.read(inStringBuffer, 15).then([inStringBuffer](size_t bytesRead)
            {
                const std::string &text = inStringBuffer.collection();

                // For demonstration, convert the response text to a wide-character string.
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16conv;
                std::wostringstream ss;
                ss << utf16conv.from_bytes(text.c_str()) << std::endl;
                std::wcout << ss.str();
            });
        });


//        try
//        {
//            auto response = client.request(methods::GET, builder.to_string(), buf, U("application/octet-stream")).get();
//            printf("Response code:%d, buf[8]=%d\n", response.status_code(), buf[8]);
//        }
//        catch(http_exception& e)
//        {
//            printf("Exception:%s\n", e.what());
//        }
    }
    else
    {
        cout<<"Invalid request method!"<<endl;
        return -1;
    }

    //
    return 0;
}
