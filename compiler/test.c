/**
 * Return the number of times that <letter> appears in <str>
 */


struct TestStruct{
    int test1;
    int test2;
    char test3;
};

int getNumberOfOccurances(char* str, char letter){

    char* test[5] = {};

    struct TestStruct testStruct = {1, 2, 'c'};

    int count = 0;
    int i=0;
    while (1){
        if (str[i] == '\0'){
            break;
        } else if (str[i] == letter){
            count++;
        }
        i++;
    }
    return count;
}
int main(void){
    return 0;
}
