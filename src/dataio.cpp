// dataio.cpp
// read/write .tif, .json, ...
// developed by Yang Yu (gnayuy@gmail.com)

#include "dataio.h"

// metainfo
MetaInfo::MetaInfo()
{
}

MetaInfo::~MetaInfo()
{
}

// tile
Tile::Tile()
{
    visited = false;
}

Tile::~Tile()
{
}

/// class Timer
//Timer::Timer()
//{
//    m_startTime.tv_sec = 0;
//    m_startTime.tv_usec = 0;
//}

//void Timer::start()
//{
//    gettimeofday(&m_startTime, 0);
//}

//double Timer::getEclipseTime() const
//{
//    struct timeval currentTime;
//    gettimeofday(&currentTime, 0);

//    double const dsecs = currentTime.tv_sec - m_startTime.tv_sec;
//    double const dus = currentTime.tv_usec - m_startTime.tv_usec;

//    return (dsecs * 1000.0) + (dus / 1000.0);
//}
