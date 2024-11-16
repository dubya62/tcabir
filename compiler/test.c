/**
 * Return the number of times that <letter> appears in <str>
 */
int getNumberOfOccurances(char* str, char letter){

    char* test[5] = {};

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
