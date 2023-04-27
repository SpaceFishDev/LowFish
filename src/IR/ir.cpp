#pragma GCC diagnostic ignored "-Wreturn-type"
#include<iostream>
#include<fstream>
#include<vector>
#include<speedtypes.h>
enum LexerTypes
{
    REGISTER,
    IDENTIFIER,
    NUMBER,
    STRING,
    INDEX,
    END
};
char* ReadFile(std::string file) {
    const char* filename = file.c_str();
    FILE *file_ptr;
    char ch;
    int size = 0;
    char *buffer = NULL;

    file_ptr = fopen(filename, "r");

    if (file_ptr == NULL) {
        printf("Unable to open file.\n");
        return NULL;
    }

    fseek(file_ptr, 0L, SEEK_END);
    size = ftell(file_ptr);
    rewind(file_ptr);

    buffer = (char*)malloc(size + 1);

    int i = 0;
    while ((ch = fgetc(file_ptr)) != EOF) {
        buffer[i] = ch;
        i++;
    }
    buffer[i] = '\0'; 

    fclose(file_ptr);
    return buffer;
}
struct token
{
    int Type;
    std::string Text;
    token(int type, std::string text){
        Text = text;
        Type = type;
    }
};
#define current text[Position]
class Lexer
{
    public:
    int Position = 0;
    std::string text;
    std::vector<token> Tokens;
    Lexer(std::string text){
        this->text = text;  
    }
    void LexAll()
    {
        while(true)
        {
            token t = Lex();
            Tokens.push_back(t);
            if(t.Type == END)
            {
                return;
            }
        }
    }
    token Lex()
    {
        
        if(current >= 'A' && current <= 'Z' || current >= 'a' && current <= 'z' || current == '_')
        {
            std::string temp = "";
            while(current >= 'A' && current <= 'Z' || current >= 'a' && current <= 'z' || current == '_' || current >= '0' && current <= '9' || current == '-')
            {
                temp += current;
                ++Position;
            }
            return token(IDENTIFIER, temp);
        }
        if(current >= '0' && current <= '9' || current == '-')
        {
            std::string temp = std::string(1,current);
            ++Position;
            while(current && current >= '0' && current <= '9')
            {
                temp += current;
                ++Position;
            }
            return token(NUMBER, temp);
        }
        switch(current)
        {
            case '\0':
            {
                return token(END,"END OF FILE");
            }    
            case '"':
            {
                std::string temp = "";
                ++Position;
                while(current && current != '"'){
                    temp += current;
                    ++Position;
                }
                ++Position;
                return token(STRING, temp);
            }
            case '\n':
            case ' ':{
                ++Position;
                return Lex();
            }
            case '%':
            {
                ++Position;
                std::string temp = "";
                while(current >= 'A' && current <= 'Z' || current >= 'a' && current <= 'z' || current == '_')
                {
                    temp += current;
                    ++Position;
                }
                return token(REGISTER, temp);
            }
            case '$':
            {
                ++Position;
                std::string temp = "";
                if(current == "-")
                {
                    printf("Index can only be positive.\n");
                    exit(-1);
                }
                while(current && current >= '0' && current <= '9' || current >= 'A' && current <= 'Z' || current >= 'a' && current <= 'z' || current == '_')
                {
                    temp += current;
                    ++Position;
                }
                return token(INDEX, temp);
            }
        }
        return token(END,"END OF FILE");
    }

};

struct var
{
    int stack_pos;
    int size;
    std::string scope;
    bool ptr;
    bool initialized;
    var(int sp, std::string s, int size, bool ptr)
    {
        stack_pos = sp;
        scope = s;
        this->size = size;
        this->ptr = ptr;
        initialized = false;
    }
};

struct label
{
    bool used = false;
    int index = 0;
    std::string name;
    std::string scope;
    label(int index, std::string name, std::string scope)
    {
        this->index = index;
        this->name = name;
        this->scope = scope;    
    }
};

std::vector<token> Optimize(std::vector<token> tokens)
{
    std::string scope = "";
    std::vector<label> Labels;
    for(int i = 0; i != tokens.size(); ++i)
    {
        switch(tokens[i].Type)
        {
            case IDENTIFIER:
            {
                
            }
        }
    }    
}

std::string compile_win32(std::vector<token> tokens)
{
    std::string text = "section .text\n";
    std::string bss = "section .bss\n";
    std::string data = "section .data\n";
    int strIndex = 0;
    std::string scope = "global";
    hash_map* map = init_hash_map();
    int stack_pos = 0;
    std::string sizes[] = {"", "byte", "word", "","dword","","","","qword"};

    for(int i = 0; i < tokens.size(); ++i)
    {
        token t = tokens[i];
        try{
            switch(tokens[i].Type)
            {
                case IDENTIFIER:
                {
                    if(tokens[i].Text == "func")
                    {
                        text += tokens[i + 1].Text;
                        text += ":\n";
                        text += "push ebp\nmov ebp, esp\n"; 
                        scope = tokens[i + 1].Text;
                        ++i;
                        stack_pos += 4;
                    }
                    else if(tokens[i].Text == "label")
                    {
                        text += tokens[i + 1].Text;
                        text += ":\n";
                        ++i;
                    }
                    else if(tokens[i].Text == "push")
                    {
                        if(tokens[i + 1].Type == STRING)
                        {
                            text += "push STR" + std::to_string(strIndex) + "\n";
                            data += "STR" + std::to_string(strIndex++) + ":\n";
                            data += "db `" + tokens[i + 1].Text + "`,0\n";
                            ++i;
                        }
                        else if(tokens[i + 1].Type == NUMBER)
                        {
                            text += "push " + tokens[i + 1].Text + "\n";
                            ++i; 
                        }
                        else if(tokens[i + 1].Type == IDENTIFIER)
                        {
                            text += "push ";
                            if(!hash_contains(map, (tokens[i + 1].Text + scope).c_str()))
                            {
                                printf("Variable %s doesnt exist.", tokens[i+1].Text.c_str());
                                exit(-1);
                            }
                            hash_map_element* el = get_element(map, (tokens[i + 1].Text + scope).c_str());
                            var* v = el->value;
                            if(v->ptr )
                            {
                                text += " dword ";
                            }else
                            {
                                text += sizes[v->size];
                            }
                            text += " [ebp + " + std::to_string(v->stack_pos) + "]\n";
                            ++i;    
                        }
                    }
                    else if(tokens[i].Text == "retend" && scope != "main")
                    {
                        text += "ret\n";
                        stack_pos = 0;
                        scope = "global";
                    }
                    else if(tokens[i].Text == "retend" && scope == "main")
                    {
                        text += "hlt\n";
                        stack_pos = 0;
                        scope = "global";
                    }
                    else if(tokens[i].Text == "call")
                    {
                        text += "call " + tokens[i + 1].Text + "\n";
                        ++i;
                    }
                    else if(tokens[i].Text == "native")
                    {
                        text += tokens[i + 1].Text + "\n";
                        ++i;
                    }
                    else if(tokens[i].Text == "cmp")
                    {
                        ++i;
                        if(tokens[i].Type == IDENTIFIER)
                        {
                            if(!hash_contains(map, (tokens[i].Text + scope).c_str()))
                            {
                                printf("Variable %s doesnt exist.\n", tokens[i].Text.c_str());
                                exit(-1);
                            }
                            hash_map_element* el = get_element(map, (tokens[i].Text + scope).c_str());
                            var* v = el->value;
                            text += "mov ebx," + sizes[v->size] + " [ebp + " + std::to_string(v->stack_pos) + "]\n";
                            ++i;
                            if(tokens[i].Type == NUMBER)
                            {
                                text += "cmp ebx, " + tokens[i].Text + "\n";
                            }
                            else if(tokens[i].Type == IDENTIFIER)
                            {
                                if(!hash_contains(map, (tokens[i].Text + scope).c_str()))
                                {
                                    printf("Variable %s doesnt exist.\n", tokens[i].Text.c_str());
                                    exit(-1);
                                }
                                hash_map_element* el2 = get_element(map, (tokens[i].Text + scope).c_str());
                                var* v2 = el2->value;
                                text += "mov eax, " + sizes[v2->size] + "[ebp + " + std::to_string(v2->stack_pos) + "]\n";
                                text += "cmp ebx, eax\n";
                            }
                        }
                    }
                    else if(tokens[i].Text == "jne")
                    {
                        ++i;
                        text += "jne " + tokens[i].Text + "\n";
                    }
                    else if(tokens[i].Text == "add")
                    {

                        if(tokens[i + 1].Type == IDENTIFIER)
                        {
                            if(!hash_contains(map, (tokens[i + 1].Text + scope).c_str()))
                            {
                                printf("Variable %s doesn't exist.", tokens[i + 1].Text.c_str());
                            }
                            ++i;
                            if(tokens[i + 1].Type == IDENTIFIER)
                            {
                                if(!hash_contains(map, (tokens[i + 1].Text + scope).c_str()))
                                {
                                    printf("Variable %s doesn't exist.", tokens[i + 1].Text.c_str());
                                }
                                hash_map_element* el1 = get_element(map, (tokens[i].Text + scope).c_str()); 
                                hash_map_element* el2 = get_element(map, (tokens[i + 1].Text + scope).c_str());
                                var* v = el1->value; 
                                var* v1 = el2->value; 
                                text += "mov eax," + sizes[v->size] + "[ebp + "  + std::to_string(v->stack_pos) + "]\n";
                                text += "mov ebx," + sizes[v1->size] + "[ebp + "  + std::to_string(v1->stack_pos) + "]\n";
                                text += "add eax, ebx\n";
                                ++i;
                            }
                            else if(tokens[i + 1].Type == NUMBER)
                            {
                                hash_map_element* el1 = get_element(map, (tokens[i].Text + scope).c_str()); 
                                var* v = el1->value;
                                text += "mov eax," + sizes[v->size] + "[ebp + "  + std::to_string(v->stack_pos) + "]\n";
                                text += "mov ebx," + tokens[i + 1 ].Text + "\n";
                                text += "add eax, ebx\n";
                            }
                        }
                    }
                    else if(tokens[i].Text == "sub")
                    {
                        if(tokens[i + 1].Type == IDENTIFIER)
                        {
                            if(!hash_contains(map, (tokens[i + 1].Text + scope).c_str()))
                            {
                                printf("Variable %s doesn't exist.", tokens[i + 1].Text.c_str());
                            }
                            ++i;
                            if(tokens[i + 1].Type == IDENTIFIER)
                            {
                                if(!hash_contains(map, (tokens[i + 1].Text + scope).c_str()))
                                {
                                    printf("Variable %s doesn't exist.", tokens[i + 1].Text.c_str());
                                }
                                hash_map_element* el1 = get_element(map, (tokens[i].Text + scope).c_str()); 
                                hash_map_element* el2 = get_element(map, (tokens[i + 1].Text + scope).c_str());
                                var* v = el1->value; 
                                var* v1 = el2->value; 
                                text += "mov eax," + sizes[v->size] + "[ebp + "  + std::to_string(v->stack_pos) + "]\n";
                                text += "mov ebx," + sizes[v1->size] + "[ebp + "  + std::to_string(v1->stack_pos) + "]\n";
                                text += "sub eax, ebx\n";
                                ++i;
                            }
                            else if(tokens[i + 1].Type == NUMBER)
                            {
                                hash_map_element* el1 = get_element(map, (tokens[i].Text + scope).c_str()); 
                                var* v = el1->value;
                                text += "mov eax," + sizes[v->size] + "[ebp + "  + std::to_string(v->stack_pos) + "]\n";
                                text += "mov ebx," + tokens[i + 1 ].Text + "\n";
                                text += "sub eax, ebx\n";
                            }
                        }
                    }
                    else if(tokens[i].Text == "jmp")
                    {
                        ++i;
                        text += "jmp " + tokens[i].Text + "\n";
                    }
                    else if(tokens[i].Text == "je")
                    {
                        ++i;
                        text += "je " + tokens[i].Text + "\n";
                    }
                    else if(tokens[i].Text == "jg")
                    {
                        ++i;
                        text += "jg " + tokens[i].Text + "\n";
                    }
                    else if(tokens[i].Text == "jl")
                    {
                        ++i;
                        text += "jl " + tokens[i].Text + "\n";
                    }
                    else if(tokens[i].Text == "letptr")
                    {
                        add_element(map, init_element(new var(stack_pos, scope, 1  , true), (tokens[i + 1].Text + scope).c_str()));
                        stack_pos += 4;
                        ++i;
                    }
                    else if(tokens[i].Text == "letptr32")
                    {
                        add_element(map, init_element(new var(stack_pos, scope, 4  , true), (tokens[i + 1].Text + scope).c_str()));
                        stack_pos += 4;
                        ++i;
                    }
                    else if(tokens[i].Text == "letptr8")
                    {
                        add_element(map, init_element(new var(stack_pos, scope, 1  , true), (tokens[i + 1].Text + scope).c_str()));
                        stack_pos += 4;
                        ++i;
                    }
                    else if(tokens[i].Text == "letptr16")
                    {
                        add_element(map, init_element(new var(stack_pos, scope, 2  , true), (tokens[i + 1].Text + scope).c_str()));
                        stack_pos += 4;
                        ++i;
                    }
                    else if(tokens[i].Text == "let32")
                    {
                        var* v = new var(stack_pos, scope , 4, false);
                        add_element(map, init_element(v, (tokens[i + 1].Text + scope).c_str()));
                        stack_pos += 4;
                        ++i;
                    }
                    else if(tokens[i].Text == "let16")
                    {
                        add_element(map, init_element(new var(stack_pos, scope, 2, false), (tokens[i + 1].Text + scope).c_str()));
                        stack_pos += 2;
                        ++i;
                    }
                    else if(tokens[i].Text == "let8")
                    {
                        add_element(map, init_element(new var(stack_pos, scope, 1, false), (tokens[i + 1].Text + scope).c_str()));
                        stack_pos += 1;
                        ++i;
                    }
                    else if(tokens[i].Text == "set")
                    {
                        ++i;
                        if(tokens[i].Type == INDEX) [[unlikely]]
                        {
                            std::string str = std::string(tokens[i].Text);
                            if(!hash_contains(map, (tokens[i + 1].Text + scope).c_str()))
                            {
                                printf("Variable %s, doesn't exist!\n", tokens[i + 1].Text.c_str());
                            }
                            hash_map_element* el = get_element(map, (tokens[i + 1].Text + scope).c_str());
                            var* v = el->value;
                            if(str[0] == '_' || str[0] >= 'a' && str[1] <= 'z' || str[0] >= 'A' && str[0] <= 'Z')
                            {
                                if(tokens[i + 2].Type == NUMBER)
                                {
                                    if(!hash_contains(map, (str + scope).c_str()))
                                    {
                                        printf("Variable %s, doesn't exist!\n", str.c_str());
                                    } 
                                    hash_map_element* el2 = get_element(map, (str + scope).c_str());
                                    var* v2 = el2->value;
                                    text += "push ebx\n";
                                    text += "push eax\n";
                                    text += "mov eax," + sizes[v2->size] + "[ebp + " + std::to_string(v2->stack_pos) + "]\n";
                                    text += "mov ebx, [ebp + " + std::to_string(v->stack_pos) + "]\n";
                                    text += "add ebx, eax\n";
                                    text += "mov [ebx], " + sizes[v->size] + " " + tokens[i + 2].Text + "\n";
                                    text += "pop eax\n";
                                    text += "pop ebx\n";
                                }
                                else if(tokens[i + 2].Type == STRING)
                                {
                                    if(!hash_contains(map, (str + scope).c_str()))
                                    {
                                        printf("Variable %s, doesn't exist!\n", str.c_str());
                                    } 
                                    hash_map_element* el2 = get_element(map, (str + scope).c_str());
                                    var* v2 = el2->value;
                                    text += "push ebx\n";
                                    text += "push eax\n";
                                    text += "mov eax," + sizes[v2->size] + "[ebp + " + std::to_string(v2->stack_pos) + "]\n";
                                    text += "mov ebx, [ebp + " + std::to_string(v->stack_pos) + "]\n";
                                    text += "add ebx, eax\n";
                                    text += "mov [ebx], " + sizes[v->size] + " " + std::to_string((int)tokens[i + 2].Text[0]) + "\n";
                                    text += "pop eax\n";
                                    text += "pop ebx\n";
                                }
                            }
                            else if(str[0] >= '0' || str[0] <= '9')
                            {
                                if(tokens[i + 2].Type == NUMBER)
                                {
                                    int index = std::atoi(str.c_str());
                                    std::string reg = (sizes[v->size] == "dword") ? "ebx" : "ebx";  
                                    std::string reg2 = (sizes[v->size] == "dword") ? "ebp" : "ebp";  
                                    std::cout << v->size << "\n";
                                    text += "push " + reg + "\n";
                                    text += "mov " + reg + ", [" + reg2 +" +" + std::to_string(v->stack_pos) + "]\n";
                                    text += "add " + reg + ", " + std::to_string(index ) + "\n";
                                    text += "mov [" + reg +"], " + sizes[v->size] + " " + tokens[i + 2].Text + "\n";
                                    text += "pop " + reg + "\n";
                                    i++;
                                }
                                else if(tokens[i + 2].Type == STRING)
                                {
                                    int index = std::atoi(str.c_str());
                                    int s = v->size;
                                    std::string reg = (sizes[v->size] == "dword") ? "ebx" : "ebx";  
                                    std::string reg2 = "ebp";  
                                    text += "push " + reg + "\n";
                                    text += "mov " + reg + ", [" + reg2 +" +" + std::to_string(v->stack_pos) + "]\n";
                                    text += "add " + reg + ", " + std::to_string(index ) + "\n";
                                    text += "mov [" + reg +"], " + sizes[v->size] + " " + std::to_string((int)tokens[i + 2].Text[0]) + "\n";
                                    text += "pop " + reg + "\n";
                                    i++;
                                }
                            }
                            
                            ++i;

                        }
                        else if(tokens[i].Type == IDENTIFIER) [[likely]]
                        {
                            if(hash_contains(map, (tokens[i].Text + scope).c_str()))
                            {
                                hash_map_element* el = get_element(map, (tokens[i].Text + scope).c_str());
                                var* v = (var*)el->value;
                                std::string name = tokens[i].Text;
                                if(v->ptr)
                                {
                                    ++i;
                                    if(tokens[i].Type == STRING)
                                    {
                                        text += "push ebx\n";
                                        text += "mov ebx, ebp \n"; 
                                        text += "add ebx, " +  std::to_string(v->stack_pos) + "\n";
                                        text += "mov [ebx], dword STR" + std::to_string(strIndex) + "\n";
                                        text += "pop ebx\n";
                                        data += "STR" + std::to_string(strIndex) + ":\n";
                                        data += "db `" + tokens[i].Text + "`,0\n";
                                        strIndex++;
                                        v->initialized = true;  
                                    }
                                    else if(tokens[i].Type == REGISTER)
                                    {
                                        text += "mov [ebp + " + std::to_string(v->stack_pos) + "], " + tokens[i].Text + "\n";
                                    }
                                    
                                }
                                else
                                {
                                    switch(tokens[i + 1].Type)
                                    {
                                        case STRING:
                                        {
                                            printf("Cannot assign string to non pointer variable %s\n", name.c_str());
                                            exit(1);
                                        } break;
                                        case NUMBER:
                                        {
                                            text += "push ebx\n";
                                            text += "mov ebx, ebp\n";
                                            text += "add ebx, " + std::to_string(v->stack_pos) + "\n";
                                            text += "mov [ebx], " + sizes[v->size] + " " + tokens[i + 1].Text + "\n";
                                            text += "pop ebx\n";
                                        } break;
                                        case REGISTER:
                                        {
                                            text += "mov [ebp + " + std::to_string(v->stack_pos) + "], " + tokens[i + 1].Text + "\n";
                                        } break;
                                        case INDEX:
                                        {
                                            if (tokens[i + 2].Text[0] >= '0' && tokens[i + 2].Text[0] <= '9') {
                                                int index = std::atoi(tokens[i + 1].Text.c_str());
                                                ++i;
                                                ++i;
                                                if (!(hash_contains(map, (tokens[i].Text + scope).c_str())))
                                                {
                                                    printf("Variable %s doesn't exist.\n", tokens[i].Text.c_str());
                                                    exit(-1);
                                                }
                                                hash_map_element* el = get_element(map, (tokens[i].Text + scope).c_str());
                                                var* v1 = el->value;
                                                text += "push ebx\n";
                                                text += "push eax\n";
                                                text += "mov ebx, ebp\n";
                                                text += "add ebx, " + std::to_string(v->stack_pos) + "\n";
                                                text += "mov eax, [ebp + " + std::to_string(v1->stack_pos) + "]\n";
                                                text += "add eax, " + std::to_string(index) + "\n";
                                                text += "mov eax, [eax]\n";
                                                text += "mov [ebx], eax\n";
                                                text += "pop eax\n";
                                                text += "pop ebx\n";
                                                --i;
                                            }
                                            else
                                            {
                                                std::string str = tokens[i + 1].Text;
                                                if (!hash_contains(map, (str + scope).c_str()))
                                                {
                                                    printf("Variable %s doesn't exist.\n", tokens[i].Text.c_str());
                                                    exit(-1);
                                                }
                                                hash_map_element* el = get_element(map, (str + scope).c_str());
                                                ++i;
                                                if (!hash_contains(map, (tokens[i + 1].Text + scope).c_str()))
                                                {
                                                    printf("Variable %s doesn't exist.\n", tokens[i].Text.c_str());
                                                    exit(-1);
                                                }
                                                std::cout << str << " " << tokens[i + 1].Text << "\n";
                                                hash_map_element* el2 = get_element(map, (tokens[i + 1].Text + scope).c_str());
                                                var* v1 = el->value;
                                                var* v2 = el2->value;
                                                text += "push ebx\n";
                                                text += "push eax\n";
                                                text += "mov ebx, ebp\n";
                                                text += "add ebx, " + std::to_string(v->stack_pos) + "\n";
                                                text += "mov eax, [ebp + " + std::to_string(v2->stack_pos) + "]\n";
                                                text += "add eax, [ebp + " + std::to_string(v1->stack_pos) + "]\n";
                                                text += "mov eax, [eax]\n";
                                                text += "mov [ebx], eax\n";
                                                text += "pop eax\n";
                                                text += "pop ebx\n";
                                            }
                                        } break;
                                    } 
                                    ++i;
                                }
                            }
                            else
                            {
                                printf("Variable %s doesnt exist.\n", tokens[i].Text.c_str());
                                exit(1);
                            }
                        }
                    }
                } break;
                case END:
                {
                    return data + "\n" + bss + "\n" + text + "\n";
                }
            }
        }
        catch(...)
        {
            printf(":P\n");    
        }
    }
}

int main()
{
    std::string src = ReadFile("out.lfir");
    Lexer lexer = Lexer(src);
    lexer.LexAll();
    std::string assembly =  compile_win32(lexer.Tokens);
    std::cout << assembly << "\n";
    std::ofstream outFile("out.asm");
    outFile << assembly;
    outFile.close();
    system("nasm out.asm -f win32 -o out.obj");
    system("golink /entry:main /console msvcrt.dll kernel32.dll  out.obj > nul 2> nul");
}