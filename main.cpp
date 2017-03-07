#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

struct endpoint
{
    int Ld, K;
    std::map<int, int> servers;
};

struct request
{
    int Rv, Re, Rn;
    bool operator < (const request& str) const      //coś potrzebne do sortowania
    {
        return (Rn < str.Rn);
    }
};

class Cache
{
public:
    int _capacity;
    std::map<int, int> _endpoints;
    std::vector<int> _video_index;
};

void add_video_to_cache(int id, int size, Cache & cache)
{
    if (size < cache._capacity && std::find(cache._video_index.begin(), cache._video_index.end(), id) == cache._video_index.end())
    {
        cache._video_index.push_back(id);
        cache._capacity -= size;
    }
}


int main()
{
    int V, E, R, C, X;
    std::vector<int> videos;
    std::vector<endpoint> endpoints;
    std::vector<request> requests;
    std::fstream data("kittens.in", std::ios_base::in);
    std::string line, number;
    if (data.is_open())
    {
        data >> V;
        data >> E;
        data >> R;
        data >> C;
        data >> X;
        videos.resize(V);
        endpoints.resize(E);
        requests.resize(R);
        for (int i = 0; i < V; i++) data >> videos[i];  // wczytujemy rozmiary wideo
        for (int j = 0; j < E; j++)                     // wczytujemy dane endpointów
        {
            data >> endpoints[j].Ld;
            data >> endpoints[j].K;
            int c, Lc;
            for (int k = 0; k < endpoints[j].K; k++)
            {
                data >> c;
                data >> Lc;
                endpoints[j].servers[c] = Lc;
            }
        }
        for (int i = 0; i < R; i++)                         //wczytujemy requesty
        {
            data >> requests[i].Rv;
            data >> requests[i].Re;
            data >> requests[i].Rn;
        }
        data.close();
    }
    /*std::cout << V << " " << E << " " << R << " " << C << " " << X << std::endl;
    for (int j = 0; j < V; j++) std::cout << videos[j] << " ";

    for (int j = 0; j < E; j++)
    {
        std::cout << endpoints[j].Ld << " " << endpoints[j].K << " " << std::endl;
        for (std::pair<int, int> p : endpoints[j].servers)
        {
            std::cout << p.first << " " << p.second << std::endl;
        }
    }

    for (auto el : requests)
    {
        std::cout << el.Rv << " " << el.Re << " " << el.Rn << std::endl;
    }
*/
//Sortujemy wektor requestów względem Rn
    std::sort(requests.begin(), requests.end());
    /*for (auto el : requests)
    {
        std::cout << el.Rv << " " << el.Re << " " << el.Rn << std::endl;
    }*/

    //Tworzymy wektor serwerów cache 
    std::vector<Cache> cache_servers(C);
    for (int i = 0; i < E; i++)
    {
        for (std::map<int, int>::iterator it = endpoints[i].servers.begin(); it != endpoints[i].servers.end(); it++)
        {
            cache_servers[it->first]._capacity = X;
            cache_servers[it->first]._endpoints[i] = it->second;
        }
    }

    //Dodajemy filmy do cachów
    int lat_min, id_min;
    //std::map<int, int>::iterator it_first;
    for (int i = 0; i < R; i++)
    {
        id_min = endpoints[requests[i].Re].servers.begin()->first;      //to trzeba poprawić!!! - kontrola pełności cache
        lat_min = endpoints[requests[i].Re].servers.begin()->second;
        for (std::map<int, int>::iterator it = endpoints[requests[i].Re].servers.begin(); it != endpoints[requests[i].Re].servers.end(); it++)
        {
            if (it->second < lat_min && cache_servers[it->first]._capacity > videos[requests[i].Rv])
            {
                id_min = it->first;
                lat_min = it->second;
            }
        }
        add_video_to_cache(requests[i].Rv, videos[requests[i].Rv], cache_servers[id_min]);
    }

    //Zliczamy zajęte serwery
    int count_servers = 0;
    for (auto c : cache_servers)
    {
        if (c._capacity < X) count_servers++;
    }

    std::cout << count_servers << std::endl;
    for (int i = 0; i < C; i++)
    {
        if (cache_servers[i]._capacity < X)
        {
            std::cout << i;
            for (int j = 0;j < cache_servers[i]._video_index.size(); j++)
            {
                std::cout << " " << cache_servers[i]._video_index[j];
            }
            std::cout << "\n";
        }
    }

    std::ofstream outcome("outcome_zoo.txt");

        outcome << count_servers << "\n";
        for (int i = 0; i < C; i++)
        {
            if (cache_servers[i]._capacity < X)
            {
                outcome << i;
                for (int j = 0;j < cache_servers[i]._video_index.size(); j++)
                {
                    outcome << " " << cache_servers[i]._video_index[j];
                }
                outcome << "\n";
            }
        }
        outcome.close();
    return 0;
}