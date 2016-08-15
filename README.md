# dataManagement
a research data management software

## compile
To compile dataManagement, you need to compile [LibTIFF][] and [C++ REST SDK][] first. To compile [C++ REST SDK][], you have to compile [boost][] and [OpenSSL][].

dataManagement use [JSON for Modern C++][] read and write data information saved in json file.

## api
dataManagement use rest api to read/write multiple channel 3D image stacks via HTTP server:

  % dataManagement <server> <POST> <fileList>
  % dataManagement <server> <GET>
  

[LibTIFF]: http://www.remotesensing.org/libtiff
[C++ REST SDK]:https://github.com/Microsoft/cpprestsdk
[JSON for Modern C++]:https://github.com/nlohmann/json
[boost]:http://www.boost.org
[OpenSSL]:https://www.openssl.org

