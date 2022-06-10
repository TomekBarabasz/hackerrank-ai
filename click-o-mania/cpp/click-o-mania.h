#include <vector>
#include <stdint.h>
#include <stdexcept>
#include <functional>
#include <string>
#include <cstring>

#define ALIGNED(x) __attribute__((aligned(x)))

namespace Click_o_mania
{
template<typename T>
struct Point_t 
{
    using value_type = T;
    T r,c;
    Point_t() : r(0),c(0) {}
    Point_t(int r,int c) : r ((T)r), c((T)c) {}
    bool operator==(const Point_t& other) const { return r==other.r && c==other.c;}
};
using Point = Point_t<int16_t>;

template<typename Point>
struct BlockList_t
{
    using Point_t = Point;
    using Group_t = std::tuple<int16_t,int16_t,int16_t>;
    int16_t npoints,ngroups;
    const size_t Capacity;
    const bool Delete;
    Point_t *points;
    Group_t *groups;
    BlockList_t(size_t capacity) : Capacity(capacity), Delete(true)
    {
        points = new Point_t[Capacity];
        groups = new Group_t[Capacity];
        clear();
    }
    BlockList_t(size_t capacity,uint8_t*raw_ptr) : Capacity(capacity), Delete(false)
    {
        points = reinterpret_cast<Point_t*>(raw_ptr);
        groups = reinterpret_cast<Group_t*>(raw_ptr + capacity*sizeof(Point_t));
        clear();
    }
    void clear()
    {
        npoints = ngroups = 0;
    }
    ~BlockList_t()
    {
        if (Delete) {
            delete[] points;
            delete[] groups;
        }
    }
};
using BlockList = BlockList_t<Point>;
template<typename Point,typename T>
struct SearchState_t
{
    int16_t nrow,ncol;
    T *grid;
    BlockList blocks;

    static SearchState_t* alloc(int nrow,int ncol)
    {
        const size_t size = nrow*ncol;        
        const int additional_size = size*sizeof(T) 
                                  + size*sizeof(Point)
                                  + size*sizeof(decltype(blocks)::Group_t);
        uint8_t *raw = new uint8_t[sizeof(SearchState_t) + additional_size];
        auto *ss = new(raw) SearchState_t(nrow,ncol,raw+sizeof(SearchState_t));
        return ss;
    }
    SearchState_t(int nrow,int ncol, uint8_t* raw) : nrow(nrow),ncol(ncol), blocks(nrow*ncol,raw)
    {
        const int size = nrow*ncol;
        grid = reinterpret_cast<T*>(raw + size*sizeof(BlockList::Point_t)
                                        + size*sizeof(BlockList::Group_t));
    }
    ~SearchState_t()
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
struct Ring_t
{
    const int Capacity;
    const bool Delete;
    using value_type=T;
    T* array;
    int first,nelem;
    Ring_t(int capacity) : Capacity(capacity),Delete(true)
    {
        array = new T[capacity];
        clear();
    }
    Ring_t(int capacity,T*array_) : Capacity(capacity),Delete(false)
    {
        array = array_;
        clear();
    }
    ~Ring_t() 
    { 
        if (Delete){
            delete[] array;
        }
    }
    void clear()
    {
        first = nelem = 0;
    }
    bool push_back(const T& elem)
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

struct Point2x
{
    Point arr[2];
};

using Ring = Ring_t<Point2x>;
using SearchState = SearchState_t<Point,uint8_t>;

struct Workspace
{
    using GroupMap_t = uint8_t;
    using GroupCount_t = int16_t;
    const int Nrow,Ncol;
    const bool Delete;
    GroupMap_t *group_map;
    GroupCount_t *group_count;
    Ring ring;
    Pool<SearchState> ss_pool;
    ALIGNED(32) Point::value_type directions[16];

    Workspace(int nrow,int ncol) : Nrow(nrow),Ncol(ncol), ring(nrow*ncol), Delete(true)
    {
        const int size = nrow*ncol;
        group_map = new uint8_t[size];
        group_count = new int16_t[size];
    }
    Workspace(int nrow,int ncol,uint8_t*raw_ptr) : 
        Nrow(nrow),Ncol(ncol), 
        ring(nrow*ncol,reinterpret_cast<Ring::value_type*>(raw_ptr)), 
        Delete(false)
    {
        const int size = nrow*ncol;
        group_map = reinterpret_cast<GroupMap_t*>(raw_ptr + size * sizeof(Ring::value_type));
        group_count = reinterpret_cast<GroupCount_t*>(group_map + calcGroupMapSize(size));
    }
    ~Workspace()
    {
        if (Delete){
            delete[] group_map;
            delete[] group_count;
        }
    }
    static constexpr size_t calcGroupMapSize(size_t nelem)
    {
        return ((sizeof(GroupMap_t)*nelem + 3)/4)*4;
    }
    static Workspace* make(int nrow,int ncol)
    {
        const int totSize = nrow*ncol;
        int additional_size = calcGroupMapSize(totSize)
                            + sizeof(GroupCount_t)*(totSize+1)
                            + sizeof(Ring::value_type)*totSize;
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
    void clearGroupMap(size_t nelem)
    {
        memset(group_map,0,sizeof(GroupMap_t)*nelem);
    }
    void clearGroupCount(size_t nelem)
    {
        memset(group_count,0,sizeof(GroupCount_t)*nelem);
    }
};

template<typename T>
void printIntGrid(const T* grid, int nrow, int ncol)
{
    for(int r=0;r<nrow;++r) {
        for (int c=0;c<ncol;++c) printf("%d",*grid++);
        printf("\n");
    }
}

constexpr uint8_t EmptySpace = '-';
SearchState* makeGrid(std::initializer_list<std::string> init);
void printGrid(const SearchState& ss);
int partitionGrid(const SearchState& ss,Workspace& wrk);
void collectGroups(int nrow,int ncol,int groups,Workspace& wrk, BlockList& blocks);
void sortGroups(BlockList& blocks);
SearchState* removeGroup(SearchState& ss, int group_idx, Workspace& wrk);
bool updateColumn(SearchState& ss,int icol);
void removeColumn(SearchState& ss, int icol);
void removeEmptyRows(SearchState& ss);
Point solve(std::initializer_list<std::string> init);
}

