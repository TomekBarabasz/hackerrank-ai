#include <vector>
#include <stdint.h>
#include <stdexcept>
#include <functional>
#include <string>
#include <cstring>

namespace Click_o_mania
{
template<typename T>
struct BlockList
{
    using Point = std::tuple<T,T>;
    using Group = std::tuple<int16_t,int16_t,int16_t>;
    int16_t npoints,ngroups;
    const size_t Capacity;
    const bool Delete;
    Point *points;
    Group *groups;
    BlockList(size_t capacity) : Capacity(capacity), Delete(true)
    {
        points = new Point[Capacity];
        groups = new Group[Capacity];
        clear();
    }
    BlockList(size_t capacity,uint8_t*raw_ptr) : Capacity(capacity), Delete(false)
    {
        points = reinterpret_cast<Point*>(raw_ptr);
        groups = reinterpret_cast<Group*>(raw_ptr + capacity*sizeof(Point));
        clear();
    }
    void clear()
    {
        npoints = ngroups = 0;
    }
    ~BlockList()
    {
        if (Delete) {
            delete[] points;
            delete[] groups;
        }
    }
};
struct SearchState
{
    int16_t nrow,ncol;
    uint8_t *grid;
    BlockList<int8_t> blocks;

    static SearchState* alloc(int nrow,int ncol)
    {
        const size_t size = nrow*ncol;        
        const int additional_size = size*sizeof(uint8_t) 
                                  + size*sizeof(decltype(blocks)::Point)
                                  + size*sizeof(decltype(blocks)::Group);
        uint8_t *raw = new uint8_t[sizeof(SearchState) + additional_size];
        auto *ss = new(raw) SearchState(nrow,ncol,raw+sizeof(SearchState));
        return ss;
    }
    SearchState(int nrow,int ncol, uint8_t* raw) : nrow(nrow),ncol(ncol), blocks(nrow*ncol,raw)
    {
        const int size = nrow*ncol;
        grid = reinterpret_cast<uint8_t*>(raw + size*sizeof(decltype(blocks)::Point)
                                             + size*sizeof(decltype(blocks)::Group));
    }
    ~SearchState()
    {
    }
};
template<typename T>
struct Pool
{
    std::vector<T*> pool;
    ~Pool()
    {
        for (auto p:pool) {
            delete p;
        }
    }
    T* allocate(std::function<T*()> make)
    {
        if (!pool.empty())
        {
            T* e = pool.back();
            pool.pop_back();
            return e;
        } 
        else {
            return make();
        } 
    }
    void release(T* elem){
        pool.push_back(elem);
    }
};
template <typename T>
struct Ring
{
    const int Capacity;
    const bool Delete;
    using value_type=T;
    T* array;
    int first,nelem;
    Ring(int capacity) : Capacity(capacity),Delete(true)
    {
        array = new T[capacity];
        clear();
    }
    Ring(int capacity,T*array_) : Capacity(capacity),Delete(false)
    {
        array = array_;
        clear();
    }
    ~Ring() 
    { 
        if (Delete){
            delete[] array;
        }
    }
    void clear()
    {
        first = nelem = 0;
    }
    bool push(const T&& elem)
    {
        if (nelem >= Capacity) return false;
        auto next = first + nelem;
        if (next >= Capacity) next -= Capacity;
        array[next] = elem;
        nelem += 1;
        return true;
    }
    T popfirst()
    {
        if (0==nelem) throw std::out_of_range("popfirst from empty ring");
        T& e = array[first];
        first += 1;
        if (first >= Capacity) first = 0;
        nelem -= 1;
        return e;
    }
    bool empty() const
    {
        return 0==nelem;
    }
    size_t size() const { return nelem; }
};
using Int8Ring = Ring<std::tuple<int8_t,int8_t>>;
struct Workspace
{
    const int Nrow,Ncol;
    const bool Delete;
    uint8_t *group_map;
    int16_t *group_count;
    Int8Ring ring;
    Pool<SearchState> ss_pool;
    Workspace(int nrow,int ncol) : Nrow(nrow),Ncol(ncol), ring(nrow*ncol), Delete(true)
    {
        const int size = nrow*ncol;
        group_map = new uint8_t[size];
        group_count = new int16_t[size];
    }
    Workspace(int nrow,int ncol,uint8_t*raw_ptr) : 
        Nrow(nrow),Ncol(ncol), 
        ring(nrow*ncol,reinterpret_cast<Int8Ring::value_type*>(raw_ptr)), 
        Delete(false)
    {
        const int size = nrow*ncol;
        group_map = reinterpret_cast<uint8_t*>(raw_ptr + size * sizeof(Int8Ring::value_type));
        group_count = reinterpret_cast<int16_t*>(group_map + size * sizeof(int8_t));
    }
    ~Workspace()
    {
        if (Delete){
            delete[] group_map;
            delete[] group_count;
        }
    }
    static Workspace* make(int nrow,int ncol)
    {
        const int totSize = nrow*ncol;
        int additional_size = sizeof(uint8_t)*totSize    //group_map
                            + sizeof(int16_t)*(totSize+1)
                            + sizeof(Int8Ring::value_type)*totSize;
        uint8_t *raw = new uint8_t[sizeof(Workspace)+additional_size];
        Workspace *wrk = new(raw) Workspace(nrow,ncol,raw+sizeof(Workspace));
        return wrk;
    }
    SearchState* allocSearchState()
    {
        return ss_pool.allocate([nrow=Nrow,ncol=Ncol](){return SearchState::alloc(nrow,ncol);});
    }
    void releaseSearchState(SearchState *pss)
    {
        ss_pool.release(pss);
    }
};
template<typename T>
struct Ref
{
    T *ptr;
    Ref(T*ptr) : ptr(ptr) {}
    ~Ref() { delete ptr;ptr=nullptr;}
    operator T() { return *ptr;}
    T& get() { return *ptr;}

};
constexpr uint8_t EmptySpace = '-';
SearchState* makeGrid(std::initializer_list<std::string> init);
void printGrid(const SearchState& ss);
int partitionGrid(const SearchState& ss,Workspace& wrk);
void collectGroups(int nrow,int ncol,int groups,Workspace& wrk, BlockList<int8_t>& blocks);
void sortGroups(BlockList<int8_t>& blocks);
SearchState* removeGroup(SearchState& ss, int group_idx, Workspace& wrk);
bool updateColumn(SearchState& ss,int icol);
void removeColumn(SearchState& ss, int icol);
void removeEmptyRows(SearchState& ss);
std::tuple<int8_t,int8_t> solve(std::initializer_list<std::string> init);
}

