// File for Cache
#include "vector"
#include "iostream"
#include "queue"
#include "map"
#include <utility>
#include<list>
using namespace std;

class cache 
{
    private:
        long cache_line_length;
        long cache_size;
        long lines;
        long element_size;
        long associativity;
        long set_length;
        map<long,list<long>> lru_caches;
        vector<vector<long>> cache_block;
        vector<bool> tags;

        void initialize_cache()
        {
            /*Initialises the cache*/
            lines = cache_size / cache_line_length;
            long elements_in_line = cache_line_length / element_size;
            if(lines > associativity)set_length = lines/associativity;
            else set_length = 1;
            cache_block.resize(lines, vector<long>(elements_in_line, 0));
            tags.resize(lines,0);

            // initalise pq for sets
            for(long i = 0 ; i <associativity; i++)
            {
                for(long j = 0 ; j < set_length; j++ )
                {
                    lru_caches[i].push_back(j);
                }
            }
        }
        void update_lru(long target_set, long line_idx)
        {
            /* updates the lru cache , getting the most recently used to the front*/
            for(auto it = lru_caches[target_set].begin(); it != lru_caches[target_set].end(); ++it)
            {
                if(*it == line_idx)
                {
                    lru_caches[target_set].splice(lru_caches[target_set].begin(), lru_caches[target_set], it);
                    break;
                }
            }
        }
        long replace_lru(long target_set)
        {
            /*choses the index to be replaced and performs the replacement mainitaing property of LRU*/
            long replacement_line;
            if(!lru_caches[target_set].empty())
            {
                replacement_line = lru_caches[target_set].back();
                lru_caches[target_set].pop_back();
                lru_caches[target_set].push_front(replacement_line);
            }
            return replacement_line;
        }
        void fetch_cache_line(long line,long addr)
        {
            /* fetch_cache_lines and places the address and neighbours at the line*/
            for(long j = 0 ; j < cache_block[line].size();j++)
            {
                cache_block[line][j] = addr+j;
            }
        }
        long get_set(long addr)
        {
            /*Gives the set a particular address belongs*/
            if (addr>cache_size) addr = addr%cache_size;
            return addr/(set_length*cache_line_length);
        }

    public:
        cache(long size, long line_length, long elem_size, long assoc) 
            : cache_size(size), cache_line_length(line_length), 
            element_size(elem_size), associativity(assoc)
        {
            initialize_cache();
        }
        bool find(long addr)
        {
            /*Finds address in cache, returns true for a hit and false for miss*/
            long target_set = get_set(addr);
            for(long i = target_set*set_length ; i < (target_set+1)*set_length;i++)
            {
                for(long j = 0 ; j < cache_block[i].size();j++)
                {
                    if(cache_block[i][j] == addr)
                    {
                        long update_idx = i/set_length;
                        update_lru(target_set,update_idx);
                        if(tags[i] ==0)
                        {
                            fetch_cache_line(i,addr);
                            tags[i]=1;
                            return false;
                        }
                        return true;
                    }
                }
            }
            long replacement_idx = replace_lru(target_set);
            long replacement_line  = (target_set*set_length) + replacement_idx;
            tags[replacement_line]=1;
            fetch_cache_line(replacement_line,addr);
            return false;
        }
        void display_cache()
        {
            /*displays the cache*/
            for(long i = 0 ; i < cache_block.size(); i++)
            {
                for(long j = 0; j <cache_block[i].size();j++)
                {
                    cout<<cache_block[i][j]<< " ";
                }
                cout<<endl;
            } 
        }
};