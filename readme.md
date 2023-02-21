# LOW FISH
- Written in C++
- Low Fish is a low level fast language.
- Low Fish has static types.
- Goal is to compile to Windows Linux and [MaslOS](https://github.com/marceldobehere/MaslOS).   
# COMPILER
- Not fully optimized but as fast as I know how to make.
- Fast. 
- Statically typed.

# FEATURES
- Basically is a weird mixture between C and C++
- Modular , has 'containers' which are ways to create modules out of your code

# Hello, World!

```less
include io from "std.lf"

int main(){
    puts("Hello, World!");
}
```

To compile this:
```
lf -o main.exe main.lf
```
or
```
lf main.lf -o main.exe
```
to get compile time info add flag `-t`
the output of main.exe is
`Hello, World!`

