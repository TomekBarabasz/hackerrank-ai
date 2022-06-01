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
    std::tuple<T,T> *points;
    Group *groups;
    BlockList(size_t capacity) : Capacity(capacity)
    {
        points = new Point[Capacity];
        groups = new Group[Capacity];
        npoints= 0;
        ngroups= 0;
    }
    ~BlockList()
    {
        delete[] points;
        delete[] groups;
    }
};
struct SearchState
{
    int16_t nrow,ncol;
    int8_t    *grid;
    BlockList<int8_t> *blocks;

    static SearchState* makeInstance(const int8_t*grid,int nrow,int ncol)
    {
        const size_t size = nrow*ncol;
        auto *ss = new SearchState(nrow,ncol);
        memcpy(ss->grid, grid, size*sizeof(int8_t));
        return ss;
    }
    SearchState(int nrow,int ncol) : nrow(nrow),ncol(ncol)
    {
        grid = new int8_t[nrow*ncol];
    }
    ~SearchState()
    {
        //delete[] reinterpret_cast<int8_t*>(this);
        delete[] grid;
    }
};
template<typename T>
struct Pool
{
    std::vector<T> pool;
    T* allocate(std::function<T*()> make)
    {
        if (!pool.empty()){
            T* e = pool.last();
            pool.pop_back();
            return e;
        } else {
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
    int8_t *group_map;
    int16_t *group_count;
    Int8Ring ring;
    Workspace(int nrow,int ncol) : Nrow(nrow),Ncol(ncol), ring(nrow*ncol), Delete(true)
    {
        const int size = nrow*ncol;
        group_map = new int8_t[size];
        group_count = new int16_t[size];
    }
    Workspace(int nrow,int ncol,uint8_t*raw_ptr) : 
        Nrow(nrow),Ncol(ncol), 
        ring(nrow*ncol,reinterpret_cast<Int8Ring::value_type*>(raw_ptr)), 
        Delete(false)
    {
        const int size = nrow*ncol;
        group_map = reinterpret_cast<int8_t*>(raw_ptr + size * sizeof(Int8Ring::value_type));
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
        int additional_size = sizeof(int8_t)*totSize    //group_map
                            + sizeof(int16_t)*totSize
                            + sizeof(Int8Ring::value_type)*totSize;
        uint8_t *raw = new uint8_t[sizeof(Workspace)+additional_size];
        Workspace *wrk = new(raw) Workspace(nrow,ncol,raw+sizeof(Workspace));
        return wrk;
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
constexpr int8_t EmptySpace = ' ';
SearchState makeGrid(std::initializer_list<std::string> init);
int partitionGrid(SearchState& ss,Workspace& wrk);
int collectGroups(int nrow,int ncol,const int8_t*group_map, BlockList<int8_t>& blocks);
}

