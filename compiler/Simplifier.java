/**
 * The purpose of this file is to simplify the C code to a representation 
 * closer to the IR
 */
import java.util.ArrayList;
import java.util.Set;
import java.util.HashSet;
import java.util.Arrays;

public class Simplifier{
    public ArrayList<String> tokens;

    public Simplifier(ArrayList<String> tokens){
        Main.debug("Starting the Simplifier...");
        this.tokens = tokens;

        // convert all variables into numbered variables
        this.tokens = generalizeVariables(this.tokens);

        Main.debug("Simplifier Finsihed!");
        Main.debug("Simplifier output:");
        Main.debug(this.tokens.toString());
    }

    private ArrayList<String> generalizeVariables(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // keep track of which tokens are builtins and should not be replaced
        String[] builtinArray = {"*", "char", "int", "float", "double", "+", "-", "(", ")", ".", "[", "]", "{", "}", "<", ">", ",", "/", "=", "|", "%", "#", "!", "~", "^", "&", ";", ":", "return", "break", "void", "if", "else", "for", "while", "switch", "case", "short", "long", "const", "unsigned", "struct", "NULL", "signed"};

        Set<String> builtins = new HashSet<>();
        builtins.addAll(Arrays.asList(builtinArray));

        int varnum = 0;
        for (int i=0; i<tokens.size(); i++){
            String currentString = tokens.get(i);
            if (!builtins.contains(currentString)){
                // do not convert string, char, int, or float literals
                
                // if it starts with "/' it is a string/char


                result.add("#" + varnum);
                varnum++;
            } else {
                result.add(currentString);
            }
        }

        return result;
    }
}
