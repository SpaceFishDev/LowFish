// #ifndef BYTECODE_H
// #define BYTECODE_H
// #include<stdint.h>
// #include<stdlib.h>
// #include<stdio.h>
// #include<string.h>

// typedef enum opcode
// {
//     NATIVE,
// 	// all of the commANDs prefixed by 'SET' are immediate values.
// 	SET_EAX,
// 	SET_AX,
// 	SET_AL,
// 	SET_EDX,
// 	SET_ECX,
// 	SET_DX,
// 	SET_CX,
// 	SET_CL,
// 	SET_DL,
// 	SET_EBX,
// 	SET_BX,
// 	SET_BL,
// 	SET_EDI,
// 	SET_ESI,
// 	SET_EBP,
// 	SET_ESP,
// 	DECL_LOCATION,
	
// 	JUMP_LOCATION,
// 	PUSH_EAX,
// 	POP_EAX,
// 	PUSH_EDX,	POP_EDX,
// 	PUSH_ECX,
// 	POP_ECX,
// 	PUSH_EBX,
// 	POP_EBX,
// 	SET_ADDR_EAX,
// 	SET_ADDR_EBX, // sets the data inside of the address.
// 	// all of the following prefixed by 'MOV' move the EAX register into the register selected, or its lower equivalents like AX or AL.
// 	MOV_EBX,
// 	MOV_ECX,
// 	MOV_EDX,
// 	MOV_EAX_PTR,
// 	MOV_ESI,
// 	MOV_EDI,
// 	MOV_EBP,
// 	MOV_ESP,
// 	CMP_EAX_EBX,
// 	CMP_EAX_EDX,
// 	CMP_EAX_ECX,
// 	CMP_EAX_ESI,
// 	CMP_EAX_IMM, // Compares EAX with immediate value.
// 	JNE,
// 	JE,
// 	JZ,
// 	JG,
// 	JGE,
// 	JLE,
// 	JL,
// 	NOP,
// 	// 'ADD' 'SUB' 'MUL' 'DIV' all act on EAX AND put the result into the EAX register.
// 	ADD_EBX,
// 	ADD_ECX,
// 	ADD_EAX,
// 	ADD_EDX,
// 	ADD_ESI,
// 	ADD_EDI,
// 	SUB_EBX,
// 	SUB_ECX,
// 	SUB_EAX,
// 	SUB_EDX,
// 	SUB_ESI,
// 	MUL_EBX,
// 	MUL_ECX,
// 	MUL_EAX,
// 	MUL_EDX,
// 	MUL_ESI,
// 	DIV_EBX,
// 	DIV_ECX,
// 	DIV_EAX,
// 	DIV_EDX,
// 	DIV_ESI,
// 	CALL_FUNC,
// 	DECL_FUNC,
// 	DECL_ARG,
// 	DECL_VAR,
// 	PUSH_VAR,
// 	POP_VAR,
// 	VAR_MOV_EAX,
// 	VAR_MOV_EBX,
// 	VAR_MOV_EDX,
// 	VAR_MOV_ECX,
// 	VAR_MOV_ESI,
// 	VAR_SET_IMM,
// 	EAX_MOV_VAR,
// 	ECX_MOV_VAR,
// 	EDX_MOV_VAR,
// 	EBX_MOV_VAR,
// 	ESI_MOV_VAR,
// } opcode_t;

// typedef enum value_type
// {
//     NONE,
// } value_type_t;

// typedef struct value
// {
// 	value_type_t type;
// 	size_t size_of_data;
// 	char* data;
// } value_t;

// typedef struct operation
// {
// 	opcode_t opcode;
// 	size_t n_immediate_value;
// 	value_t* immediate_values;
// } operation_t;

// char* compile_all(operation_t* operations, size_t num_operation);  
// char* compile(operation_t operation);

// #endif
