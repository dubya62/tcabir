/**
 * The purpose of this file is to simplify the C code to a representation 
 * closer to the IR
 */
import java.util.ArrayList;
import java.util.Set;
import java.util.HashSet;
import java.util.Arrays;
import java.util.HashMap;

public class Simplifier{
    public ArrayList<String> tokens;

    public Simplifier(ArrayList<String> tokens){
        Main.debug("Starting the Simplifier...");
        this.tokens = tokens;

        // create extra scopes to contain for blocks
        this.tokens = createExtraScopes(this.tokens);

        // convert all variables into numbered variables
        this.tokens = generalizeVariables(this.tokens);

        Main.debug("Simplifier Finsihed!");
        Main.debug("Simplifier output:");
        Main.debug(this.tokens.toString());
    }

    /**
     * Create extra scopes around if, for, switch, and while
     * so that initializers
     */ 
    private ArrayList<String> createExtraScopes(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        int openBraces = 0;
        boolean openedFor = false;
        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("{")){
                openBraces++;
            } else if (tokens.get(i).equals("}")){
                openBraces--;
                if (openBraces == 0){
                    if (openedFor){
                        result.add("}");
                        openedFor = false;
                    }
                }
            } else if (tokens.get(i).equals("for")){
                result.add("{");
                openedFor = true;
            }

            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> generalizeVariables(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // Array of sets to keep track of scopes
        ArrayList<HashMap<String, Integer>> scopes = new ArrayList<>();
        scopes.add(new HashMap<String, Integer>());
        int currentScope = 0;

        // keep track of which tokens are builtins and should not be replaced
        String[] builtinArray = {"*", "char", "int", "float", "double", "+", "-", "(", ")", ".", "[", "]", "{", "}", "<", ">", ",", "/", "=", "|", "%", "#", "!", "~", "^", "&", ";", ":", "return", "break", "void", "if", "else", "for", "while", "switch", "case", "short", "long", "const", "unsigned", "struct", "NULL", "signed", "sizeof"};

        Set<String> builtins = new HashSet<>();
        builtins.addAll(Arrays.asList(builtinArray));


        int varnum = 0;
        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("{")){
                currentScope++;
                scopes.add(new HashMap<String, Integer>());
            } else if (tokens.get(i).equals("}")){
                currentScope--;
                scopes.remove(scopes.size()-1);
            }

            String currentString = tokens.get(i);
            int isLiteral = 0;
            if (!builtins.contains(currentString)){
                // do not convert string, char, int, or float literals
                try {
                    Integer.parseInt(currentString);
                    isLiteral = 1;
                } catch (Exception e){}

                try {
                    Float.parseFloat(currentString);
                    isLiteral = 1;
                } catch (Exception e){}

                // if it starts with " or ' it is a string/char
                if (currentString.length() > 0){
                    if (currentString.charAt(0) == '"' || currentString.charAt(0) == '\''){
                        isLiteral = 1;
                    }
                }

                if (isLiteral != 0){
                    result.add(currentString);
                    continue;
                }

                // check if this variable has already been used within this scope
                boolean existed = false;
                for (int j=0; j<scopes.size(); j++){
                    if (scopes.get(j).containsKey(currentString)){
                        result.add("#" + scopes.get(j).get(currentString));
                        existed = true;
                        break;
                    }
                }
                if (!existed){
                    scopes.get(scopes.size()-1).put(currentString, varnum);
                    result.add("#" + varnum);
                    varnum++;
                }
            } else {
                result.add(currentString);
            }
        }

        return result;
    }
}

