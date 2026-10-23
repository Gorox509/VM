#include "compiler.hpp"

int main(int argc, char *argv[]) { // TODO: to file
    if (argc != 3) {
        fprintf(stderr, "Error: wrong arguments");
        return 1;
    }
    char *file_in  = argv[1];
    char *file_out = argv[2];

    compile(file_in, file_out);

    return 0;
}
