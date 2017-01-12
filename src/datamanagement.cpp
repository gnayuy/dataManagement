// datamanagement.cpp
// manage mouse light data with dvid
// developed by Yang Yu (gnayuy@gmail.com)

#include "datamanagement.h"
#include "test.h"

// class IndexChunk
IndexBuffer::IndexBuffer(long otx, long oty, long otz, long ocx, long ocy, long ocz)
{
    tileOffsets.setXYZ(otx, oty, otz);
    chunkOffsets.setXYZ(ocx, ocy, ocz);
}

IndexBuffer::~IndexBuffer()
{

}

// class Block
Block::Block()
{

}

Block::~Block()
{

}

// class DataManager
DataManager::DataManager()
{
    m_Data = new BioMedicalData();
}

DataManager::~DataManager()
{
    clearData();
}

void DataManager::clearData()
{
    del1dp<BioMedicalData>(m_Data);
}

int DataManager::upstreaming(http_client client, uri_builder builder, unsigned char *buffer, long sx, long sy, long sz, long bufSizeX, long bufSizeY, long bufSizeZ)
{
    if(buffer!=NULL)
    {
        //
        long size = sx*sy*sz;
        bool hasData = false;

        for(long k=0; k<sz; k++)
        {
            long offz = k*sx*sy;
            for(long j=0; j<sy; j++)
            {
                long offy = offz + j*sx;
                for(long i=0; i<sx; i++)
                {
                    if(buffer[offy+i]>0)
                    {
                        hasData = true;
                        break;
                    }
                }
            }
        }

        cout<<(void *)buffer<<" "<<std::boolalpha<<hasData<<endl;

        if(hasData)
        {
            //
            unsigned char *p = NULL;
            new1dp<unsigned char, long>(p, size);
            for(long k=0; k<sz; k++)
            {
                long offz = k*sx*sy;
                long oz = k*bufSizeX*bufSizeY;
                for(long j=0; j<sy; j++)
                {
                    long offy = offz + j*sx;
                    long oy = oz + j*bufSizeX;
                    for(long i=0; i<sx; i++)
                    {
                        p[offy+i] = buffer[oy+i];
                    }
                }
            }

            //
            rawptr_buffer<unsigned char> rawBuf(p, size, std::ios::in);
            concurrency::streams::istream isbuf(rawBuf);
            httpPostAsync(client, builder, isbuf, size).wait();

            // dealloc
            del1dp<unsigned char>(p);
        }
    }
    else
    {
        cout<<"NULL pointer!"<<endl;
        return -1;
    }

    //
    return 0;
}

pplx::task<void> DataManager::httpPostAsync(http_client client, uri_builder builder, concurrency::streams::istream isbuf, utility::size64_t size)
{
    auto path_query_fragment = builder.to_string();

    http_request msg(methods::POST);
    msg.set_request_uri(path_query_fragment);
    msg.set_body(isbuf, size, U("application/octet-stream"));

    // make an HTTP POST request and asynchronously process the response
    return client.request(msg, pplx::cancellation_token::none()).then([](http_response response)
    {
        // display the status code that the server returned
        std::wostringstream stream;
        stream << U("Server returned status code ") << response.status_code() << U('.') << std::endl;
        std::wcout << stream.str();
    });
}

//int DataManager::putBufferData(tileListType tiles, utility::string_t server, utility::string_t uuid, utility::string_t dataName, long bufNumber)
//{
//    if(bufLUT.size()==0)
//    {
//        cout<<"Invalid buffer look up table!"<<endl;
//        return -1;
//    }

//    // client
//    http::uri uri = http::uri(server);
//    http_client client(uri);

//    http::uri_builder builder(U("/api/node/"));
//    builder.append_path(uuid);
//    builder.append_path(U("/"));
//    builder.append_path(dataName);
//    //builder.append_path(U("/raw/0_1_2/640_736_544/"));
//    builder.append_path(U("/raw/0_1_2/688_512_288/"));

//    cout<<builder.to_string()<<endl;

//    //
//    Tile t = tiles[0];
//    cout<<t.octreepath<<" "<<std::boolalpha<<t.visited<<" , of tiles in "<<tiles.size()<<endl;

//    // make chunk 1x4x8 (640x552x204) -> 4x3x3 (640x736x544)
//    // Least Common Multiple (dim, 32)
//    //long tsx = 640, tsy = 552, tsz = 204;
//    //long csx = 640, csy = 736, csz = 544;
//    long tsx = 688, tsy = 512, tsz = 288;
//    long csx = 688, csy = 512, csz = 288;

//    //
//    long offz, offy, offx;
//    long offTileX, offTileY, offTileZ, otx, oty, otz;
//    long offChunkX, offChunkY, offChunkZ;
//    long bufX, bufY, bufZ;

//    //
//    //bufX = tsx*4;
//    //bufY = tsy*4;
//    //bufZ = tsz*8;

//    bufX = tsx*4;
//    bufY = tsy;
//    bufZ = tsz;

//    long sizeBuf = bufX*bufY*bufZ;

//    //
//    cout<<"bufLUT "<<bufLUT.size()<<" "<<bufNumber<<endl;

//    //
//    offTileX = bufLUT[bufNumber].offTileX;
//    offTileY = bufLUT[bufNumber].offTileY;
//    offTileZ = bufLUT[bufNumber].offTileZ;

//    offChunkX = bufLUT[bufNumber].offChunkX;
//    offChunkY = bufLUT[bufNumber].offChunkY;
//    offChunkZ = bufLUT[bufNumber].offChunkZ;

//    cout<<offTileX<<" "<<offTileY<<" "<<offTileZ<<endl;

//    // find tile and fill the buffer
//    unsigned char *buffer = NULL;
//    for(long ii=0; ii<1; ii++)
//    {
//        otx = offTileX + ii;
//        for(long jj=0; jj<1; jj++)
//        {
//            oty = offTileY + jj;
//            for(long kk=0; kk<1; kk++)
//            {
//                otz = offTileZ + kk;

//                //
//                int n = findNode(tiles, otx, oty, otz);
//                if(n>=0)
//                {
//                    cout<<otx<<" "<<oty<<" "<<otz<<" : "<<tiles[n].octreepath<<endl;

//                    if(buffer==NULL)
//                    {
//                        new1dp<unsigned char>(buffer, sizeBuf);
//                    }

//                    long bx = ii*tsx;
//                    long by = jj*tsy;
//                    long bz = kk*tsz;
//                    unsigned char *buf = buffer + bz*bufX*bufY + by*bufX + bx;
//                    loadTile(buf, tiles[n].ch1, tiles[n].ch2, bufX, bufY, bufZ);
//                }

//            }
//        }
//    }

//    // upstreaming
//    if(buffer!=NULL)
//    {
//        //
//        for(long ii=0; ii<4; ii++)
//        {
//            otx = offChunkX + ii;
//            offx = otx*csx;
//            for(long jj=0; jj<1; jj++) //
//            {
//                oty = offChunkY + jj;
//                offy = oty*csy;
//                for(long kk=0; kk<1; kk++) //
//                {
//                    otz = offChunkZ + kk;
//                    offz = otz*csz;

//                    cout<<otx<<" "<<oty<<" "<<otz<<" : "<<offx<<"_"<<offy<<"_"<<offz<<endl;

//                    string offsetpath = std::to_string(offx);
//                    offsetpath.append("_");
//                    offsetpath.append(std::to_string(offy));
//                    offsetpath.append("_");
//                    offsetpath.append(std::to_string(offz));

//                    http::uri_builder queryPath = builder;
//                    queryPath.append_path(offsetpath);
//                    cout<<queryPath.to_string()<<endl;

//                    long bx = ii*csx;
//                    long by = jj*csy;
//                    long bz = kk*csz;
//                    unsigned char *buf = buffer + bz*bufX*bufY + by*bufX + bx;
//                    if(upstreaming(client, queryPath, buf, csx, csy, csz, bufX, bufY, bufZ))
//                    {
//                        cout<<"Fail to upstreaming data to the server"<<endl;
//                        return -1;
//                    }
//                }
//            }
//        }
//    }

//    // dealloc buffer
//    del1dp<unsigned char>(buffer);

//    //
//    return 0;
//}

int DataManager::putData(tileListType tiles, utility::string_t server, utility::string_t uuid, utility::string_t dataName, LongTuplet tilesize, LongTuplet chunksize, int branch)
{
    // example:
    // make chunk 1x4x8 (640x552x204) -> 4x3x3 (640x736x544)
    // Least Common Multiple (dim, 32)
    //long tsx = 640, tsy = 552, tsz = 204;
    //long csx = 640, csy = 736, csz = 544;

    long tsx = tilesize.getX(), tsy = tilesize.getY(), tsz = tilesize.getZ();
    long csx = chunksize.getX(), csy = chunksize.getY(), csz = chunksize.getZ();
    long bb_bx, bb_by, bb_bz, bb_ex, bb_ey, bb_ez, offz, offy, offx, bboffx, bboffy, bboffz;
    long offTileX, offTileY, offTileZ, otx, oty, otz;
    long offChunkX, offChunkY, offChunkZ;
    long bufX, bufY, bufZ;

    // client
    http::uri uri = http::uri(server);
    http_client client(uri);

    string sizePath = std::to_string(csx);
    sizePath.append("_");
    sizePath.append(std::to_string(csy));
    sizePath.append("_");
    sizePath.append(std::to_string(csz));

    http::uri_builder builder(U("/api/node/"));
    builder.append_path(uuid);
    builder.append_path(U("/"));
    builder.append_path(dataName);
    builder.append_path(U("/raw/0_1_2/"));
    builder.append_path(sizePath);
    builder.append_path(U("/"));

    cout<<"client builder address: "<<builder.to_string()<<endl;

    //
    Tile t = tiles[0];
    cout<<t.octreepath<<" "<<std::boolalpha<<t.visited<<endl;

    //

    switch (branch) {
    case 1:

        bb_bx = 0;
        bb_by = 0;
        bb_bz = 0;

        bb_ex = 32*tsx;
        bb_ey = 32*tsy;
        bb_ez = 32*tsz;

        bboffx = 0;
        bboffy = 0;
        bboffz = 0;

        break;

    case 2:

        break;

    case 3:

        bb_bx = 0;
        bb_by = 32*tsy;
        bb_bz = 0;

        bb_ex = 32*tsx;
        bb_ey = 64*tsy;
        bb_ez = 32*tsz;

        bboffx = 0;
        bboffy = 32;
        bboffz = 0;

        break;

    case 4:

        break;

    case 5:

        break;

    case 6:

        break;

    case 7:

        break;

    case 8:

        break;

    default:
        break;
    }

    //
    bufX = tsx*4;
    bufY = boost::math::lcm(tsy, csy);
    bufZ = boost::math::lcm(tsz, csz);

    LongTuplet bufSize(bufX, bufY, bufZ);

    long tstepx = 1;
    long tstepy = bufY/tsy;
    long tstepz = bufZ/tsz;

    long cstepx = 4;
    long cstepy = bufY/csy;
    long cstepz = bufZ/csz;

    long sizeBuf = bufX*bufY*bufZ;

    //
    for(long i=0; i<32; i+=tstepx)
    {
        offTileX = i + bboffx;
        offChunkX = i*cstepx;
        for(long j=0; j<32; j+=tstepy)
        {
            offTileY = j + bboffy;
            offChunkY = j/tstepy * cstepy;
            for(long k=0; k<32; k+=tstepz)
            {
                offTileZ = k + bboffz;
                offChunkZ = k/tstepz * cstepz;

                // find tile and fill the buffer
                unsigned char *buffer = NULL;
                for(long ii=0; ii<tstepx; ii++)
                {
                    otx = offTileX + ii;
                    for(long jj=0; jj<tstepy; jj++)
                    {
                        oty = offTileY + jj;
                        for(long kk=0; kk<tstepz; kk++)
                        {
                            otz = offTileZ + kk;

                            //
                            int n = findNode(tiles, otx, oty, otz);
                            if(n>=0)
                            {
                                cout<<otx<<" "<<oty<<" "<<otz<<" : "<<tiles[n].octreepath<<endl;

                                if(buffer==NULL)
                                {
                                    new1dp<unsigned char>(buffer, sizeBuf);
                                }

                                long bx = ii*tsx;
                                long by = jj*tsy;
                                long bz = kk*tsz;
                                unsigned char *buf = buffer + bz*bufX*bufY + by*bufX + bx;
                                loadTile(buf, tiles[n].ch1, tiles[n].ch2, bufSize);
                            }

                        }
                    }
                }

                // upstreaming
                if(buffer!=NULL)
                {
                    //
                    for(long ii=0; ii<cstepx; ii++)
                    {
                        otx = offChunkX + ii;
                        offx = bb_bx + otx*csx;
                        for(long jj=0; jj<cstepy; jj++)
                        {
                            oty = offChunkY + jj;
                            offy = bb_by + oty*csy;
                            for(long kk=0; kk<cstepz; kk++)
                            {
                                otz = offChunkZ + kk;
                                offz = bb_bz + otz*csz;

                                cout<<otx<<" "<<oty<<" "<<otz<<" "<<offx<<" "<<offy<<" "<<offz<<endl;

                                string offsetpath = std::to_string(offx);
                                offsetpath.append("_");
                                offsetpath.append(std::to_string(offy));
                                offsetpath.append("_");
                                offsetpath.append(std::to_string(offz));

                                http::uri_builder queryPath = builder;
                                queryPath.append_path(offsetpath);
                                cout<<queryPath.to_string()<<endl;

                                long bx = ii*csx;
                                long by = jj*csy;
                                long bz = kk*csz;
                                unsigned char *buf = buffer + bz*bufX*bufY + by*bufX + bx;
                                if(upstreaming(client, queryPath, buf, csx, csy, csz, bufX, bufY, bufZ))
                                {
                                    cout<<"Fail to upstreaming data to the server"<<endl;
                                    return -1;
                                }
                            }
                        }
                    }
                }

                // dealloc buffer
                del1dp<unsigned char>(buffer);
            }
        }
    }

    //
    return 0;
}

int DataManager::loadTile(unsigned char *&p, string ch1, string ch2, LongTuplet bufSize)
{
    //
    BioMedicalDataIO tile1, tile2;

    if(tile1.readData(ch1)!=0)
    {
        cout<<"Fail to read ch1!"<<endl;
        return -1;
    }

    if(tile2.readData(ch2)!=0)
    {
        cout<<"Fail to read ch1!"<<endl;
        return -1;
    }

    //
    long x,y,z;
    long sx = tile1.m_Data->size.getX();
    long sy = tile1.m_Data->size.getY();
    long sz = tile1.m_Data->size.getZ();
    int datatype = (int)(tile1.m_Data->dataType());

    if(tile2.m_Data->size.getX()!=sx || tile2.m_Data->size.getY()!=sy || tile2.m_Data->size.getZ()!=sz || tile2.m_Data->dataType()!=datatype)
    {
        cout<<"Inconsistent tiff images with channel1 "<<ch1<<" and channel 2 "<<ch2<<endl;
        return -1;
    }

    if(bufSize.getX()<sx || bufSize.getY()<sy || bufSize.getZ()<sz)
    {
        cout<<"Incorrect size of buffer"<<endl;
        return -1;
    }

    // interleaved RG copy to p
    if(datatype==USHORT)
    {
        //
        unsigned short *pData = (unsigned short*)p;
        unsigned short *pCh1 = (unsigned short*)(tile1.m_Data->data());
        unsigned short *pCh2 = (unsigned short*)(tile2.m_Data->data());

        // buffer size designed for byte data
        bufSize.setX(bufSize.getX() / 2);

        //
        for(z=0; z<sz; z++)
        {
            long offz = z*sy*sx;
            long oz = z*bufSize.getX()*bufSize.getY();
            for(y=0; y<sy; y++)
            {
                long offy = offz + y*sx;
                long oy = oz + y*bufSize.getX();
                for(x=0; x<sx; x++)
                {
                    long idx = offy + x;
                    long ind = oy + 2*x;

                    pData[ind] = pCh1[idx];
                    pData[ind+1] = pCh2[idx];
                }
            }
        }
    }
    else
    {
        // other data type
    }

    //
    return 0;
}

int DataManager::findNode(tileListType tiles, long ofx, long ofy, long ofz)
{
    //
    long size = tiles.size();
    for(long t=0; t<size; t++)
    {
        if(tiles[t].visited)
            continue;
        //
        if(tiles[t].offTileX==ofx && tiles[t].offTileY==ofy && tiles[t].offTileZ==ofz)
        {
            tiles[t].visited = true;
            return t;
        }
    }

    //
    return -1;
}

void DataManager::computeOffset(tileListType &tiles)
{
    //
    for(long t=0; t<tiles.size(); t++)
    {
        string str = tiles[t].octreepath;
        std::stringstream ss(str);

        int i;
        std::vector<int> vect;
        while (ss >> i)
        {
            vect.push_back(i);
            if (ss.peek() == '/')
                ss.ignore();
        }

        int ioff,joff,koff;
        for (i=0; i<vect.size(); i++)
        {
            if(i==0)
            {
                switch(vect.at(i))
                {
                case 1:
                    ioff = 0;
                    joff = 0;
                    koff = 0;
                    break;
                case 2:
                    ioff = 1;
                    joff = 0;
                    koff = 0;
                    break;
                case 3:
                    ioff = 0;
                    joff = 1;
                    koff = 0;
                    break;
                case 4:
                    ioff = 1;
                    joff = 1;
                    koff = 0;
                    break;
                case 5:
                    ioff = 0;
                    joff = 0;
                    koff = 1;
                    break;
                case 6:
                    ioff = 1;
                    joff = 0;
                    koff = 1;
                    break;
                case 7:
                    ioff = 0;
                    joff = 1;
                    koff = 1;
                    break;
                case 8:
                    ioff = 1;
                    joff = 1;
                    koff = 1;
                    break;
                default:
                    cout<<"Invalid octree node!"<<endl;
                    break;
                }
            }
            else
            {
                switch(vect.at(i))
                {
                case 1:
                    ioff *= 2;
                    joff *= 2;
                    koff *= 2;
                    break;
                case 2:
                    ioff = 2*ioff + 1;
                    joff *= 2;
                    koff *= 2;
                    break;
                case 3:
                    ioff *= 2;
                    joff = 2*joff + 1;
                    koff *= 2;
                    break;
                case 4:
                    ioff = 2*ioff + 1;
                    joff = 2*joff + 1;
                    koff *= 2;
                    break;
                case 5:
                    ioff *= 2;
                    joff *= 2;
                    koff = 2*koff + 1;
                    break;
                case 6:
                    ioff = 2*ioff + 1;
                    joff *= 2;
                    koff = 2*koff + 1;
                    break;
                case 7:
                    ioff *= 2;
                    joff = 2*joff + 1;
                    koff = 2*koff + 1;
                    break;
                case 8:
                    ioff = 2*ioff + 1;
                    joff = 2*joff + 1;
                    koff = 2*koff + 1;
                    break;
                default:
                    cout<<"Invalid octree node!"<<endl;
                    break;
                }
            }
        }

        //
        tiles[t].offTileX = ioff;
        tiles[t].offTileY = joff;
        tiles[t].offTileZ = koff;
    }
}

int DataManager::saveTile(string outFileName, long sx, long sy, long sz, long sc, float vsx, float vsy, float vsz, int dataType)
{
    //
    if(!m_Data)
    {
        cout<<"NULL pointer!"<<endl;
        return -1;
    }

    //
    long chnsize = sx*sy*sz;
    long size = chnsize*3; // RG -> RGB

    // cxyz -> xyzc
    if(dataType==USHORT)
    {
        unsigned short *pData = (unsigned short*)(m_Data->data());
        unsigned short *p = NULL;

        //
        new1dp<unsigned short, long>(p,size);
        for(long i=0; i<size; i++)
        {
            p[i] = 0;
        }

        //
        for(long k=0; k<sz; k++)
        {
            long offk = k*sx*sy;
            long offz = k*sy*sx*sc;
            for(long j=0; j<sy; j++)
            {
                long offj = offk + j*sx;
                long offy = offz + j*sx*sc;
                for(long i=0; i<sx; i++)
                {
                    long offx = offy + i*sc;
                    for(long c=0; c<sc; c++)
                    {
                        p[c*chnsize + offj + i] = pData[offx+c];
                    }
                }
            }
        }

        cout<<"Data copied (cxyz -> xyzc)"<<endl;

        //
        BioMedicalDataIO output;

        output.m_Data->setData((void *)p);
        output.m_Data->setDataType((DataType)dataType);
        output.m_Data->size.setXYZCT(sx,sy,sz,3,1);
        output.m_Data->spacing.setXYZCT(vsx,vsy,vsz,1.0,1.0);
        output.writeData(outFileName);

        //
        del1dp<unsigned short>(p);
    }
    else
    {
        // other data type
    }

    //
    return 0;
}

pplx::task<size_t> DataManager::httpGetAsync(http_client client, uri_builder builder, rawptr_buffer<unsigned char> rawBuf)
{
    return client.request(methods::GET, builder.to_string()).then([rawBuf](http_response response)
    {
        if(response.status_code() != status_codes::OK)
        {
            // display the status code that the server returned
            std::wostringstream stream;
            stream << U("Server returned status code ") << response.status_code() << U('.') << std::endl;
            std::wcout << stream.str();

            //            return response.extract_string().then([](utility::string_t message)
            //            {
            //                return pplx::task_from_exception<size_t>(std::exception(utility::conversions::to_utf8string(message).c_str()));
            //            });
        }

        // perform actions here reading from the response stream ...
        return response.body().read_to_end(rawBuf).get();
    });
}

int DataManager::getData(utility::string_t server, utility::string_t uuid, utility::string_t dataName, long xoff, long yoff, long zoff, long sx, long sy, long sz, long sc, float vsx, float vsy, float vsz, string outFileName)
{
    //
    Timer timer;

//    //
//    if(sz>1)
//    {
//        // 3D block data

//        //
//        string sizePath = std::to_string(sx*4);
//        sizePath.append("_");
//        sizePath.append(std::to_string(sy));
//        sizePath.append("_");
//        sizePath.append(std::to_string(sz));

//        string offsetPath = std::to_string(xoff);
//        offsetPath.append("_");
//        offsetPath.append(std::to_string(yoff));
//        offsetPath.append("_");
//        offsetPath.append(std::to_string(zoff));

//        // client
//        http::uri uri = http::uri(server);
//        http_client client(uri);

//        http::uri_builder builder(U("/api/node/"));
//        builder.append_path(uuid);
//        builder.append_path(U("/"));
//        builder.append_path(dataName);
//        builder.append_path(U("/raw/0_1_2/"));
//        builder.append_path(sizePath);
//        builder.append_path(U("/"));
//        builder.append_path(offsetPath);

//        cout<<builder.to_string()<<endl;

//        //
//        timer.start();

//        //
//        long size = 4*sx*sy*sz; // in bytes
//        new1dp<unsigned char, long>(m_Data, size);
//        rawptr_buffer<unsigned char> rawBuf(m_Data, size);
//        //concurrency::streams::ostream stream(rawBuf);

//        cout<<"create raw buffer with size "<<rawBuf.size()<<endl;

//        // GET data
//        httpGetAsync(client, builder, rawBuf).wait();

//        cout<<"download data from the server in "<<timer.getEclipseTime()<<" ms. "<<endl;

//    }
//    else if(sz==1)
//    {
//        // 2D slice data

//        //
//        string sizePath = std::to_string(sx*4);
//        sizePath.append("_");
//        sizePath.append(std::to_string(sy));

//        string offsetPath = std::to_string(xoff);
//        offsetPath.append("_");
//        offsetPath.append(std::to_string(yoff));
//        offsetPath.append("_");
//        offsetPath.append(std::to_string(zoff));

//        // client
//        http::uri uri = http::uri(server);
//        http_client client(uri);

//        http::uri_builder builder(U("/api/node/"));
//        builder.append_path(uuid);
//        builder.append_path(U("/"));
//        builder.append_path(dataName);
//        builder.append_path(U("/raw/0_1/"));
//        builder.append_path(sizePath);
//        builder.append_path(U("/"));
//        builder.append_path(offsetPath);

//        cout<<builder.to_string()<<endl;

//        //
//        timer.start();
//        //
//        long size = 4*sx*sy*sz; // in bytes
//        new1dp<unsigned char, long>(m_Data, size);
//        rawptr_buffer<unsigned char> rawBuf(m_Data, size);

//        cout<<"create raw buffer with size "<<rawBuf.size()<<endl;

//        // GET data
//        httpGetAsync(client, builder, rawBuf).wait();

//        cout<<"download data from the server in "<<timer.getEclipseTime()<<" ms. "<<endl;
//    }
//    else
//    {
//        cout<<"Invalid size!"<<endl;
//        return -1;
//    }

    //
    string sizePath = std::to_string(sx*4);
    sizePath.append("_");
    sizePath.append(std::to_string(sy));
    sizePath.append("_");
    sizePath.append(std::to_string(sz));

    string offsetPath = std::to_string(xoff);
    offsetPath.append("_");
    offsetPath.append(std::to_string(yoff));
    offsetPath.append("_");
    offsetPath.append(std::to_string(zoff));

    // client
    http::uri uri = http::uri(server);
    http_client client(uri);

    http::uri_builder builder(U("/api/node/"));
    builder.append_path(uuid);
    builder.append_path(U("/"));
    builder.append_path(dataName);
    builder.append_path(U("/raw/0_1_2/"));
    builder.append_path(sizePath);
    builder.append_path(U("/"));
    builder.append_path(offsetPath);

    cout<<builder.to_string()<<endl;

    //
    timer.start();

    //
    long size = 4*sx*sy*sz; // in bytes
    m_Data->zeros(size);
    rawptr_buffer<unsigned char> rawBuf((unsigned char*)(m_Data->data()), size);
    //concurrency::streams::ostream stream(rawBuf);

    cout<<"create raw buffer with size "<<rawBuf.size()<<endl;

    // GET data
    httpGetAsync(client, builder, rawBuf).wait();

    cout<<"download data from the server in "<<timer.getEclipseTime()<<" ms. "<<endl;

    // save as a tiff
    saveTile(outFileName, sx, sy, sz, sc, vsx, vsy, vsz, USHORT);

    cout<<"successful save the image"<<endl;

    //
    return 0;
}

void DataManager::setBufferLUT(LongTuplet tileSize, LongTuplet blockSize, LongTuplet chunkSize, int branch)
{
    //
    long ioff, joff, koff;
    switch(branch)
    {
    case 1:
        ioff = 0;
        joff = 0;
        koff = 0;
        break;
    case 2:
        ioff = tileSize.getX();
        joff = 0;
        koff = 0;
        break;
    case 3:
        ioff = 0;
        joff = tileSize.getY();
        koff = 0;
        break;
    case 4:
        ioff = tileSize.getX();
        joff = tileSize.getY();
        koff = 0;
        break;
    case 5:
        ioff = 0;
        joff = 0;
        koff = tileSize.getZ();
        break;
    case 6:
        ioff = tileSize.getX();
        joff = 0;
        koff = tileSize.getZ();
        break;
    case 7:
        ioff = 0;
        joff = tileSize.getY();
        koff = tileSize.getZ();
        break;
    case 8:
        ioff = tileSize.getX();
        joff = tileSize.getY();
        koff = tileSize.getZ();
        break;
    default:
        cout<<"Invalid octree node!"<<endl;
        break;
    }

    //
    double offx = chunkSize.getX()/blockSize.getX();
    double offy = chunkSize.getY()/blockSize.getY();
    double offz = chunkSize.getZ()/blockSize.getZ();

    //cout<<offx<<" "<<offy<<" "<<offz<<endl;

    //
    for(long i=0; i<tileSize.getX(); i+=blockSize.getX())
    {
        for(long j=0; j<tileSize.getY(); j+=blockSize.getY())
        {
            for(long k=0; k<tileSize.getZ(); k+=blockSize.getZ())
            {
                //
                IndexBuffer ib(i+ioff, j+joff, k+koff, long((i+ioff)*offx), long((j+joff)*offy), long((k+koff)*offz));

                cout<<" offsets "<<ib.chunkOffsets.getX()<<" "<<ib.chunkOffsets.getY()<<" "<<ib.chunkOffsets.getZ()<<endl;

                bufLUT.push_back(ib);
            }
        }
    }
}

// CLI
DEFINE_string(tiles, "", "a json file of the tile list");
DEFINE_string(server, "", "server address (url:port)");
DEFINE_string(uuid, "", "uuid");
DEFINE_string(name, "", "data name");
DEFINE_bool(methods, false, "GET/POST"); // GET(true)/POST(false, by default)
DEFINE_uint64(buffer, 0, "buffer#");
DEFINE_string(output, "", "output TIFF file name (.tif)");
DEFINE_uint64(sx, 1, "size (voxels) in x axis");
DEFINE_uint64(sy, 1, "size (voxels) in y axis");
DEFINE_uint64(sz, 1, "size (voxels) in z axis");
DEFINE_uint64(sc, 1, "the number of color channels");
DEFINE_uint64(x, 1, "offset (voxels) in x axis");
DEFINE_uint64(y, 1, "offset (voxels) in y axis");
DEFINE_uint64(z, 1, "offset (voxels) in z axis");
DEFINE_double(vsx, 1.0, "voxel size in x axis");
DEFINE_double(vsy, 1.0, "voxel size in y axis");
DEFINE_double(vsz, 1.0, "voxel size in z axis");
DEFINE_uint64(branch, 0, "branch number of the first level of octree");
DEFINE_uint64(cx, 1, "size (voxels) in x axis of chunk");
DEFINE_uint64(cy, 1, "size (voxels) in y axis of chunk");
DEFINE_uint64(cz, 1, "size (voxels) in z axis of chunk");

//
DEFINE_string(ch1, "", "1st color channel");
DEFINE_string(ch2, "", "2nd color channel");
DEFINE_bool(test, false, "test");
DEFINE_uint64(testOption, 0, "test option");
DEFINE_string(octreepath, "", "octree path (e.g. 3/1/6/2/2/1)");

// main
int main(int argc, char *argv[])
{
    //
    SetUsageMessage("dataManager -tiles <tilelist.json> -server <url:port> -uuid <uuid> -name <dataname> -methods <GET/POST>");
    ParseCommandLineFlags(&argc, &argv, true);

    //
    if(FLAGS_test)
    {
        if(FLAGS_testOption==0)
        {
            //time ./src/datamanagement -output data/testOutput.tif -ch1 data/default.0.tif -ch2 data/default.1.tif -test true
            return testReadWriteData(FLAGS_output, FLAGS_ch1, FLAGS_ch2);
        }
        else if(FLAGS_testOption==1)
        {
            // time ./src/datamanagement -test true -testOption 1
            testSplitData();
        }
        else if(FLAGS_testOption==2)
        {
            // time ./src/datamanagement -test true -testOption 2 -tiles ../data/tileList.json -branch 3
            testBufferMap(FLAGS_tiles, FLAGS_branch);

        }
        else if(FLAGS_testOption==3)
        {
            // time ./src/datamanagement -test true -testOption 3 -tiles ../data/tileList.json -sx 640 -sy 552 -sz 204 -octreepath 3/1/6/2/2/1
            testOctreePath(FLAGS_tiles, FLAGS_octreepath, FLAGS_sx, FLAGS_sy, FLAGS_sz);
        }
        else if(FLAGS_testOption==4)
        {
            // time ./src/datamanagement -test true -testOption 4 -server http://tem-dvid:7400 -uuid 0dd -name grayscale -methods true -x 53760 -y 17664 -z 5100 -sx 1024 -sy 256 -sz 1
            testStreamData(FLAGS_server, FLAGS_uuid, FLAGS_name, FLAGS_x, FLAGS_y, FLAGS_z, FLAGS_sx, FLAGS_sy, FLAGS_sz);
        }
        else if(FLAGS_testOption==5)
        {
            // time ./src/datamanagement -test true -testOption 5 -server http://tem-dvid:7400 -uuid 0dd -name grayscale -methods true
            testMultipleBlockStream(FLAGS_server, FLAGS_uuid, FLAGS_name);
        }
        else
        {

        }

        //
        return 0;
    }

    //
    string requestMethods = FLAGS_methods?"GET":"POST";

    //
    if(requestMethods == "POST")
    {
        // POST
        // time ./src/datamanagement -tiles ../data/tileList.json -server http://localhost:8000 -uuid 09a -name grayscale -branch 3 -buffer 0
        // time ./src/datamanagement -tiles ../test/neuron/tileList.json -server http://tem-dvid:7400 -uuid f3e -name grayscale -branch 1 -sx 688 -sy 512 -sz 288 -cx 688 -cy 512 -cz 288 >> putWholeDataLog.txt

        //
        std::cout<<"Parameters:"<<std::endl;
        std::cout<<"a json file of the tile list: "<<FLAGS_tiles<<std::endl;
        std::cout<<"server address: "<<FLAGS_server<<std::endl;
        std::cout<<"uuid: "<<FLAGS_uuid<<std::endl;
        std::cout<<"data name: "<<FLAGS_name<<std::endl;
        //std::cout<<"methods: "<<std::boolalpha<<FLAGS_methods<<std::endl;
        std::cout<<"request methods: "<< requestMethods <<std::endl;

        // load tiles info
        tileListType tiles;

        if(FLAGS_tiles.substr(FLAGS_tiles.find_last_of(".") + 1) == "json")
        {
            std::ifstream f(FLAGS_tiles);
            json::value j = json::value::parse(f);

            json::value ja = j[U("tile")];

            for(auto val : ja.as_array())
            {
                Tile t;
                t.uuid = val[U("uuid")].as_string();
                t.ch1 = val[U("ch1")].as_string();
                t.ch2 = val[U("ch2")].as_string();
                t.octreepath = val[U("octreepath")].as_string();

                tiles.push_back(t);
            }
        }
        else
        {
            std::cout<<"Invalid tile list file!"<<endl;
            return -1;
        }

        //
        DataManager dataManager;
        dataManager.computeOffset(tiles);

        // dataManager.setBufferLUT(32,32,32,1,4,8,4,3,3,FLAGS_branch); // customized for specific data size here
        // dataManager.putBufferData(tiles, FLAGS_server, FLAGS_uuid, FLAGS_name, FLAGS_buffer);

        // dataManager.setBufferLUT(32,32,32,1,1,1,4,1,1,FLAGS_branch);
        // dataManager.putBufferData(tiles, FLAGS_server, FLAGS_uuid, FLAGS_name, FLAGS_buffer);

        LongTuplet tileSize(32,32,32);
        LongTuplet blockSize(1,1,1);
        LongTuplet chunkSize(4,1,1);

        dataManager.setBufferLUT(tileSize, blockSize, chunkSize, FLAGS_branch);

        LongTuplet szTile(FLAGS_sx, FLAGS_sy, FLAGS_sz);
        LongTuplet szChunk(FLAGS_cx, FLAGS_cy, FLAGS_cz);
        dataManager.putData(tiles, FLAGS_server, FLAGS_uuid, FLAGS_name, szTile, szChunk, FLAGS_branch);
    }
    else
    {
        // GET
        // time ./src/datamanagement -server http://localhost:8000 -uuid 09a -name grayscale -methods true -x 23040 -y 21344 -z 3264 -sx 640 -sy 736 -sz 544 -sc 2 -vsx 0.25 -vsy 0.25 -vsz 1.00 -output ./test.tif

        //
        std::cout<<"Parameters:"<<std::endl;
        std::cout<<"server address: "<<FLAGS_server<<std::endl;
        std::cout<<"uuid: "<<FLAGS_uuid<<std::endl;
        std::cout<<"data name: "<<FLAGS_name<<std::endl;
        std::cout<<"request methods: "<< requestMethods <<std::endl;
        std::cout<<"offset: "<<FLAGS_x<<"_"<<FLAGS_y<<"_"<<FLAGS_z<<std::endl;
        std::cout<<"size: "<<FLAGS_sx*FLAGS_sc*2<<"_"<<FLAGS_sy<<"_"<<FLAGS_sz<<std::endl;
        std::cout<<"voxelsize: "<<FLAGS_vsx<<" "<<FLAGS_vsy<<" "<<FLAGS_vsz<<std::endl;
        std::cout<<"output: "<<FLAGS_output<<std::endl;

        //
        if(FLAGS_output.substr(FLAGS_output.find_last_of(".") + 1) != "tif")
        {
            std::cout<<"Invalid output file format!"<<endl;
            return -1;
        }

        //
        DataManager dataManager;
        dataManager.getData(FLAGS_server, FLAGS_uuid, FLAGS_name, FLAGS_x, FLAGS_y, FLAGS_z, FLAGS_sx, FLAGS_sy, FLAGS_sz, FLAGS_sc, FLAGS_vsx, FLAGS_vsy, FLAGS_vsz, FLAGS_output);
    }

    //
    return 0;
}
