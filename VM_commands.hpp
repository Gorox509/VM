#ifndef VM_CMDS
#define VM_CMDS

enum VM_commands {
    #define GENERATE(cmd) cmd,

    #include "generate.txt"
    UNKNOWN,

    #undef GENERATE
};

#endif
