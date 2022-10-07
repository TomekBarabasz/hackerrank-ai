import argparse
from copy import copy
from queue import PriorityQueue
from datetime import datetime

def pos2coord(idx,GridSize): return idx//GridSize,idx%GridSize
def coord2pos(r,c,GridSize): return r*GridSize+c
def getValidMoves(pos,grid,GridSize):
    mvs = []
    r,c = pos2coord(pos,GridSize)
    if r>0 : mvs.append( (coord2pos(r-1,c  ,GridSize),'u' ) )
    if c>0 : mvs.append( (coord2pos(r,  c-1,GridSize),'l' ) )
    if r<GridSize-1: mvs.append( (coord2pos(r+1,c  ,GridSize),'d') )
    if c<GridSize-1: mvs.append( (coord2pos(r,  c+1,GridSize),'r') )
    return mvs
def applyMove(grid,pos,newpos):
    tt = str.maketrans( {grid[pos]:grid[newpos], grid[newpos]:grid[pos]})
    return grid.translate(tt)

def loadPuzzle(args):
    if args.random is not None:
        import random
        random.seed( args.seed if args.seed is not None else 1)
        grid='012345678'
        GridSize=3
        pos=0
        Moves=[]
        visited = set(grid)
        while len(Moves) < args.random:
            np,mv = random.choice(getValidMoves(pos,grid,GridSize))
            ngrid = applyMove(grid,pos,np)
            if ngrid not in visited:
                grid = ngrid
                pos=np
                visited.add(grid)
                Moves.append(mv)
        print("random moves",Moves)
        return grid,GridSize
    
    if args.puzzle==1:
        return '038417265',3

def is_solved(grid):
    return all([n==chr(48+i) for i,n in enumerate(grid)])

def solvePuzzle_bfs(grid,GridSize,verbose):
    stack = [ (grid.index('0'),grid,[]) ]
    checked=set()
    while stack:
        pos,grid,path = stack.pop(0)
        verbose(grid)
        if is_solved(grid):
            return path
        else:
            if grid in checked:
                continue
            checked.add(grid)
            mvs=[]
            for np,mv in getValidMoves(pos,grid,GridSize):
                ngrid = applyMove(grid,pos,np)
                if ngrid not in checked:
                    stack.append( (np,ngrid,path+[mv]) )
                    mvs.append(mv)
            verbose(mvs)
    return None

def solvePuzzle_dfs(grid,GridSize,verbose):
    stack = [ (grid.index('0'),grid,[],set()) ]
    shortestPath = []
    shortestDist = 1e6
    while stack:
        pos,grid,path,checked = stack.pop(0)
        if is_solved(grid):
            d = len(path)
            if d < shortestDist:
                shortestDist = d
                shortestPath = copy(path)
        else:
            if grid in checked:
                continue
            checked.add(grid)
            Mvs = [mv for mv in getValidMoves(pos,grid,GridSize) if mv not in checked]
            stack.extend( [(np,applyMove(grid,pos,np),path+[mv],copy(checked)) for np,mv in Mvs] )
    return shortestPath

def solvePuzzle_astar(grid,GridSize,verbose):
    def heuristic(start,end): 
        return abs(start[0]-end[0]) + abs(start[1]-end[1])

    frontier = PriorityQueue()
    frontier.put( (0, (grid.index('0'),[grid],[]) ) )
    visited = set()
    while not frontier.empty():
        _,state = frontier.get_nowait()
        pos,grid,path = state
        if grid not in visited:
            visited.append(grid)
        if not is_solved(grid):
            Mvs = getValidMoves(pos,grid,GridSize)
            #for n in [n for n in getNeighbours(ROWS,COLS,p[0],p[1],grid) if n not in path]:
            #    priority = d + heuristic(n,food)
            #    frontier.put( (priority,path+[n]))
        else:
            return path,visited

def makeVerbose(verbose):
    if verbose == 'none':
        def dummy(string): pass
        return dummy
    if verbose == 'con':
        def toconsole(string): print(string)
        return toconsole
    else:
        fp = open(verbose,'w')
        def tofile(string) : fp.write(string+'\n')
        return tofile

def main(Args):
    grid,GridSize = loadPuzzle(Args)
    print("starting position",grid)
    verbose = makeVerbose(Args.verbose)
    Types = {'bfs':solvePuzzle_bfs, 'dfs':solvePuzzle_dfs, 'a*':solvePuzzle_astar}
    t0 = datetime.now()
    moves = Types[Args.type](grid,GridSize,verbose)
    dt = datetime.now() - t0
    print("completed in",dt)
    print("moves",len(moves))
    if Args.moves:
        M={'u':'UP','d':'DOWN','l':'LEFT','r':'RIGHT'}
        for m in moves:
            print(M[m])

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("type",type=str,choices=['dfs','bfs','a*'])
    parser.add_argument("-puzzle", type=int,help="puzzle number", choices=[1,2,3])
    parser.add_argument("-random","-r", type=int, help='random shuffle n time')
    parser.add_argument("-seed","-s", type=int, help='random shuffle n time')
    parser.add_argument("-verbose","-v", type=str, default='none', help='verbose selection')
    parser.add_argument("-moves","-m", action='store_true', help='display path')
    Args = parser.parse_args()
    main(Args)
