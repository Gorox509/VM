#ifndef VM_CMDS
#define VM_CMDS

enum VM_commands {
    PUSH = 1,
    ADD,
    SUB,
    MUL,
    DIV,
    OUT,
    HLT,

    PUSHR,
    POPR,
    JMP,
    JA,
    JAE,
    JB,
    JBE,
    JE,
    JNE,

    IN,
    CALL,
    RET,

    UNKNOWN,
};

#endif
