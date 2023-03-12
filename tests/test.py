import random
import string
func_id = 0

def randStr(chars = string.ascii_uppercase + string.ascii_lowercase, N=10):
	return ''.join(random.choice(chars) for _ in range(N))

def test(path, name):
    file = open(path, "r")
    real = file.read()
    real = real.replace("@^", name)
    i = real.split("\n~\n")[0]
    o = real.split("\n~\n")[1]
    f = open(name+".lf", "w")
    f.write(i)
    f.close()
    import os
    os.system("lf " + name + ".lf " + "-o " + name + ".asm")
    out = open(name + ".asm", "r").read()
    out = out.replace(o, " ")
    if out.isspace():
        os.remove(name + ".lf")
        os.remove(name + ".asm")
        os.remove(name + ".bin")
    return out.isspace()

def get_num(str):
    test_types = ["func", "var", "assign_var", "if statement"]
    i = 0
    while i != test_types.count:
        if test_types[i] == str:
             return i
        i = i + 1

i = 0
import os
tests = []
num_t = 0
for path in os.scandir("impl/"):
     tests.append(("impl/" + path.name, randStr(N=7)))
     num_t = num_t + 1
succeed = [False]*num_t
while i != num_t:
    succeed[i] = test(tests[i][0],tests[i][1])
    i = i + 1
i = 0
success = True
for n in succeed:
    print("TEST " + str(i) + "(TYPE=" + tests[i][0] + ", INPUT=" + tests[i][1] +") : SUCCEED=" + str(n))
    if n == False:
         success = n
    import os
    name = tests[i][1]
    i = i + 1

if success == False:
    print("\n[One or more tests failed.]")

