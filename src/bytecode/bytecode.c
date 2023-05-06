// #include<bytecode/bytecode.h>


// char* append(char* a , char* b)
// {
// 	size_t len_a = strlen(a);
// 	size_t len_b = strlen(b);
// 	a = realloc(a, len_a + len_b + 1);
// 	memcpy(&a[len_a], b, len_b);
// 	a[len_a + len_b] = 0;
// }
// char** append_list(size_t a_len, char** a, char* b)
// {
// 	a = realloc(a, sizeof(b) + (sizeof(b) * a_len));
// 	a[(sizeof(b) + (sizeof(b) * a_len)) - 1] = b;
// 	return a;	
// }

// typedef struct compiler
// {
// 	size_t n_label;
// 	size_t n_function;
// 	char** labels;
// 	char** functions;
// } compiler_t;

// char* compile(operation_t operation, compiler_t compiler)
// {
// 	switch(operation.type)
// 	{
// 		case NATIVE:
// 		{
// 			return operation.args[0];
// 		}
// 		case JMP:
// 		{
// 			for(size_t i = 0; i < n_label; ++i)
// 			{
// 				if(!strcmp(compiler.labels[i], operation.args[0]))
// 				{
// 					char* a = "jmp ";
// 					char* b = compiler.labels[i];
// 					a = append(a,b);
// 					return a;	
// 				}	
// 			}
// 		} break;
// 	}
// }

// char* compile_all(operation_t* operations, size_t num_operation)
// {
// 	compiler_t compiler = {};
// 	char* result = "";
// 	for(size_t i = 0; i < num_operation; ++i)
// 	{
// 		append(result, compile(operations[i], compiler));		
// 	}
// 	return result;
// }

