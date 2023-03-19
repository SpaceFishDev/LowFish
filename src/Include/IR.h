#include<syntax/Parser.h>

class IR
{
    public:

        Node* tree;
        std::string IRCode = "";
        std::string CurrentFunction = "global";
        std::vector<std::string> DefinedVariables;
        std::vector<std::string> DefinedVariablesScopes;
        void AppendIR(std::string str)
        {
            IRCode += str;
            IRCode += "\n";
        }

        IR(std::string str)
        {
            Parser parser = Parser(str);
            tree = new Node(new Token(0,"root", 0, 0),PROGRAM, 0);
            tree = parser.Parse(tree, tree);
            printTree(tree, "", true);
        }
        int LoopId = 0;
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
                if(root->Parent->Type == IFNODE || root->Parent->Type == WHILENODE)
                {
                    AppendIR("label LO" + std::to_string(root->Parent->ID) + "END");
                }
                if(root->Parent->Type == WHILENODE)
                {
                    AppendIR("jmp LO" + root->Parent->ID);
                }
                if(root->Parent->Type == FUNCTION)
                {
                    CurrentFunction = "global";
                }
            }
            CompileEndOfNode(root);
        }
        void CompileWhileNode(Node* root)
        {
            root->ID = LoopId;
            ++LoopId;
            AppendIR("label LO" + root->ID);
        }
        void CompileAssembly(Node* root)
        {
            AppendIR("native \"" + root->NodeToken->Text + "\"");
        }
        void CompileBoolExpr(Node* root)
        {
            if
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
        }
        void CompileEqual(Node* root)
        {
            std::string o = "set " + root->NodeToken->Text;
            switch(root->Children[0]->Type)
            {
                case REFERENCE:
                {
                    o += " " +  root->Children[0]->NodeToken->Text;
                } break;
                case CONSTANT_NODE:
                {
                    if(root->Children[0]->NodeToken->Type == CONSTANT)
                    {
                        o += " " +  root->Children[0]->NodeToken->Text;
                    }
                    if(root->Children[0]->NodeToken->Type == STRING && root->Children[0]->NodeToken->Text.length() == 1)
                    {
                        o += " '" +  root->Children[0]->NodeToken->Text + "'";
                    }
                    else if(root->Children[0]->NodeToken->Type == STRING)
                    {
                        o += " \"" +  root->Children[0]->NodeToken->Text + "\"";
                    }
                } break;
            }
            AppendIR(o);
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
                AppendIR("let " + root->Children[0]->NodeToken->Text);
            }
        }
        void CompileEndOfNode(Node* root)
        {
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
                AppendIR("return\n");
            }
        }
        void CompileFunction(Node* root)
        {
            AppendIR("func " + root->Children[0]->NodeToken->Text);
            CurrentFunction = root->Children[0]->NodeToken->Text;
        }
    private:
};
