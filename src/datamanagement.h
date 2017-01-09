// datamanagement.h
// manage mouse light data with dvid
// developed by Yang Yu (gnayuy@gmail.com)

#ifndef __DATAMANAGEMENT_H__
#define __DATAMANAGEMENT_H__

#include "dataio.h"
#include <gflags/gflags.h>
#include "png/lodepng.h"
#include <boost/math/common_factor_rt.hpp>

//
using namespace google;

//
typedef Tuplet<long> LongTuplet;

//
class IndexBuffer
{
public:
    IndexBuffer(long otx, long oty, long otz, long ocx, long ocy, long ocz);
    ~IndexBuffer();

public:
    LongTuplet tileOffsets, chunkOffsets;
};

typedef std::vector<IndexBuffer> IndexBufferType;

//
class Block
{
public:
    Block();
    ~Block();

public:
    LongTuplet size; // block size
    LongTuplet offsets; // block offset in database
    LongTuplet blockOffsets; // block offset in buffer
};

typedef std::vector<Block> BlockList;

//
class DataManager
{
public:
    DataManager();
    ~DataManager();

public:
    int upstreaming(http_client client, uri_builder builder, unsigned char *buffer, long sx, long sy, long sz, long bufSizeX, long bufSizeY, long bufSizeZ);
    int loadTile(unsigned char *&p, string ch1, string ch2, LongTuplet bufSize);
    int findNode(tileListType tiles, long ofx, long ofy, long ofz);
    void computeOffset(tileListType &tiles);
    int saveTile(string outFileName, long sx, long sy, long sz, long sc, float vsx, float vsy, float vsz, int dataType);

    pplx::task<void> httpPostAsync(http_client client, uri_builder builder, concurrency::streams::istream isbuf, utility::size64_t size);
    int putData(tileListType tiles, utility::string_t server, utility::string_t uuid, utility::string_t dataName, LongTuplet tilesize, LongTuplet chunksize, int branch);
    //int putBufferData(tileListType tiles, utility::string_t server, utility::string_t uuid, utility::string_t dataName, long bufNumber);

    pplx::task<size_t> httpGetAsync(http_client client, uri_builder builder, rawptr_buffer<unsigned char> rawBuf);
    int getData(utility::string_t server, utility::string_t uuid, utility::string_t dataName, long xoff, long yoff, long zoff, long sx, long sy, long sz, long sc, float vsx, float vsy, float vsz, string outFileName);

    void clearData();

public:
    void setBufferLUT(LongTuplet tileSize, LongTuplet blockSize, LongTuplet chunkSize, int branch);

public:
    BioMedicalData *m_Data;
    IndexBufferType bufLUT;
};

//
// octree:
// z=0 1 2  z=1  5 6
//     3 4       7 8
//
//

#endif // __DATAMANAGEMENT_H__
