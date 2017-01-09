// test.cpp
// manage mouse light data with dvid
// developed by Yang Yu (gnayuy@gmail.com)


#include "test.h"

//
int testReadWriteData(string outFileName, string ch1, string ch2)
{
    //
    unsigned char *p = NULL;

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

    // interleaved RG copy to p
    long szChannel = sx*sy*sz;
    long size = szChannel*6; // ushort with 2 colors
    new1dp<unsigned char, long>(p, size);

    //
    if(datatype==USHORT)
    {
        //
        unsigned short *pData = (unsigned short*)p;
        unsigned short *pCh1 = (unsigned short*)(tile1.m_Data->data());
        unsigned short *pCh2 = (unsigned short*)(tile2.m_Data->data());

        //
        for(z=0; z<sz; z++)
        {
            long offz = z*sy*sx;
            for(y=0; y<sy; y++)
            {
                long offy = offz + y*sx;
                for(x=0; x<sx; x++)
                {
                    long idx = offy + x;

                    pData[idx] = pCh1[idx];
                    pData[idx+szChannel] = pCh2[idx];
                    pData[idx+2*szChannel] = 0;
                }
            }
        }
    }
    else
    {
        // other data type
    }

    // stream buffer
    rawptr_buffer<unsigned char> rawBuf(p, size, std::ios::in);
    concurrency::streams::istream isbuf(rawBuf);

    unsigned char *pOut = NULL;
    new1dp<unsigned char, long>(pOut, size);
    rawptr_buffer<unsigned char> rb(pOut, size);
    //concurrency::streams::ostream osbuf(rb);

    cout<< isbuf.read_to_end(rb).get() << endl;

    // save output
    BioMedicalDataIO output;

    output.m_Data->setData((void *)pOut);
    output.m_Data->setDataType((DataType)datatype);
    output.m_Data->size.setXYZCT(sx,sy,sz,3,1);
    output.m_Data->spacing.setXYZCT(tile1.m_Data->spacing.getX(),tile1.m_Data->spacing.getY(),tile1.m_Data->spacing.getZ(),1.0,1.0);
    output.writeData(outFileName);

    //
    del1dp<unsigned char>(p);
    del1dp<unsigned char>(pOut);

    //
    return 0;
}

//
int testSplitData()
{
    // make chunk 1x4x8 (640x552x204) -> 4x3x3 (640x736x544)
    // Least Common Multiple (dim, 32)
    long tsx = 640, tsy = 552, tsz = 204;
    long csx = 640, csy = 736, csz = 544;
    long level1;
    long bb_bx, bb_by, bb_bz, bb_ex, bb_ey, bb_ez, offz, offy, offx;
    long offTileX, offTileY, offTileZ, otx, oty, otz;
    long offChunkX, offChunkY, offChunkZ;
    long bufX, bufY, bufZ;

    //
    level1 = 3; // pick branch 3

    bb_bx = 0;
    bb_by = 32*tsy;
    bb_bz = 0;

    bb_ex = 32*tsx;
    bb_ey = 64*tsy;
    bb_ez = 32*tsz;

    bufX = tsx*4;
    bufY = tsy*4;
    bufZ = tsz*8;

    long sizeBuf = bufX*bufY*bufZ;

    //
    long nBuffer = 0;

    //
    for(long i=0; i<32; i++)
    {
        offTileX = i;
        offChunkX = i*4;
        for(long j=0; j<32; j+=4)
        {
            offTileY = j + 32;
            offChunkY = j/4 * 3;
            for(long k=0; k<32; k+=8)
            {
                offTileZ = k;
                offChunkZ = k/8 * 3;

                // find tile and fill the buffer
                unsigned char *buffer = NULL;
                for(long ii=0; ii<1; ii++)
                {
                    otx = offTileX + ii;
                    for(long jj=0; jj<4; jj++)
                    {
                        oty = offTileY + jj;
                        for(long kk=0; kk<8; kk++)
                        {
                            otz = offTileZ + kk;

                            //
                            //cout<<otx<<" "<<oty<<" "<<otz<<endl;
                        }
                    }
                }

                // upstreaming
                for(long ii=0; ii<4; ii++)
                {
                    otx = offChunkX + ii;
                    offx = bb_bx + otx*csx;
                    for(long jj=0; jj<3; jj++)
                    {
                        oty = offChunkY + jj;
                        offy = bb_by + oty*csy;
                        for(long kk=0; kk<3; kk++)
                        {
                            otz = offChunkZ + kk;
                            offz = bb_bz + otz*csz;

                            //cout<<otx<<" "<<oty<<" "<<otz<<" : /"<<offx<<"_"<<offy<<"_"<<offz<<endl;
                        }
                    }
                }

                //
                cout<<i<<" "<<j<<" "<<k<<" "<<nBuffer++<<endl;
            }
        }
    }

    //
    return 0;
}

int testOctreePath(string tilesFile, string octreePath, long sx, long sy, long sz)
{
    // load tiles info
    tileListType tiles;

    if(tilesFile.substr(tilesFile.find_last_of(".") + 1) == "json")
    {
        std::ifstream f(tilesFile);
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

    //
    long size = tiles.size();
    bool found = false;
    for(long t=0; t<size; t++)
    {
        //
        if(tiles[t].octreepath==octreePath)
        {
            found = true;

            cout<<tiles[t].ch1<<endl;
            cout<<tiles[t].ch2<<endl;
            cout<<tiles[t].offTileX*sx*4<<"_"<<tiles[t].offTileY*sy<<"_"<<tiles[t].offTileZ*sz<<endl;

            break;
        }
    }

    if(!found)
    {
        cout<<"octree path "<<octreePath.c_str()<<" does not exist."<<endl;
    }

    //
    return 0;
}

int testBufferMap(string tilesFile, int branch)
{
    // load tiles info
    tileListType tiles;

    if(tilesFile.substr(tilesFile.find_last_of(".") + 1) == "json")
    {
        std::ifstream f(tilesFile);
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

    LongTuplet tileSize(32,32,32);
    LongTuplet blockSize(1,4,8);
    LongTuplet chunkSize(4,3,3);

    dataManager.setBufferLUT(tileSize,blockSize,chunkSize, branch);

    //
    bool hasData = false;

    //
    for(long i=0; i<dataManager.bufLUT.size(); i++)
    {
        //
        cout<<"Buffer #"<<i<<":"<<endl;

        cout<<dataManager.bufLUT[i].tileOffsets.getX()<<" "<<dataManager.bufLUT[i].tileOffsets.getY()<<" "<<dataManager.bufLUT[i].tileOffsets.getZ()<<endl;
        cout<<dataManager.bufLUT[i].chunkOffsets.getX()<<" "<<dataManager.bufLUT[i].chunkOffsets.getY()<<" "<<dataManager.bufLUT[i].chunkOffsets.getZ()<<endl;
        cout<<endl;

        //
        long otx, oty, otz;

        // find tile
        for(long ii=0; ii<1; ii++)
        {
            otx = dataManager.bufLUT[i].tileOffsets.getX() + ii;
            for(long jj=0; jj<4; jj++)
            {
                oty = dataManager.bufLUT[i].tileOffsets.getY() + jj;
                for(long kk=0; kk<8; kk++)
                {
                    otz = dataManager.bufLUT[i].tileOffsets.getZ() + kk;

                    //
                    int n = dataManager.findNode(tiles, otx, oty, otz);
                    if(n>=0)
                    {
                        //cout<<" - "<<otx<<" "<<oty<<" "<<otz<<" : "<<tiles[n].octreepath<<endl;
                        hasData = true;
                        continue;
                    }

                }
            }
        }

        if(hasData)
        {
            //cout<<i<<endl;
            hasData = false;
        }
    }

    //
    return 0;
}

int testStreamData(string server, string uuid, string dataName, long x, long y, long z, long sx, long sy, long sz)
{
    //
    long size = 4*sx*sy*sz; // in bytes
    unsigned char *p = NULL;

    //
    if(sz>1)
    {
        // 3D block data
        new1dp<unsigned char, long>(p, size);
        memset(p, 0, size);

        //
        string sizePath = std::to_string(sx*4);
        sizePath.append("_");
        sizePath.append(std::to_string(sy));
        sizePath.append("_");
        sizePath.append(std::to_string(sz));

        string offsetPath = std::to_string(x);
        offsetPath.append("_");
        offsetPath.append(std::to_string(y));
        offsetPath.append("_");
        offsetPath.append(std::to_string(z));

        // client
        http::uri_builder builder(U("/api/node/"));
        builder.append_path(uuid);
        builder.append_path(U("/"));
        builder.append_path(dataName);
        builder.append_path(U("/raw/0_1_2/"));
        builder.append_path(sizePath);
        builder.append_path(U("/"));
        builder.append_path(offsetPath);

        cout<<builder.to_string()<<endl;

        http::uri uri = http::uri(server.append(builder.to_string()));
        http_client client(uri);

        //
        concurrency::streams::producer_consumer_buffer<uint8_t> rwbuf;
        auto ostr = concurrency::streams::ostream(rwbuf);

        http_request msg(methods::GET);
        msg.set_response_stream(ostr);
        http_response rsp = client.request(msg).get();

        rsp.content_ready().get();

        cout<<"size of data: "<<rwbuf.in_avail()<<" - "<<rwbuf.size()<<endl;

        rwbuf.getn(p, rwbuf.in_avail()).get();

        for(int x=0; x<16; x++)
        {
            //cout<<((unsigned short*)p)[x]<<" ";
            cout<<int(p[x])<<" ";
        }
        cout<<endl;
    }
    else if(sz==1)
    {
        // 2D slice data

        //
        string sizePath = std::to_string(sx*4);
        sizePath.append("_");
        sizePath.append(std::to_string(sy));

        string offsetPath = std::to_string(x);
        offsetPath.append("_");
        offsetPath.append(std::to_string(y));
        offsetPath.append("_");
        offsetPath.append(std::to_string(z));

        // client
        http::uri_builder builder(U("/api/node/"));
        builder.append_path(uuid);
        builder.append_path(U("/"));
        builder.append_path(dataName);
        builder.append_path(U("/raw/0_1/"));
        builder.append_path(sizePath);
        builder.append_path(U("/"));
        builder.append_path(offsetPath);
        cout<<builder.to_string()<<endl;

        http::uri uri = http::uri(server.append(builder.to_string()));
        http_client client(uri);

        //
        concurrency::streams::producer_consumer_buffer<uint8_t> rwbuf;
        auto ostr = concurrency::streams::ostream(rwbuf);

        //
        Timer timer;
        timer.start();

        //
        http_request msg(methods::GET);
        msg.set_response_stream(ostr);
        http_response rsp = client.request(msg).get();

        rsp.content_ready().get();

        cout<<"size of data: "<<rwbuf.in_avail()<<" - "<<rwbuf.size()<<" costs: "<<timer.getEclipseTime()<<"ms"<<endl;

        long szbuf = rwbuf.in_avail();

        new1dp<unsigned char, long>(p, szbuf);
        memset(p, 0, szbuf);

        rwbuf.getn(p, szbuf).get();

        //
        const char* filename = "./tile2D.tif"; // .png

//        FILE *fp;
//        fp=fopen("./tile2Dtest.png", "wb");
//        fwrite(p, szbuf, szbuf, fp);
//        fclose(fp);

        std::vector<unsigned char> png;
        for(int i=0; i<szbuf; i++)
        {
            png.push_back(p[i]);
        }

        std::vector<unsigned char> image;
        unsigned w,h;
        unsigned error = lodepng::decode(image, w, h, png);
        if(error)
        {
            std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        }
        else
        {
            cout<<w<<" = 4*"<<sx<<", "<<h<<" = "<<sy<<", "<<szbuf<<" < "<<w*h<<endl;
        }

//        error = lodepng::encode(filename, image, w, h);
//        if(error)
//        {
//            std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
//        }

        //
        long szImage = w*h;
        unsigned char *pImg = NULL;
        new1dp<unsigned char, unsigned>(pImg, szImage);
        unsigned short *pIn = (unsigned short *)(pImg);

        for(long i=0; i<szImage; i+=2)
        {
            pImg[i] = image[i+1];
            pImg[i+1] = image[i];
        }

        unsigned short *pOut = NULL;
        new1dp<unsigned short, long>(pOut, 3*sx*sy);

        long offc = sx*sy;

        for(long j=0; j<sy; j++)
        {
            long offy = j*sx;
            for(long i=0; i<sx; i++)
            {
                pOut[offy+i] = pIn[2*offy + 2*i];
                pOut[offy+i+offc] = pIn[2*offy+2*i+1];
            }
        }

        // save output
        BioMedicalDataIO output;

        output.m_Data->setData((void *)pOut);
        output.m_Data->setDataType(USHORT);
        output.m_Data->size.setXYZCT(sx,sy,sz,3,1);
        output.m_Data->spacing.setXYZCT(0.25,0.25,1.0,1.0,1.0);
        output.writeData(filename);

//        //
//        for(int x=0; x<16; x++)
//            cout<<int(image[x])<<" ";
//        cout<<endl;

        //
        del1dp<unsigned char>(pImg);
        del1dp<unsigned short>(pOut);
    }
    else
    {
        cout<<"Invalid size!"<<endl;
        return -1;
    }

    //
    del1dp<unsigned char>(p);

    //
    return 0;
}

int testMultipleBlockStream(string server, string uuid, string dataName)
{
    BlockList blocks;

    // create simulated data

    // 128x128x128
    // 20736_17664_5100

    long sx = 8192;
    long sy = 1616;
    long sz = 624;

    long xoff = 20736;
    long yoff = 17664;
    long zoff = 5100;

    //
    for(int i=0; i<32; i++)
    {
        Block b;

        b.size = LongTuplet(128,128,128);
        b.offsets = LongTuplet(xoff + b.blockOffsets.getX()*4, yoff + b.blockOffsets.getY(), b.blockOffsets.getZ()); // uint16 + 2 colors
        b.blockOffsets = LongTuplet(128 * i, i*48, i*16);

        blocks.push_back(b);
    }

    long n = blocks.size() - 1;

    for(int i=0; i<32; i++)
    {
        Block b;

        b.size = LongTuplet(128,128,128);
        b.offsets = LongTuplet(blocks[n].offsets.getX() + 128*4, blocks[n].offsets.getY() - 32, blocks[n].offsets.getZ() - 12);
        b.blockOffsets = LongTuplet(blocks[n].blockOffsets.getX() + 128, blocks[n].blockOffsets.getY() - 32, blocks[n].blockOffsets.getZ() - 12);

        n++;

        blocks.push_back(b);
    }

    cout<<blocks.size()<<endl;

    //
    long size = 4*128*128*128; // in bytes
    unsigned char *p = NULL;
    new1dp<unsigned char, long>(p, size);

    //
    long chnsize = sx*2*sy*sz; // uint16
    long sizeBuffer = chnsize; // 1st color channel
    unsigned char *pBuffer = NULL;
    new1dp<unsigned char, long>(pBuffer, sizeBuffer);
    memset(pBuffer, 0, sizeBuffer);

    //
    Timer timer;
    timer.start();

    string dvidserver;

    //
    for(int iblk=0; iblk<blocks.size(); iblk++)
    {
        //
        dvidserver = server;

        //
        string sizePath = std::to_string(128*4);
        sizePath.append("_");
        sizePath.append(std::to_string(128));
        sizePath.append("_");
        sizePath.append(std::to_string(128));

        string offsetPath = std::to_string(blocks[iblk].offsets.getX());
        offsetPath.append("_");
        offsetPath.append(std::to_string(blocks[iblk].offsets.getY()));
        offsetPath.append("_");
        offsetPath.append(std::to_string(blocks[iblk].offsets.getZ()));

        // client
        http::uri_builder builder(U("/api/node/"));
        builder.append_path(uuid);
        builder.append_path(U("/"));
        builder.append_path(dataName);
        builder.append_path(U("/raw/0_1_2/"));
        builder.append_path(sizePath);
        builder.append_path(U("/"));
        builder.append_path(offsetPath);

        http::uri uri = http::uri(dvidserver.append(builder.to_string()));
        http_client client(uri);

        cout<<uri.to_string()<<endl;

        //
        memset(p, 0, size);

        //
        concurrency::streams::producer_consumer_buffer<uint8_t> rwbuf;
        auto ostr = concurrency::streams::ostream(rwbuf);

        http_request msg(methods::GET);
        msg.set_response_stream(ostr);
        http_response rsp = client.request(msg).get();

        rsp.content_ready().get();

        cout<<"size of data: "<<rwbuf.in_avail()<<" - "<<rwbuf.size()<<endl;

        rwbuf.getn(p, rwbuf.in_avail()).get();

        //
        unsigned short *pData = (unsigned short*)(pBuffer);
        unsigned short *pIn = (unsigned short*)(p);

        //
        //cout<<iblk<<" "<<blocks[iblk].box + 128 << " " << blocks[iblk].boy + 128 << " " <<blocks[iblk].boz + 128 <<" "<<pIn[15]<<endl;

        //
        for(long k=0; k<128; k++)
        {
            long offk = (k+blocks[iblk].blockOffsets.getZ())*sx*sy;
            long offz = k*128*128*2;
            for(long j=0; j<128; j++)
            {
                long offj = offk + (j+blocks[iblk].blockOffsets.getY())*sx;
                long offy = offz + j*128*2;
                for(long i=0; i<128; i++)
                {
                    pData[offj + i + blocks[iblk].blockOffsets.getX()] = pIn[offy + i*2];
                }
            }
        }

        //
        cout<<"save 1st color to the buffer"<<endl;

        //
        rwbuf.close();

    }

    cout<<"download data from the server in "<<timer.getEclipseTime()<<" ms. "<<endl;

    //
    unsigned short *pData = (unsigned short*)(pBuffer);
    for(long j=0; j<sx*sy; j++)
    {
        for(long k=0; k<sz; k++)
        {
            if( pData[j] < pData[k*sx*sy + j])
            {
                pData[j] = pData[k*sx*sy + j];
            }
        }
    }

    //
    BioMedicalDataIO output;

    output.m_Data->setData((void *)pBuffer);
    output.m_Data->setDataType(USHORT);
    output.m_Data->size.setXYZCT(sx,sy,1,1,1);
    output.m_Data->spacing.setXYZCT(0.25,0.25,1.0,1.0,1.0);
    output.writeData(string("./testMultipleBlockStream.tif"));

    //
    del1dp<unsigned char>(p);

    //
    del1dp<unsigned char>(pBuffer);

    //
    return 0;
}



