#include<syntax/Parser.h>

class IR
{
    public:
        Node* tree;
        std::string IRCode = "";

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
            }
            for(Node* child : root->Children)
            {
                Compile(child);
            }
            if(root->Type == FUNCTION_CALL)
            {
                CompileFunctionCall(root);
            }
            CompileEndOfNode(root);
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
                AppendIR("let " + root->Children[0]->NodeToken->Text);
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
                AppendIR("return");
            }
        }
        void CompileFunction(Node* root)
        {
            AppendIR("func " + root->Children[0]->NodeToken->Text);
        }
    private:
};