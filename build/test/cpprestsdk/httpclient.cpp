// test rest api using cpprestsdk
// Yang Yu (gnayuy@gmail.com)

// g++ -std=c++11 -I/usr/local/include -o httpclient httpclient.cpp -L/usr/local/lib -lcpprest -lboost_system -lboost_chrono -lboost_thread -lboost_random -lboost_regex -lssl -lcrypto


//
#include <iostream>
#include <sstream>

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

// Creates an HTTP request and prints the length of the response stream.
pplx::task<void> HTTPStreamingAsync()
{
    http_client client(U("http://localhost:8000/api/help"));

    // Make the request and asynchronously process the response. 
    return client.request(methods::GET).then([](http_response response)
    {
        // Print the status code.
        std::wostringstream ss;
        ss << U("Server returned returned status code ") << response.status_code() << U('.') << std::endl;
        std::wcout << ss.str();

        // TODO: Perform actions here reading from the response stream.
        auto bodyStream = response.body();

        // In this example, we print the length of the response to the console.
        ss.str(std::wstring());
        ss << U("Content length is ") << response.headers().content_length() << U(" bytes.") << std::endl;
        std::wcout << ss.str();
    });

    /* Sample output:
    Server returned returned status code 200.
    Content length is 63803 bytes.
    */
}

// Builds an HTTP request that uses custom header values.
pplx::task<void> HTTPRequestCustomHeadersAsync()
{
    http_client client(U("http://www.fourthcoffee.com"));

    // Manually build up an HTTP request with header and request URI.
    http_request request(methods::GET);
    request.headers().add(U("MyHeaderField"), U("MyHeaderValue"));
    request.set_request_uri(U("requestpath"));
    return client.request(request).then([](http_response response)
    {
        // Print the status code.
        std::wostringstream ss;
        ss << U("Server returned returned status code ") << response.status_code() << U(".") << std::endl;
        std::wcout << ss.str();
    });

    /* Sample output:
    Server returned returned status code 200.
    */
}

// Upload a file to an HTTP server.
pplx::task<void> UploadFileToHttpServerAsync()
{
    using concurrency::streams::file_stream;
    using concurrency::streams::basic_istream;

    // To run this example, you must have a file named myfile.txt in the current folder. 
    // Alternatively, you can use the following code to create a stream from a text string. 
    // std::string s("abcdefg");
    // auto ss = concurrency::streams::stringstream::open_istream(s); 

    // Open stream to file. 
    return file_stream<unsigned char>::open_istream(U("myfile.txt")).then([](pplx::task<basic_istream<unsigned char>> previousTask)
    {
        try
        {
            auto fileStream = previousTask.get();

            // Make HTTP request with the file stream as the body.
            http_client client(U("http://www.fourthcoffee.com"));
            return client.request(methods::PUT, U("myfile"), fileStream).then([fileStream](pplx::task<http_response> previousTask)
            {
                fileStream.close();

                std::wostringstream ss;
                try
                {
                    auto response = previousTask.get();
                    ss << U("Server returned returned status code ") << response.status_code() << U(".") << std::endl;
                }
                catch (const http_exception& e)
                {
                    ss << e.what() << std::endl;
                }
                std::wcout << ss.str();
            });
        }
        catch (const std::system_error& e)
        {
            std::wostringstream ss;
            ss << e.what() << std::endl;
            std::wcout << ss.str();

            // Return an empty task. 
            return pplx::task_from_result();
        }
    });

    /* Sample output:
    The request must be resent
    */
}
//
//int wmain()
//{
//    // This example uses the task::wait method to ensure that async operations complete before the app exits.  
//    // In most apps, you typically don�t wait for async operations to complete.
//
//    std::wcout << L"Calling HTTPStreamingAsync..." << std::endl;
//    HTTPStreamingAsync().wait();
//
//    std::wcout << L"Calling HTTPRequestCustomHeadersAsync..." << std::endl;
//    HTTPRequestCustomHeadersAsync().wait();
//
//    std::wcout << L"Calling UploadFileToHttpServerAsync..." << std::endl;
//    UploadFileToHttpServerAsync().wait();
//}

int main(int argc, char* argv[])
{
    auto fileStream = std::make_shared<ostream>();
    
    // Open stream to output file.
    pplx::task<void> requestTask = fstream::open_ostream(U("results.html")).then([=](ostream outFile)
                                                                                 {
                                                                                     *fileStream = outFile;
                                                                                     
                                                                                     // Create http_client to send the request.
                                                                                     http_client client(U("http://localhost:8000/"));
                                                                                     
                                                                                     // Build request URI and start the request.
                                                                                     uri_builder builder(U("/api/help"));
                                                                                     //builder.append_query(U("q"), U("cpprestsdk github"));
                                                                                     return client.request(methods::GET, builder.to_string());
                                                                                 })
    
    // Handle response headers arriving.
    .then([=](http_response response)
          {
              printf("Received response status code:%u\n", response.status_code());
              
              // Write response body into the file.
              return response.body().read_to_end(fileStream->streambuf());
          })
    
    // Close the file stream.
    .then([=](size_t)
          {
              return fileStream->close();
          });
    
    // Wait for all the outstanding I/O to complete and handle any exceptions
    try
    {
        requestTask.wait();
    }
    catch (const std::exception &e)
    {
        printf("Error exception:%s\n", e.what());
    }
    
    //
    HTTPStreamingAsync().wait();
    
    web::json::value postParameters = web::json::value::object();
    
    postParameters[U("Parameter1")] = web::json::value::string(U("Test1"));
    postParameters[U("Parameter2")] = web::json::value::string(U("Test2"));
    
    return 0;
}
