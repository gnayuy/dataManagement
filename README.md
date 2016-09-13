# dataManagement
a research data management software

## compile with cmake
Install [LibTIFF][] for reading/writing TIFF images.

Install [C++ REST SDK][] for using REST API and reading/writing JSON file. To compile [C++ REST SDK][], [boost][], [OpenSSL][], and [WebSocket++][] are required.

Install [gflags][] for using the command lines.

To support 2D slice image streaming to/from DVID, [libpng][] is required. In this project, we use [LodePNG][] to decode png images.

## api
dataManagement use rest api to read/write multiple channel 3D image stacks via HTTP server:
```
  // POST Data
  % dataManagement -tiles <tilelist> -server <url:port> -uuid <uuid> -name <dataname> -methods <POST>
  
  // GET Data
  % dataManagement -tiles <tilelist> -server <url:port> -uuid <uuid> -name <dataname> -methods <GET> -sx <size_x> -sy <size_y> -sz <size_z> -sc <colorchannels> -x <offset_x> -y <offset_y> -z <offset_z> -vsx <voxelsize_x> -vsy <voxelsize_y> -vsz <voxelsize_z>
```
Data can also be uploaded to DVID branch by branch and buffer by buffer.
```
  // POST a data buffer
  % dataManagement -tiles <tilelist> -server <url:port> -uuid <uuid> -name <dataname> -methods <POST> -branch <branch #> -buffer <buffer #>
```
By default, "methods" set as "POST" and the tile list is required.
  
##

[LibTIFF]: http://www.remotesensing.org/libtiff
[C++ REST SDK]:https://github.com/Microsoft/cpprestsdk
[gflags]: https://github.com/gflags/gflags
[boost]:http://www.boost.org
[OpenSSL]:https://www.openssl.org
[WebSocket++]: https://github.com/zaphoyd/websocketpp
[libpng]: http://libpng.sf.net
[LodePNG]: http://lodev.org/lodepng

