import os

def test(a):
    os.system(f"./lowfish tests/{a} > out.txt")
    f = open('out.txt', 'r')
    got = f.read()
    f2 = open(f'tests/{a}.expected', 'r')
    expected = f.read()
    f.close()
    f2.close()
    return [expected in got, a, got]

def print_results(x):
    print(f'-- Tested file: {x[1]} --')
    print(f'-- Succeeded: {x[0]} --')
    if x[0] == False:
        print(f'-- What We got: \'\n{x[2]}\n\' --')

dirs =  list(filter((lambda a : a.endswith('.lf')), os.listdir('./tests/')))
results = list(map(test ,dirs))
list(map(print_results, results))

