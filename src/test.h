// test.h
// manage mouse light data with dvid
// developed by Yang Yu (gnayuy@gmail.com)

#ifndef __DATAMANAGEMENT_TEST_H__
#define __DATAMANAGEMENT_TEST_H__

#include "datamanagement.h"

//
int testReadWriteData(string outFileName, string ch1, string ch2);

//
int testSplitData();

//
int testOctreePath(string tilesFile, string octreePath, long sx, long sy, long sz);

//
int testBufferMap(string tilesFile, int branch);

//
int testStreamData(string server, string uuid, string dataName, long x, long y, long z, long sx, long sy, long sz);

//
int testMultipleBlockStream(string server, string uuid, string dataName);


#endif // __DATAMANAGEMENT_TEST_H__
