#ifndef VM_CMDS
#define VM_CMDS

enum VM_commands {
    UNKNOWN = 0,
    
    #define GENERATE(cmd) cmd,

    #include "generate.txt"

    #undef GENERATE
};

#endif
