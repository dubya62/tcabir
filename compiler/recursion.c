


int recursive(int testNum){
    if (testNum == 0){
        return 1;
    } else if (testNum == 1){
        return 2;
    }
    return recursive(testNum-1);
}

int recursive2(int testNum){
    if (testNum == 100){
        // jump back to beginning with these different parameters. when done, jump back to here
        recursive2(testNum-1);
        return 100;
    }
    return recursive2(testNum) + recursive2(testNum+1);
}

/*

int recursive(int testNum){
    int currVal;
    while (currVal != 0){
        currVal = currVal-1;
    }
    currVal = 1;
    return currVal;
}


int recursive2(int testNum){
    while(1){
        if (testNum == 100){
            testNum -= 1;
            continue;

        }
    }
    if (testNum == 100){
        recursive2(testNum-1);
        return 100;
    }
    return recursive2(testNum) + recursive2(testNum+1);
}




*/


int main(int argc, char** argv){
    return 0;
}
