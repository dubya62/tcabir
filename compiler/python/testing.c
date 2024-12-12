

int testFunc(int b){
    return b;
}


int main(int argc, char** argv){

    testFunc(testFunc(argc)) + testFunc(4);

    int i = 0;
    ++i;

    return 0;
}


