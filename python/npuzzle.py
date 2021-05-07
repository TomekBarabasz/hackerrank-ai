from queue import PriorityQueue
import argparse

def loadPuzzle(puzzle):
    if puzzle==1:
        return [[0,3,8],[4,1,7],[2,6,5]],3

def solvePuzzle(grid,GridSize):
    moves = ['RIGHT','DOWN','LEFT','UP']
    return moves

def main(Args):
    grid,GridSize = loadPuzzle(Args.puzzle)

    if not Args.random:
        moves = solvePuzzle(grid,GridSize)
    else:
        pass
    print("moves",len(moves))
    if Args.moves:
        for m in moves:
            print(m)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    #parser.add_argument("type",type=str,choices=['dfs','bfs','a*'])
    parser.add_argument("-puzzle", type=int,help="puzzle number", choices=[1,2,3])
    parser.add_argument("-iter","-i", type=int,help='number of random iterations')
    parser.add_argument("-random","-r", action='store_true', help='randomize start & end positions')
    parser.add_argument("-moves","-m", action='store_true', help='display path')
    Args = parser.parse_args()
    main(Args)
