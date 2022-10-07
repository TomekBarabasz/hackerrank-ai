import argparse,unittest,sys

def removeUsedPairs(pairs, idx):
    return [ (i1,i2) for (i1,i2) in pairs if i1!=idx and i2!=idx ]

def solution(A):
    if len(A)==1: return 0
    Pairs = []
    for i in range(len(A)):
        i2 = (i+1) % len(A)
        if (A[i] + A[i2]) % 2 == 0:
            Pairs.append( (i,i2) )

    stack = [ (Pairs, 0, 0) ] #avalable pairs, next pair idx, num pairs

    max_pairs = 0
    while stack:
        Pairs, start, npairs = stack.pop(0)
        if len(Pairs)==0 or start >= len(Pairs):
            if npairs > max_pairs:
                max_pairs = npairs
        else:
            idx = start
            for idx in range(start,len(Pairs)):
                i0,i1 = Pairs[idx]
                NPairs = removeUsedPairs(Pairs,  i0)
                NPairs = removeUsedPairs(NPairs, i1)
                stack.append( (NPairs, 0, npairs+1) )
    
    return max_pairs

class TestSolution(unittest.TestCase):
    def test_1(self):
        Tests = [
            ([1],      0),
            ([1,1],    1),
            ([1,1,1],  1),
            ([1,2,1],  1),
            ([1,1,1,1],2),
            ([1,2,1,1],1),
            ([1,1,2,1],1),
            ([1,1,1,2],1),
            ([1,2,1,2],0),
            ([1,2,1,1,2,1],2),
            ([1,2,1,1,1,2,1],2),
        ]
        for I,O in Tests:
            out = solution(I)
            self.assertEqual(out, O)

def main(Args):
    print('solution main')
    pass

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-ut",action='store_true')
    parser.add_argument("-iter","-i", type=int, default=10,help='number of perf iterations')
    Args = parser.parse_args()

    if Args.ut:
        unittest.main(argv=[sys.argv[0]])
    else:
        main(Args)
