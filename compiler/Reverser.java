/**
 * The purpose of this file is to go in reverse,
 * converting from the IR back to C/Cuda
 */
import java.util.ArrayList;

public class Reverser{
    public ArrayList<String> tokens;


    public static void main(String[] args){
        Reverser reverser = new Reverser(new ArrayList<String>());

    }


    public Reverser(ArrayList<String> tokens){
        this.tokens = tokens;

        String result = convertToC(tokens);
        System.out.println(result);
    }


    public String convertToC(ArrayList<String> tokens){
        String result = "";

        result += "int* intStack[4096];\n";
        result += "char* charStack = (char*) intStack;\n";
        result += "float* floatStack = (float*) intStack;\n";
        result += "double* doubleStack = (double*) intStack;\n";
        result += "long* longStack = (long*) intStack;\n";
        result += "short* longStack = (short*) intStack;\n";
        result += "void** pointerStack = (void**) intStack;\n";

        result += "\n";
        result += "int main(int argc, char** argv){\n";
        result += "\treturn 0;\n";
        result += "}\n";

        return result;
    }



}
