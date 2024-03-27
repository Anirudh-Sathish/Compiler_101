// Main file of cache emulator
#include "iostream"
#include "Cache/cache.cpp"

using namespace std;

int main()
{
    cache my_cache(1024,64,4,2);
    // my_cache.display_cache();
    int hit = 0, miss = 0;
    for(int i = 0 ; i < 2000;)
    {
        if(my_cache.find(i))hit++;
        else miss++;
        i += 18;
    }
    cout<<"HIT "<<hit<<endl;
    cout<<"MISS "<<miss<<endl;
    my_cache.display_cache();
    cout<<endl;

}