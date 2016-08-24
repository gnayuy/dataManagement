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
    int upstreaming(http_client client, uri_builder builder, unsigned char *buffer, long size);
    pplx::task<void> httpPostAsync(http_client client, uri_builder builder, concurrency::streams::istream isbuf, utility::size64_t size);
    int putData(tileListType tiles, utility::string_t server, utility::string_t uuid, utility::string_t dataName);
    int loadTile(unsigned char *p, string ch1, string ch2);
    int findNode(tileListType tiles, long xoff, long yoff, long zoff);
    void computeOffset(tileListType &tiles);
};

#endif // __DATAMANAGEMENT_H__
