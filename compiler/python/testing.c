

#define dbg(message) printf("%s", message)

struct TestStruct{
    int a;
};

int main(int argc, char** argv){
    int test = 4;
    for (int i=0; i<test; i++){
        i += 2;
    }

    return 0;
}


