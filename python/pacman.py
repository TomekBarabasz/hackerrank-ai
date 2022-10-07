from copy import copy
from queue import PriorityQueue
import argparse

#pac_r,pac_c, food_r,food_c, ROWS,COLS, grid = 
def loadMaze(maze):
    if maze==1:
        return 3,9, 5,1, 7,20,\
            ['%%%%%%%%%%%%%%%%%%%%',
            '%--------------%---%',
            '%-%%-%%-%%-%%-%%-%-%',
            '%--------P-------%-%',
            '%%%%%%%%%%%%%%%%%%-%',
            '%.-----------------%',
            '%%%%%%%%%%%%%%%%%%%%']
    if maze==2:
        return 11,9,2,15,13,20,\
            ['%%%%%%%%%%%%%%%%%%%%',
            '%----%--------%----%',
            '%-%%-%-%%--%%-%.%%-%',
            '%-%-----%--%-----%-%',
            '%-%-%%-%%--%%-%%-%-%',
            '%-----------%-%----%',
            '%-%----%%%%%%-%--%-%',
            '%-%----%----%-%--%-%',
            '%-%----%-%%%%-%--%-%',
            '%-%-----------%--%-%',
            '%-%%-%-%%%%%%-%-%%-%',
            '%----%---P----%----%',
            '%%%%%%%%%%%%%%%%%%%%']
    if maze==3:
        return 25,13,3,1,27,28,\
            ['%%%%%%%%%%%%%%%%%%%%%%%%%%%%',
            '%------------%%------------%',
            '%-%%%%-%%%%%-%%-%%%%%-%%%%-%',
            '%.%%%%-%%%%%-%%-%%%%%-%%%%-%',
            '%-%%%%-%%%%%-%%-%%%%%-%%%%-%',
            '%--------------------------%',
            '%-%%%%-%%-%%%%%%%%-%%-%%%%-%',
            '%-%%%%-%%-%%%%%%%%-%%-%%%%-%',
            '%------%%----%%----%%------%',
            '%%%%%%-%%%%%-%%-%%%%%-%%%%%%',
            '%%%%%%-%%%%%-%%-%%%%%-%%%%%%',
            '%%%%%%-%------------%-%%%%%%',
            '%%%%%%-%-%%%%--%%%%-%-%%%%%%',
            '%--------%--------%--------%',
            '%%%%%%-%-%%%%%%%%%%-%-%%%%%%',
            '%%%%%%-%------------%-%%%%%%',
            '%%%%%%-%-%%%%%%%%%%-%-%%%%%%',
            '%------------%%------------%',
            '%-%%%%-%%%%%-%%-%%%%%-%%%%-%',
            '%-%%%%-%%%%%-%%-%%%%%-%%%%-%',
            '%---%%----------------%%---%',
            '%%%-%%-%%-%%%%%%%%-%%-%%-%%%',
            '%%%-%%-%%-%%%%%%%%-%%-%%-%%%',
            '%------%%----%%----%%------%',
            '%-%%%%%%%%%%-%%-%%%%%%%%%%-%',
            '%------------P-------------%',
            '%%%%%%%%%%%%%%%%%%%%%%%%%%%%']

def getNeighbours(ROWS,COLS,r,c,grid):
    neigh = []
    if r>0: neigh.append( (r-1,c) )
    if c>0: neigh.append( (r,c-1) )
    if c<COLS-1: neigh.append( (r,c+1) )
    if r<ROWS-1: neigh.append( (r+1,c) )
    return [ n for n in neigh if grid[n[0]][n[1]]!='%' ]

def dfs_1(ROWS, COLS, pacman_r, pacman_c, food_r, food_c, grid):
    food = (food_r, food_c)
    pac = (pacman_r,pacman_c)
    visited = []
    path = []
    stack =[pac]
    shortest_dist = 1e6
    shortest_path = None
    while stack:
        p = stack.pop()
        assert( p not in path)
        path.append(p)
        if p not in visited:
            visited.append( p )
        if p != food:
            Ne = [n for n in getNeighbours(ROWS,COLS,p[0],p[1],grid) if n not in path and n not in stack]
            if Ne:
                stack.extend( Ne )
            else:
                path.pop()
        else:
            d = len(path)
            print('new path found, length',d)
            if d < shortest_dist:
                shortest_dist = d
                shortest_path = copy(path)
            path.pop()
    return shortest_path,visited

def dfs( ROWS, COLS, pacman_r, pacman_c, food_r, food_c, grid):
    food = (food_r, food_c)
    pac = (pacman_r,pacman_c)
    visited = []
    path = []
    stack =[pac]
    while stack:
        p = stack.pop()
        assert(p not in path)
        path.append(p)
        if p not in visited:
            visited.append( p )
        if p != food:
            Ne = [n for n in getNeighbours(ROWS,COLS,p[0],p[1],grid) if n not in path]
            if Ne:
                stack.extend( Ne )
            else:
                path.pop()
        else:
            return path,visited
    
    return path,visited

def bfs(ROWS, COLS, pacman_r, pacman_c, food_r, food_c, grid):
    food = (food_r, food_c)
    pac = (pacman_r,pacman_c)
    stack = [[pac]]
    visited = []
    while stack:
        path = stack.pop(0)
        p = path[-1]
        if p not in visited:
            visited.append( p )
        if p != food:
            Ne = [n for n in getNeighbours(ROWS,COLS,p[0],p[1],grid) if n not in path]
            if Ne:
                stack.extend( [path+[n] for n in Ne ] )
                continue
        else:
            return path,visited

def astar(ROWS, COLS, pacman_r, pacman_c, food_r, food_c, grid):
    def heuristic(start,end): 
        return abs(start[0]-end[0]) + abs(start[1]-end[1])
    food = (food_r, food_c)
    pac = (pacman_r,pacman_c)
    frontier = PriorityQueue()
    frontier.put( (0,[pac]) )
    visited = []
    while not frontier.empty():
        _,path = frontier.get_nowait()
        p = path[-1]
        d = len(path) + 1
        if p not in visited:
            visited.append(p)
        if p != food:
            for n in [n for n in getNeighbours(ROWS,COLS,p[0],p[1],grid) if n not in path]:
                priority = d + heuristic(n,food)
                frontier.put( (priority,path+[n]))
        else:
            return path,visited

def main(Args):
    pac_r,pac_c, food_r,food_c, ROWS,COLS, grid = loadMaze(Args.maze)
    types = { 'dfs':dfs, 'bfs':bfs, 'a*':astar}

    if not Args.random:
        path,visited = types[Args.type](ROWS, COLS, pac_r, pac_c, food_r, food_c, grid)
    print("total visited tiles",len(visited))
    print("path length",len(path)-1)
    if Args.path:
        for p in path:
            print(p[0],p[1])

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("type",type=str,choices=['dfs','bfs','a*'])
    parser.add_argument("-maze","-m", type=int,help="maze number", choices=[1,2,3])
    parser.add_argument("-iter","-i", type=int,help='number of random iterations')
    parser.add_argument("-random","-r", action='store_true', help='randomize start & end positions')
    parser.add_argument("-path","-p", action='store_true', help='display path')
    Args = parser.parse_args()
    main(Args)
