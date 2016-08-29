// datamanagement.cpp
// manage mouse light data with dvid
// developed by Yang Yu (gnayuy@gmail.com)

#include "datamanagement.h"

// class image
Image::Image()
{
    p = NULL;
}

Image::~Image()
{
    if(p)
    {
        delete []p;
        p = NULL;
    }
}

void Image::getOffset(string octreepath, double ox, double oy, double oz, double vx, double vy, double vz, long dimx, long dimy, long dimz, double &offx, double &offy, double &offz)
{

}

void Image::setOrigin(double x, double y, double z)
{

}

void Image::setDimension(long x, long  y, long z)
{

}

void Image::setResolution(double x, double y, double z)
{

}

// class DataManager
DataManager::DataManager()
{
    m_Data = NULL;
}

DataManager::~DataManager()
{
    clearData();
}

void DataManager::clearData()
{
    del1dp<unsigned char>(m_Data);
}

int DataManager::upstreaming(http_client client, uri_builder builder, unsigned char *buffer, long bx, long by, long bz, long sx, long sy, long sz, long bufSizeX, long bufSizeY, long bufSizeZ)
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
                        unsigned char val = buffer[offy+i];
                        p[oy+i] = val;
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

int DataManager::putData(tileListType tiles, utility::string_t server, utility::string_t uuid, utility::string_t dataName)
{
    // client
    http::uri uri = http::uri(server);
    http_client client(uri);

    http::uri_builder builder(U("/api/node/"));
    builder.append_path(uuid);
    builder.append_path(U("/"));
    builder.append_path(dataName);
    builder.append_path(U("/raw/0_1_2/640_736_544/"));

    cout<<builder.to_string()<<endl;

    //
    Tile t = tiles[0];
    cout<<t.octreepath<<" "<<std::boolalpha<<t.visited<<endl;

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
                                loadTile(buf, tiles[n].ch1, tiles[n].ch2, bx, by, bz, bufX, bufY, bufZ);
                            }

                        }
                    }
                }

                // upstreaming
                if(buffer!=NULL)
                {
                    //
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
                                if(upstreaming(client, queryPath, buf, bx, by, bz, csx, csy, csz, bufX, bufY, bufZ))
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

int DataManager::loadTile(unsigned char *&p, string ch1, string ch2, long bx, long by, long bz, long bufSizeX, long bufSizeY, long bufSizeZ)
{
    cout<<ch1<<" "<<bx<<" "<<by<<" "<<bz<<endl;

    // load ch1 and ch2
    TiffIO tiff1, tiff2;

    if(tiff1.canReadFile(const_cast<char*>(ch1.c_str())))
    {
        tiff1.read();
    }
    else
    {
        std::cout<<"Fail to read tiff image "<<ch1<<"."<<std::endl;
        return -1;
    }

    if(tiff2.canReadFile(const_cast<char*>(ch2.c_str())))
    {
        tiff2.read();
    }
    else
    {
        std::cout<<"Fail to read tiff image "<<ch2<<"."<<std::endl;
        return -1;
    }

    long x,y,z;
    long sx = tiff2.getDimx();
    long sy = tiff2.getDimy();
    long sz = tiff2.getDimz();
    int datatype = tiff2.getDataType();

    if(tiff1.getDimx()!=sx || tiff1.getDimy()!=sy || tiff1.getDimz()!=sz || tiff1.getDataType()!=datatype)
    {
        cout<<"Inconsistent tiff images with channel1 "<<ch1<<" and channel 2 "<<ch2<<endl;
        return -1;
    }

    // interleaved RG copy to p
    if(datatype==USHORT)
    {
        //
        unsigned short *pData = (unsigned short*)p;
        unsigned short *pCh1 = (unsigned short*)(tiff1.getData());
        unsigned short *pCh2 = (unsigned short*)(tiff2.getData());

        // buffer size designed for byte data
        bufSizeX /= 2;
        bufSizeY /= 2;
        bufSizeZ /= 2;

        //
        for(z=0; z<sz; z++)
        {
            long offz = z*sy*sx;
            long oz = z*bufSizeX*bufSizeY;
            for(y=0; y<sy; y++)
            {
                long offy = offz + y*sx;
                long oy = oz + y*bufSizeX;
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

int DataManager::findNode(tileListType tiles, long xoff, long yoff, long zoff)
{
    //
    long size = tiles.size();
    for(long t=0; t<size; t++)
    {
        if(tiles[t].visited)
            continue;

        //
        if(tiles[t].offTileX==xoff && tiles[t].offTileY==yoff && tiles[t].offTileZ==zoff)
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

    cout<<"save image as "<<outFileName<<endl;
    cout<<"image size: "<<sx<<" "<<sy<<" "<<sz<<" "<<sc<<endl;
    cout<<"voxel size: "<<vsx<<" "<<vsy<<" "<<vsz<<endl;

    //
    TiffIO tif;

    tif.setResX(vsx);
    tif.setResY(vsy);
    tif.setResZ(vsz);

    tif.setDataType(dataType);

    //
    long chnsize = sx*sy*sz;
    long size = chnsize*3; // RG -> RGB

    // cxyz -> xyzc
    if(dataType==USHORT)
    {
        unsigned short *pData = (unsigned short*)(m_Data);
        unsigned short *p = NULL;

        new1dp<unsigned short, long>(p,size);

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
                    p[sc*chnsize + offj + i] = 0;
                }
            }
        }

        cout<<"Data copied (cxyz -> xyzc)"<<endl;

        //
        tif.setDimx(sx);
        tif.setDimy(sy);
        tif.setDimz(sz);
        tif.setDimc(1); // tif.setDimc(sc); // test the first channel
        tif.setDimt(1);

        //
        tif.setData((void*)p);
        tif.setFileName(const_cast<char*>(outFileName.c_str()));

        //
        tif.write();

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
    long size = 4*sx*sy*sz; // in bytes
    new1dp<unsigned char, long>(m_Data, size);
    rawptr_buffer<unsigned char> rawBuf(m_Data, size);
    //concurrency::streams::ostream stream(rawBuf);

    cout<<"create raw buffer with size "<<rawBuf.size()<<endl;

    // GET data
    httpGetAsync(client, builder, rawBuf).wait();

    cout<<"download data from the server"<<endl;

    // save as a tiff
    saveTile(outFileName, sx, sy, sz, sc, vsx, vsy, vsz, USHORT);

    cout<<"successful save the image"<<endl;

    //
    return 0;
}

// CLI
DEFINE_string(tiles, "", "a json file of the tile list");
DEFINE_string(server, "", "server address (url:port)");
DEFINE_string(uuid, "", "uuid");
DEFINE_string(name, "", "data name");
DEFINE_bool(methods, false, "GET/POST"); // GET(true)/POST(false, by default)
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

//
DEFINE_string(ch1, "", "1st color channel");
DEFINE_string(ch2, "", "2nd color channel");
DEFINE_bool(test, false, "test");
DEFINE_uint64(testOption, 0, "test option");

//
int testReadWriteData(string outFileName, string ch1, string ch2)
{
    //
    unsigned char *p = NULL;

    // load ch1 and ch2
    TiffIO tiff1, tiff2;

    if(tiff1.canReadFile(const_cast<char*>(ch1.c_str())))
    {
        tiff1.read();
    }
    else
    {
        std::cout<<"Fail to read tiff image "<<ch1<<"."<<std::endl;
        return -1;
    }

    if(tiff2.canReadFile(const_cast<char*>(ch2.c_str())))
    {
        tiff2.read();
    }
    else
    {
        std::cout<<"Fail to read tiff image "<<ch2<<"."<<std::endl;
        return -1;
    }

    //
    long x,y,z;
    long sx = tiff2.getDimx();
    long sy = tiff2.getDimy();
    long sz = tiff2.getDimz();
    int dataType = tiff2.getDataType();

    if(tiff1.getDimx()!=sx || tiff1.getDimy()!=sy || tiff1.getDimz()!=sz || tiff1.getDataType()!=dataType)
    {
        cout<<"Inconsistent tiff images with channel1 "<<ch1<<" and channel 2 "<<ch2<<endl;
        return -1;
    }

    // interleaved RG copy to p
    long szChannel = sx*sy*sz;
    long size = szChannel*6; // ushort with 2 colors
    new1dp<unsigned char, long>(p, size);

    //
    if(dataType==USHORT)
    {
        //
        unsigned short *pData = (unsigned short*)p;
        unsigned short *pCh1 = (unsigned short*)(tiff1.getData());
        unsigned short *pCh2 = (unsigned short*)(tiff2.getData());

        //
        for(z=0; z<sz; z++)
        {
            long offz = z*sy*sx;
            //long oz = z*sy*sx*3;
            for(y=0; y<sy; y++)
            {
                long offy = offz + y*sx;
                //long oy = oz + y*sx*3;
                for(x=0; x<sx; x++)
                {
                    long idx = offy + x;
                    //long ind = oy + 3*x;

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
    TiffIO tif;

    tif.setResX(0.25);
    tif.setResY(0.25);
    tif.setResZ(1.00);

    tif.setDataType(dataType);

    //
    tif.setDimx(sx);
    tif.setDimy(sy);
    tif.setDimz(sz);
    tif.setDimc(3);
    tif.setDimt(1);

    //
    tif.setData((void*)pOut);
    tif.setFileName(const_cast<char*>(outFileName.c_str()));

    //
    tif.write();

    //
    del1dp<unsigned char>(p);
    del1dp<unsigned char>(pOut);

    //
    return 0;
}

//
int testSplitData()
{
    //

    //
    return 0;
}

// main
int main(int argc, char *argv[])
{
    //
    gflags::SetUsageMessage("dataManager -tiles <tilelist.json> -server <url:port> -uuid <uuid> -name <dataname> -methods <GET/POST>");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

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
        // time ./src/datamanagement -tiles ../data/tileList.json -server http://localhost:8000 -uuid 09a -name grayscale

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
        dataManager.putData(tiles, FLAGS_server, FLAGS_uuid, FLAGS_name);
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
