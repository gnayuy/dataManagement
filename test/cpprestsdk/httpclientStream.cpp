// test rest api using cpprestsdk
// Yang Yu (gnayuy@gmail.com)

// g++ -std=c++11 -I/usr/local/include -o httpclientStream httpclientStream.cpp -L/usr/local/lib -lcpprest -lboost_system -lboost_chrono -lboost_thread -lboost_random -lboost_regex -lssl -lcrypto


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
// http://localhost:8000/api/node/345/grayscale/raw/0_1/128_128/0_0_0/jpg:80

pplx::task<void> HTTPGetAsync(http_client client, uri_builder builder)
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

pplx::task<void> HTTPPostAsync(http_client client, uri_builder builder, concurrency::streams::istream isbuf, utility::size64_t size)
{
    auto path_query_fragment = builder.to_string();

    http_request msg(methods::POST);
    msg.set_request_uri(path_query_fragment);
    msg.set_body(isbuf, size, U("application/octet-stream"));

    // Make an HTTP POST request and asynchronously process the response
    return client.request(msg, pplx::cancellation_token::none()).then([](http_response response)
    {
        // Display the status code that the server returned
        std::wostringstream stream;
        stream << U("Server returned returned status code ") << response.status_code() << U('.') << std::endl;
        std::wcout << stream.str();

        stream.str(std::wstring());
        stream << U("Content length: ") << response.headers().content_length() << U(" bytes") << std::endl;
        stream << U("Content type: ") << response.headers().content_type().c_str() << std::endl;
        std::wcout << stream.str();

    });
}


//
int main(int argc, char* argv[])
{
    utility::string_t port = U("8000");
    utility::string_t address = U("http://localhost:");
    address.append(port);

    http::uri uri = http::uri(address);
    http_client client(uri);
    http::uri_builder builder(U("/api/node/61c/grayscale/raw/0_1_2/32_32_32/0_0_0"));
    //http_client client(builder.append_path(U("/api/node/4ab/grayscale/raw/0_1_2/32_32_32/0_0_0")).to_uri());
    
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
            utility::size64_t size = 32768; //32*32*32
            buf = new unsigned char [size];

            for(int i=0; i<size; i++)
            {
                buf[i] = i%255;
            }

            rawptr_buffer<unsigned char> rawBuf(buf, size, std::ios::in);
            cout<<"success raw buffer with size: "<<rawBuf.size()<<endl;

            concurrency::streams::istream isbuf(rawBuf);



            auto path_query_fragment = builder.to_string();

            cout<<"test "<<path_query_fragment.c_str()<<endl;

            HTTPPostAsync(client, builder, isbuf, size).wait();

//            try
//            {
//                auto response = client.request(methods::POST, path_query_fragment, isbuf).get();
//                printf("Response code:%d\n", response.status_code());
//            }
//            catch(http_exception& e)
//            {
//                printf("Exception:%s\n", e.what());
//            }

            //            const size_t rawDataSize = 8;
            //            unsigned char* rawData = new unsigned char[rawDataSize];
            //            memcpy(&rawData[0], "raw data", rawDataSize);
            //            rawptr_buffer<unsigned char> rawOutBuffer(rawData, rawDataSize, std::ios::in);
            //            concurrency::streams::ostream outStream;
            //            outStream.write(rawOutBuffer, rawDataSize).then([rawData](size_t bytesWritten)
            //            {
            //                delete []rawData;

            //                // Perform actions here once the string as been written...
            //            });
        }
        catch(...)
        {
            cout<<"fail to allocate memory for buf"<<endl;
            return -1;
        }

        //
        //        if(buf) {delete []buf; buf=NULL;}

    }
    else if(method==1)
    {

        HTTPGetAsync(client, builder).wait();

        //        client.request(methods::GET).then([](http_response response)
        //        {
        //            if(response.status_code() != status_codes::OK)
        //            {
        //                // Handle error cases...
        //                return pplx::task_from_result();
        //            }

        //            // Perform actions here reading from the response stream...
        //            // In this example, we print the first 15 characters of the response to the console.
        //            concurrency::streams::istream bodyStream = response.body();
        //            container_buffer<std::string> inStringBuffer;
        //            bodyStream.read(inStringBuffer, 15).then([inStringBuffer](size_t bytesRead)
        //            {
        //                const std::string &text = inStringBuffer.collection();

        //                // For demonstration, convert the response text to a wide-character string.
        //                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16conv;
        //                std::wostringstream ss;
        //                ss << utf16conv.from_bytes(text.c_str()) << std::endl;
        //                std::wcout << ss.str();
        //            });
        //        });


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
