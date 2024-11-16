/**
 * Return the number of times that <letter> appears in <str>
 */
int main(int argc, char** argv){
    int c = 2;
    if (argc > 0){
        int b = 3;
        printf("%s: %d: %d", argv[0], b, c);
    } else {
        int b = 4;
        b <<= 2;
        c >>= 1;
    }
}
