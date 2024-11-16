/**
 * Return the number of times that <letter> appears in <str>
 */
int getOccurancesOfLetter(char* str, char letter){
    int i = 0;
    int count = 0;
    while (1){
        if (str[i] == letter){
            count++;
        } else if (str[i] == '\0'){
            break;
        }
        i++;
    }
    return count;
}
