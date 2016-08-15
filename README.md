# dataManagement
a research data management software

## compile with cmake
Install [LibTIFF][] for reading/writing TIFF images.

Install [C++ REST SDK][] for using REST api. To compile [C++ REST SDK][], [boost][], [OpenSSL][], and [WebSocket++][] are required.

dataManagement use [JSON for Modern C++][], C++ header, read and write meta information saved in a json file.

Install [gflags][] for using the command lines. This is an option for compiling the example code.

## api
dataManagement use rest api to read/write multiple channel 3D image stacks via HTTP server:
```
  % dataManagement <server> <POST> <fileList>
  % dataManagement <server> <GET>
```
  
##

[LibTIFF]: http://www.remotesensing.org/libtiff
[C++ REST SDK]:https://github.com/Microsoft/cpprestsdk
[gflags]: https://github.com/gflags/gflags
[JSON for Modern C++]:https://github.com/nlohmann/json
[boost]:http://www.boost.org
[OpenSSL]:https://www.openssl.org
[WebSocket++]: https://github.com/zaphoyd/websocketpp

