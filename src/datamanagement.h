// datamanagement.h
// manage mouse light data with dvid
// developed by Yang Yu (gnayuy@gmail.com)

#ifndef __DATAMANAGEMENT_H__
#define __DATAMANAGEMENT_H__

#include "dataio.h"
#include <gflags/gflags.h>

//
class Image
{
public:
    Image();
    ~Image();
    
public:
    void getOffset(string octreepath, double ox, double oy, double oz, double vx, double vy, double vz, long dimx, long dimy, long dimz, double &offx, double &offy, double &offz);
    void setOrigin(double x, double y, double z);
    void setDimension(long x, long  y, long z);
    void setResolution(double x, double y, double z);

public:
    unsigned char *p;
    double ox,oy,oz; // origin
    double vx,vy,vz; // voxelsize
    long dimx,dimy,dimz; // dimension
};

//
class DataManager
{
public:
    DataManager();
    ~DataManager();

public:
    int upstreaming(http_client client, uri_builder builder, unsigned char *buffer, long bx, long by, long bz, long sx, long sy, long sz, long bufSizeX, long bufSizeY, long bufSizeZ);
    int loadTile(unsigned char *&p, string ch1, string ch2, long bx, long by, long bz, long bufSizeX, long bufSizeY, long bufSizeZ);
    int findNode(tileListType tiles, long xoff, long yoff, long zoff);
    void computeOffset(tileListType &tiles);
    int saveTile(unsigned char *p, string outFileName, long sx, long sy, long sz, long sc, float vsx, float vsy, float vsz, int dataType);

    pplx::task<void> httpPostAsync(http_client client, uri_builder builder, concurrency::streams::istream isbuf, utility::size64_t size);
    int putData(tileListType tiles, utility::string_t server, utility::string_t uuid, utility::string_t dataName);

    pplx::task<void> httpGetAsync(http_client client, uri_builder builder, utility::size64_t size);
    int getData(utility::string_t server, utility::string_t uuid, utility::string_t dataName, long xoff, long yoff, long zoff, long sx, long sy, long sz, long sc, float vsx, float vsy, float vsz, string outFileName);
};

#endif // __DATAMANAGEMENT_H__
