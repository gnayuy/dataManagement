# dataManagement
a research data management software

## compile with cmake
Install [LibTIFF][] for reading/writing TIFF images.

Install [C++ REST SDK][] for using REST API and reading/writing JSON file. To compile [C++ REST SDK][], [boost][], [OpenSSL][], and [WebSocket++][] are required.

Install [gflags][] for using the command lines. This is an option for compiling the example code.

## api
dataManagement use rest api to read/write multiple channel 3D image stacks via HTTP server:
```
  % dataManagement -tiles <tilelist> -server <url:port> -uuid <uuid> -name <dataname> -methods <GET/POST>
```
By default, "methods" set as "POST" and the tile list is required.
  
##

[LibTIFF]: http://www.remotesensing.org/libtiff
[C++ REST SDK]:https://github.com/Microsoft/cpprestsdk
[gflags]: https://github.com/gflags/gflags
[boost]:http://www.boost.org
[OpenSSL]:https://www.openssl.org
[WebSocket++]: https://github.com/zaphoyd/websocketpp

