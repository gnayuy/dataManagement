
// g++ -o octreeoffset octreeoffset.cpp

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    std::string str = "3/1/6/2/2/1";
    
    if(argc > 1)
    {
        str = argv[1];
    }
    
    std::vector<int> vect;
    
    std::stringstream ss(str);
    
    int i;
    
    while (ss >> i)
    {
        vect.push_back(i);
        
        if (ss.peek() == '/')
            ss.ignore();
    }
    
    for (i=0; i< vect.size(); i++)
        std::cout << vect.at(i)<<std::endl;
    
    
    
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
    
    std::cout << ioff << " "<< joff <<" "<<koff <<std::endl;
    
    //
    return 0;
}