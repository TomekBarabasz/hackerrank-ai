import argparse,unittest
from datetime import datetime
from pathlib import Path
from heapq import *
import random
from statistics import mean
from collections import Counter
from math import ceil
from timeit import timeit

def missingint_1(A):
    # write your code in Python 3.6
    N = set(range(1,100_000+1))
    diff = N.difference(set(A))
    for n in diff:
        return n

def missingint_2(A):
    heapify(A)
    N=1
    prev_a = None
    while len(A)>0:
        a = heappop(A)
        if a <= 0 : continue
        if a == prev_a : continue
        prev_a = a
        if N < a:
            return N
        else:
            N += 1
    return N

class TestMissingint(unittest.TestCase):
    def test_s1_1(self):
        n = missingint_1([90, 91, 92, 93])
        self.assertEqual(n, 1)

    def test_s2_1(self):
        n = missingint_2([90, 91, 92, 93])
        self.assertEqual(n, 1)

    def test_s1_2(self):
        n = missingint_1([90, -1, 91, -2, 92, -3, 93])
        self.assertEqual(n, 1)

    def test_s2_2(self):
        n = missingint_2([90, -1, 91, -2, 92, -3, 93])
        self.assertEqual(n, 1)

    def test_s1_3(self):
        n = missingint_1([0, -1, 0, -2, 1, -3, 2, -4, 4])
        self.assertEqual(n, 3)
    
    def test_s2_3(self):
        n = missingint_2([0, -1, 0, -2, 1, -3, 2, -4, 4])
        self.assertEqual(n, 3)

    def test_s1_4(self):
        n = missingint_1([1, 3, 6, 4, 1, 2])
        self.assertEqual(n, 5)

    def test_s2_4(self):
        n = missingint_2([1, 3, 6, 4, 1, 2])
        self.assertEqual(n, 5)

def missingint_main(Args):
    import matplotlib.pyplot as plt
    def genList(size, rmin=-10, rmax=None):
        if rmax is None: rmax=size*10
        return [random.randrange(rmin, rmax) for _ in range(size) ]
    res = []
    for _ in range(Args.iter):
        A = genList(100000)
        ts = datetime.now()
        n = missingint_2(A)
        dt = datetime.now() - ts
        res.append( dt.total_seconds()*1000 )
    print('mean exec time',mean(res),'[msec]')
    plt.hist(res)
    plt.xlabel('exec time [msec]')
    plt.show()

def binarygap(n):
    zeros_count = 0
    max_zeros_count = 0
    found_one = False
    #find the 1st bit set
    for shift in range(0,32):
        mask = 1 << shift
        if n & mask: 
            found_one = True
            break
    if not found_one:
        return 0
    
    for shift in range(shift+1,32):
        mask = 1 << shift
        if n & mask:
            max_zeros_count = max(zeros_count, max_zeros_count)
            zeros_count = 0
        else:
            zeros_count += 1
    return max_zeros_count

def cyclic_rotation(A,K):
    N = len(A)
    if N==0: return A
    K = K % N
    if K == 0: return A
    return A[N-K:] + A[:N-K]

def odd_occurences(A):
    counter = Counter(A)
    for a,cnt in counter.most_common():
        if cnt & 1:
            return a

def perm_missing_elem(A):
    N = len(A)
    if N==0: return 1
    heapify(A)
    min_a = 1
    while len(A):
        a = heappop(A)
        if a != min_a:
            return a-1
        min_a = a+1
    return min_a

def frog_river_one(X,A):
    path = set()
    for sec in range(len(A)):
        pos = A[sec]
        if pos <= X:
            path.add(pos)
        if len(path)==X:
            return sec
    return -1

def max_counters(N,A):
    # write your code in Python 3.6
    Counters = [0]*N
    max_value=0
    for action in A:
        if action <= N:
            cval = Counters[action-1] + 1
            Counters[action-1] = cval
            if cval > max_value:
                max_value = cval
        else:
            Counters = [max_value]*N
    return Counters

class TestBinarygap(unittest.TestCase):
    def test_1(self):
        self.assertEqual(binarygap(0b1001),2)   #9
    def test_2(self):
        self.assertEqual(binarygap(0b1000010001),4) #529
    def test_3(self):
        self.assertEqual(binarygap(0b10100),1) #20
    def test_4(self):
        self.assertEqual(binarygap(15),0)
    def test_5(self):
        self.assertEqual(binarygap(32),0)
    def test_6(self):
        self.assertEqual(binarygap(1041),5)

class TestCyclicRotation(unittest.TestCase):
    def test_1(self):
        self.assertEqual(0,2) 

def binarygap_main(Args):
    pass

def CyclicRotation_main(Args):
    pass

def max_counters_main(Args):
    pass

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("test",type=str,help="test name")
    parser.add_argument("run_type",type=str,choices=['ut','perf'], help="directory containing input (datafeed) files")
    parser.add_argument("-iter","-i", type=int, default=10,help='number of perf iterations')
    Args = parser.parse_args()

    if Args.run_type=='ut':
        unittest.main(argv=['program_selectory.py'])
    else:
        Tests = {'missingint':missingint_main, 'binarygap':binarygap_main, 'cyclicrotation':CyclicRotation_main, "mc":max_counters_main}
        Tests[Args.test](Args)