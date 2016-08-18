// test write meta info to json file
// Yang Yu (gnayuy@gmail.com)

// g++ -std=c++11 -o jwrite jwrite.cpp -L/usr/local/lib -lcpprest -lboost_system -lboost_chrono -lboost_thread -lboost_random -lboost_regex -lssl -lcrypto

#include "metainfo.h"

void printJSONValue(json::value v, string indent)
{
    string indented=indent;
    cout << indent << "{" <<endl;

    if (!v.is_null())
    {
        indented.append("    ");

        //
        for (auto iter = std::begin(v.as_object()); iter != std::end(v.as_object()); ++iter)
        {
            //
            auto key = iter->first;
            auto& value = iter->second;

            if (value.is_object() || value.is_array())
            {
                ucout << indented << key << U(" : ") << endl;
                printJSONValue(value, indented);
            }
            else
            {
                //
                ucout << indented << key << U(" : ");

                if(value.is_double())
                {
                    cout.precision(numeric_limits<double>::digits10 + 1);
                    cout << value.as_double();
                }
                else if(v.is_integer())
                {
                     cout << value.as_integer();
                }
                else
                {
                    ucout << value.serialize();
                }
                cout << endl;
            }
        }
    }

    cout<< indent <<"}"<<endl;
}

int main(int argc, char* argv[])
{
    //
    MetaInfo mi;
    
    mi.name = "2015-07-11-johan";
    mi.unit = "micrometer";
    
    mi.voxelsize_x = 0.25002763671875;
    mi.voxelsize_y = 0.25155907495471014;
    mi.voxelsize_z = 1.0010404411764706;
    
    mi.origin_x = 72739.042;
    mi.origin_y = 41644.696;
    mi.origin_z = 14966.719;
    
    mi.dim_x = 640;
    mi.dim_y = 552;
    mi.dim_z = 204;
    
    mi.size_x = 16001.76875;
    mi.size_y = 16099.780797101448;
    mi.size_z = 64066.58823529412;
    
    mi.bits_per_voxel = 16;
    mi.number_of_channels = 2;
    
    //
    json::value j;
    
    j[U("name")] = json::value(mi.name);
    j[U("unit")] = json::value(mi.unit);
    
    // {{"vx", mi.voxelsize_x}, {"vy", mi.voxelsize_y}, {"vz", mi.voxelsize_z}}
    //utility::stringstream_t ssVoxelSize;
    //ssVoxelSize << U("{\"vx\":") << mi.voxelsize_x << U(", \"vy\":") << mi.voxelsize_y << U(", \"vz\":") << mi.voxelsize_z << U("}");
    //cout<< ssVoxelSize.str() << endl;
    //j[U("voxelsize")] = json::value::parse(ssVoxelSize);

    j[U("voxelsize")][U("vx")] = json::value(mi.voxelsize_x);
    j[U("voxelsize")][U("vy")] = json::value(mi.voxelsize_y);
    j[U("voxelsize")][U("vz")] = json::value(mi.voxelsize_z);

    // {{"ox", mi.origin_x}, {"oy", mi.origin_y}, {"oz", mi.origin_z}}
    //utility::stringstream_t ssOrigin;
    //ssOrigin << U("{\"ox\":") << mi.origin_x << U(", \"oy\":") << mi.origin_y << U(", \"oz\":") << mi.origin_z << U("}");
    //cout<< ssOrigin.str() << endl;
    //j[U("origin")] = json::value::parse(ssOrigin);

    j[U("origin")][U("ox")] = json::value(mi.origin_x);
    j[U("origin")][U("oy")] = json::value(mi.origin_y);
    j[U("origin")][U("oz")] = json::value(mi.origin_z);

    // {{"ox", mi.dim_x}, {"oy", mi.dim_y}, {"oz", mi.dim_z}}
    //utility::stringstream_t ssDimension;
    //ssDimension << U("{\"dx\":") << mi.dim_x << U(", \"dy\":") << mi.dim_y << U(", \"dz\":") << mi.dim_z << U("}");
    //cout<< ssDimension.str() << endl;
    //j[U("dimension")] = json::value::parse(ssDimension);

    j[U("dimension")][U("dx")] = json::value(mi.dim_x);
    j[U("dimension")][U("dy")] = json::value(mi.dim_y);
    j[U("dimension")][U("dz")] = json::value(mi.dim_z);

    // {{"ox", mi.size_x}, {"oy", mi.size_y}, {"oz", mi.size_z}}
    //utility::stringstream_t ssSize;
    //ssSize << U("{\"sx\":") << mi.size_x << U(", \"sy\":") << mi.size_y << U(", \"sz\":") << mi.size_z << U("}");
    //cout<< ssSize.str() << endl;
    //j[U("size")] = json::value::parse(ssSize);

    j[U("size")][U("sx")] = json::value(mi.size_x);
    j[U("size")][U("sy")] = json::value(mi.size_y);
    j[U("size")][U("sz")] = json::value(mi.size_z);
    
    j[U("bits")] = json::value(mi.bits_per_voxel);
    
    j[U("channels")] = json::value(mi.number_of_channels);
   
    //
//    cout.precision(numeric_limits<double>::digits10 + 1);
//    cout<<j.serialize()<<endl;

    printJSONValue(j, "");
    
    //
    return 0;
}
