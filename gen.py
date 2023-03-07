file = "int main()\n{\tint x = 0;\nmain0();\n}"
file1 = "\nint main"
file2 = "(){\n\tint x = 0;\n"
i = 100
end = ""
x = 100
while i != -1:
    end += file1 + str(i) + file2 + "\tmain" + str(i + 1) + "();\n"+"}"
    i = i - 1
    if i % 10 == 0:
        print(str(i))
f = open("out.lf","w")
f.write(end + file)
f.close()
