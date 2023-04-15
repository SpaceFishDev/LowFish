#include<syntax/Parser.h>

class IR
{
    public:

        Node* tree;
        std::string IRCode = "";
        std::string CurrentFunction = "global";
        std::vector<std::string> DefinedVariables;
        std::vector<std::string> DefinedVariablesScopes;
        std::vector<std::string> Assembly;
        int AssemblyIndex = 0;
        std::vector<std::string> FunctionCalls;
        int strIndex = 0;
        std::vector<std::string> Strings;
        int StringIndex = 0;
        void GetCalls(Node* Root)
        {
            if(Root->Type == FUNCTION_CALL)
            {
                FunctionCalls.push_back(Root->NodeToken->Text);
            }
            for(Node* c : Root->Children)
            {
                GetCalls(c);
            }
        }
        void RemoveStrings(Node* root)
        {
            if(root == nullptr)
            {
                return;
            }
            if(root->NodeToken->Type == STRING)
            {
                std::cout << "FUNC REMOVING STRINGS ???\n";
                Strings.erase(std::next(begin(Strings) ,StringIndex));
                StringIndex--;
                return;
            }
            for(Node* c : root->Children)
            {
                RemoveStrings(c);
            }
        }
        void CullUnused(Node* Root)
        {
            if(Root->Type == STRING)
            {
                ++StringIndex;
            }
            if(Root->Type == FUNCTION)
            {
                std::string name = Root->Children[0]->NodeToken->Text;
                bool called = false;
                if(name == "main")
                {
                    called = true;
                }
                for(std::string f : FunctionCalls)
                {
                    if(f == name )
                    {
                        called = true;
                    }
                }
                if(!called)
                {
                    Node* p = Root->Parent;
                    int i = 0;
                    for(Node* c : p->Children)
                    {
                        if(c == Root)
                        {
                            break;
                        }
                        ++i;
                    }
                    RemoveStrings(p);
                    p->Children.erase(p->Children.begin() + i);
                }
            }
            for(Node* child : Root->Children)
            {
                CullUnused(child);
            }
        }
        void AppendIR(std::string str, bool New = true)
        {
            IRCode += str;
            if(New)
                IRCode += "\n";
        }
        void FixStrings(Node* n)
        {
            if(n != nullptr)
            {
                if(n->NodeToken->Type == STRING)
                {
                    std::cout << Strings.size() << " " << StringIndex << "\n";
                    n->NodeToken->Text = Strings[StringIndex++];
                }
                else if(n->Children.size() > 0)
                {
                    for(Node* c : n->Children)
                    {
                        FixStrings(c);
                    }
                }
            }
        }
        IR(std::string str)
        {
            Parser parser = Parser(str);
            tree = new Node(new Token(0,"root", 0, 0),PROGRAM, 0);
            tree = parser.Parse(tree, tree);
            Strings = parser.Strings;
            printTree(tree, "", true);
            GetCalls(tree);
            CullUnused(tree);
            StringIndex = 0;
            FixStrings(tree);
            std::cout << "CULLING ACTIVE:\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n"; 
            printTree(tree, "", true);
            Assembly = parser.Assembly;

        }
        int LoopId = 0;
        void CompileMath(Node* root)
        {
            if(root->Children[0]->Type == REFERENCE)
            {
                std::cout << root->NodeToken->Text << "\n";
                switch(root->NodeToken->Text[0]){
                    case '+':
                        AppendIR("add " + root->Children[0]->NodeToken->Text + " " + Evaluate(root->Children[1])->NodeToken->Text);
                    break;
                    case '-':
                        AppendIR("sub " + root->Children[0]->NodeToken->Text + " " + Evaluate(root->Children[1])->NodeToken->Text);
                    break;
                }
            }
            else if(root->Children[0]->Type == MATH)
            {
                if(root->Children[0]->Children[0]->Type == REFERENCE)
                {
                    AppendIR("add " + root->Children[0]->Children[0]->NodeToken->Text + " " + Evaluate(root->Children[1])->NodeToken->Text);
                }
                if(root->Parent->Type == EQUAL)
                {
                    AppendIR("set " + root->Parent->NodeToken->Text + " \%eax");
                }
                return;
            }
            else
            {
                return;
            }
            if(root->Parent->Type == EQUAL)
            {
                AppendIR("set " + root->Parent->NodeToken->Text + " \%eax");
            }
            
        }
        Node* Evaluate(Node* root)
        {
            if(root->Type == MATH)
            {
                for(Node* child : root->Children)
                {
                    if(child->Type == REFERENCE)
                    {
                        return root;
                    }
                }
                root->Type = CONSTANT_NODE;
                root->Children[0] = Evaluate(root->Children[0]);
                root->Children[1] = Evaluate(root->Children[1]);
                if(root->Children[0]->Type == MATH || root->Children[1]->Type == MATH)
                {
                    return root;
                }
                int x = 0;
                
                switch(root->NodeToken->Text[0]){
                    case '+':{
                            x = std::stoi(root->Children[0]->NodeToken->Text.c_str()) 
                                + std::stoi(root->Children[1]->NodeToken->Text.c_str());
                    } break;
                    case '-':{
                            x = std::stoi(root->Children[0]->NodeToken->Text.c_str()) 
                            - std::stoi(root->Children[1]->NodeToken->Text.c_str());
                    } break;
                    case '/':{
                            x = std::stoi(root->Children[0]->NodeToken->Text.c_str()) 
                            / std::stoi(root->Children[1]->NodeToken->Text.c_str());
                    } break;
                    case '*':{
                            x = std::stoi(root->Children[0]->NodeToken->Text.c_str()) 
                            * std::stoi(root->Children[1]->NodeToken->Text.c_str());
                    } break;
                }
                root->NodeToken = new Token( CONSTANT,std::to_string(x), root->NodeToken->Line, root->NodeToken->Column);
            }
            
            return root;
        }
        void Compile(Node* root)
        {
            switch(root->Type)
            {
                case FUNCTION:
                {
                    CompileFunction(root);
                } break;
                case VAR:
                {
                    CompileVar(root);
                } break;
                case EQUAL:
                {
                    CompileEqual(root);
                } break;
                case REFERENCE:
                {
                    CompileReference(root);
                } break;
                case IFNODE:
                {
                    CompileIfNode(root);
                } break;
                case BOOLEXPR:
                {
                    CompileBoolExpr(root);
                } break;
                case RETURNNODE:
                {
                    CompileReturn(root);
                } break;
                case ASSEMBLY:
                {
                    CompileAssembly(root);
                } break;
                case WHILENODE:
                {
                    CompileWhileNode(root);
                } break;
                case CONSTANT_NODE:
                {
                    CompileConstantNode(root);
                } break;
                case POINTERVAR:
                {
                    CompilePtr(root);
                } break; 
                case EXTERN:
                {
                    CompileExtern(root);
                } break;
                case INDEX:
                {
                    CompileIndex(root);
                } break;
            }
            for(Node* child : root->Children)
            {
                Compile(child);
            }
            if(root->Type == FUNCTION_CALL)
            {
                CompileFunctionCall(root);
            }
            
            if(root->Type == BLOCK)
            {
                if(root->Parent->Type == WHILENODE)
                {
                    AppendIR("jmp LO" + std::to_string(root->Parent->ID));
                }
                else if(root->Parent->Type == FUNCTION) [[likely]]
                {
                    CurrentFunction = "global";
                }
                else if(root->Parent->Type == IFNODE) 
                {
                    AppendIR("label LO" + std::to_string(root->Parent->ID) + "END");
                }
                else
                {
                    std::cout << NodeTypeToString(root->Parent->Type) << "\n";
                }
            }
            if(root->Type == WHILENODE)
            {
                AppendIR("label LO" + std::to_string(root->ID) + "END\n");
            }
            CompileEndOfNode(root);
        }
        void CompileIndex(Node* root)
        {
            if(root->Parent->Type == REFERENCE)
            {
                if(root->Children[1]->Children.size() != 0 && root->Children[1]->Children[0]->Type == EQUAL)
                {
                    Node* n = Evaluate(root->Children[0]);
                    Token* t = Evaluate(root->Children[1]->Children[0]->Children[0])->NodeToken;
                    if(n->Type == CONSTANT_NODE && t->Type == CONSTANT)
                    {
                        AppendIR("set $" + n->NodeToken->Text + " " + root->Parent->NodeToken->Text + " " + t->Text);
                    }
                    else if(n->Type == CONSTANT_NODE && t->Type == STRING)
                    {
                        AppendIR("set $" + n->NodeToken->Text + " " + root->Parent->NodeToken->Text + " \"" + t->Text + "\"");
                    }
                    else if(n->Type == REFERENCE)
                    {
                    }   
                }
                else if(root->Parent->Parent->Type == EQUAL && root->Parent->Parent->NodeToken->Text != "=")
                {
                    Node* n = Evaluate(root->Children[0]);
                    if(n->Type == CONSTANT_NODE && n->NodeToken->Type == CONSTANT)
                    {
                        AppendIR("set " + root->Parent->Parent->NodeToken->Text + " $" + n->NodeToken->Text + " " + root->Parent->NodeToken->Text);
                    }
                    else if(n->Type == REFERENCE)
                    {
                        AppendIR("set " + root->Parent->Parent->NodeToken->Text + " $" + n->NodeToken->Text + " " + root->Parent->NodeToken->Text);
                    }   
                }
            }
        }
        void CompileExtern(Node* root)
        {
            AppendIR("native \"[extern " + root->NodeToken->Text + "]\"");
        }
        void CompileConstantNode(Node* root)
        {
            if(root->Parent->Type == FUNCTION_CALL)
            {
                if(root->NodeToken->Type == STRING)
                {
                    AppendIR(std::string("push ") + "\"" + root->NodeToken->Text + "\"");
                }
                else
                {
                    AppendIR(std::string("push ") + root->NodeToken->Text);
                }
            }        
        }
        void CompileWhileNode(Node* root)
        {
            root->ID = LoopId;
            ++LoopId;
            AppendIR("label LO" + std::to_string(root->ID));
        }
        void CompileAssembly(Node* root)
        {
            AppendIR("native \"" +  Assembly[AssemblyIndex++] + "\"");
        }
        
        void CompileBoolExpr(Node* root)
        {
            if(root->Children[0]->NodeToken->Type == CONSTANT && root->Children[1]->NodeToken->Type == CONSTANT)
            {
                if(root->NodeToken->Text == "==")
                {
                    int a = std::atoi(root->Children[0]->NodeToken->Text.c_str());
                    int b = std::atoi(root->Children[1]->NodeToken->Text.c_str());
                    if(a != b)
                    {
                        AppendIR("jmp LO" + std::to_string(root->Parent->ID) + "END");
                    }
                }
            }
            else if
            (
                root->Children[0]->NodeToken->Type == CONSTANT
            )
            {
                if(root->Parent->Type == IFNODE || root->Parent->Type == WHILENODE)
                {
                    AppendIR
                    (
                        "cmp " + 
                        root->Children[0]->NodeToken->Text + 
                        ", 1\n" +
                        "jge LO" + std::to_string(root->Parent->ID) + "END" 
                    );
                }
            }
            else if
            (
                (root->Children[0]->Type == REFERENCE
                && root->Children[1]->Type == REFERENCE)
                || root->Children[0]->Type == REFERENCE 
                && root->Children[1]->NodeToken->Type == CONSTANT
            )
            {
                if(root->Parent->Type == IFNODE || root->Parent->Type == WHILENODE)
                {
                    AppendIR
                    (
                        "cmp " + 
                        root->Children[0]->NodeToken->Text + 
                        " " +
                        root->Children[1]->NodeToken->Text
                    );
                    if(root->NodeToken->Text == "==")
                    {
                        AppendIR("jne LO" + std::to_string(root->Parent->ID) + "END");
                    }
                    if(root->NodeToken->Text == "!=")
                    {
                        AppendIR("je LO" + std::to_string(root->Parent->ID) + "END");
                    }
                    if(root->NodeToken->Text == ">")
                    {
                        AppendIR("jl LO" + std::to_string(root->Parent->ID) + "END");
                    }
                    if(root->NodeToken->Text == "<")
                    {
                        AppendIR("jg LO" + std::to_string(root->Parent->ID) + "END");
                    }
                    if(root->NodeToken->Text == ">=")
                    {
                        AppendIR("jlne LO" + std::to_string(root->Parent->ID) + "END");
                    }
                    if(root->NodeToken->Text == "<=")
                    {
                        AppendIR("jgne LO" + std::to_string(root->Parent->ID) + "END");
                    }
                }
            }
            
        }
        void CompileReturn(Node* root)
        {
            if(root->Children[0]->Type == REFERENCE || root->Children[0]->NodeToken->Type == CONSTANT)
            {
                AppendIR("set \%eax " + root->Children[0]->NodeToken->Text);
                AppendIR("return");
            }
        }
        void CompileIfNode(Node* root)
        {
            root->ID = LoopId++;
        }
        void CompileReference(Node* root)
        {
            if(root->Parent->Type == FUNCTION_CALL)
            {
                AppendIR("push " + root->NodeToken->Text);
            }
        }
        void CompileFunctionCall(Node* root)
        {
            AppendIR("call " + root->NodeToken->Text);
            if(root->Parent->Type == EQUAL)
            {
                AppendIR("set " + root->Parent->NodeToken->Text + " \%eax");
            }
            if(root->Parent->Type == FUNCTION_CALL)
            {
                AppendIR("push \%eax");
            }
        }
        void CompileEqual(Node* root)
        {
            if(root->NodeToken->Text == "=")
            {
                return;
            }
            std::string o = "set " + root->NodeToken->Text;
            switch(root->Children[0]->Type)
            {
                case REFERENCE:
                {
                    if(root->Children.size() == 0){
                        o += " " +  root->Children[0]->NodeToken->Text;
                    }
                } break;
                case CONSTANT_NODE:
                {
                    if(root->Children[0]->NodeToken->Type == CONSTANT)
                    {
                        o += " " +  root->Children[0]->NodeToken->Text;
                    }
                    else if(root->Children[0]->NodeToken->Type == STRING && root->Children[0]->NodeToken->Text.length() == 1)
                    {
                        o += " '" +  root->Children[0]->NodeToken->Text + "'";
                    }
                    else if(root->Children[0]->NodeToken->Type == STRING)
                    {
                        o += " \"" +  root->Children[0]->NodeToken->Text + "\"";
                    }
                } break;
                case MATH:
                {
                    Node* newN = Evaluate(root->Children[0]);
                    if(root->Children[0]->NodeToken->Type == CONSTANT)
                    {
                        o += " " +  root->Children[0]->NodeToken->Text;
                        AppendIR(o);
                        return;
                    }
                    CompileMath(root->Children[0]);
                    return; 
                }
            }
            if(o != "set " + root->NodeToken->Text)
                AppendIR(o);
        }
        size_t getSize(std::string var)
        {
            if(var == "byte")
            {
                return 8;
            }
            if(var == "char")
            {
                return 8;
            }
            if(var == "int")
            {
                return 32;
            }
            if(var == "long")
            {
                return 64;
            }
            if(var == "ptr")
            {
                return 32;
            }
            return -1;
        }
        void CompilePtr(Node* root)
        {
           if
            (
                root->Children[0]->Type == REFERENCE
                || root->Children[0]->Type == EQUAL
            )
            {
                int i = 0;
                for(std::string v : DefinedVariables)
                {
                    if
                    (
                        v == root->Children[0]->NodeToken->Text
                        && DefinedVariablesScopes[i] == CurrentFunction
                    )
                    {
                        ErrorHandler::PutError(
                            REDEFINITION_OF_VARIABLE,
                            root->Children[0]->NodeToken->Text, 
                            root->Children[0]->NodeToken->Line, 
                            root->Children[0]->NodeToken->Column
                        );
                    }
                    i++;
                }
                DefinedVariablesScopes.push_back(CurrentFunction);
                DefinedVariables.push_back(root->Children[0]->NodeToken->Text);
                AppendIR("letptr" + std::to_string(getSize(root->NodeToken->Text)) + " " + root->Children[0]->NodeToken->Text);
            } 
        }
        void CompileVar(Node* root)
        {
            if
            (
                root->Children[0]->Type == REFERENCE
                || root->Children[0]->Type == EQUAL
            )
            {
                int i = 0;
                for(std::string v : DefinedVariables)
                {
                    if
                    (
                        v == root->Children[0]->NodeToken->Text
                        && DefinedVariablesScopes[i] == CurrentFunction
                    )
                    {
                        ErrorHandler::PutError(
                            REDEFINITION_OF_VARIABLE,
                            root->Children[0]->NodeToken->Text, 
                            root->Children[0]->NodeToken->Line, 
                            root->Children[0]->NodeToken->Column
                        );
                    }
                    i++;
                }
                DefinedVariablesScopes.push_back(CurrentFunction);
                DefinedVariables.push_back(root->Children[0]->NodeToken->Text);
                if(root->NodeToken->Text != "string")
                    AppendIR("let" + std::to_string(getSize(root->NodeToken->Text)) + " "  + root->Children[0]->NodeToken->Text);
                else
                    AppendIR("letptr "  + root->Children[0]->NodeToken->Text);

            }
        }
        void CompileEndOfNode(Node* root)
        {
            
            if(root->Type == BLOCK && root->Children.size() == 0)
            {
                AppendIR("retend\n");
            }
            if
            (
                root->Type != PROGRAM
                && root->Parent->Type == BLOCK
                &&  (
                        root->Parent->Parent->Type == NAME
                        || root->Parent->Parent->Type == FUNCTION
                    )
                && root == root->Parent->Children[root->Parent->Children.size() - 1]
            ) 
            {
                AppendIR("retend\n");
            }
        }
        void CompileFunction(Node* root)
        {
            AppendIR("func " + root->Children[0]->NodeToken->Text);
            CurrentFunction = root->Children[0]->NodeToken->Text;
        }   
    private:
};
