/**
 * The purpose of this file is to inline all functions and convert recursive functions into iterative versions
 */
import java.util.ArrayList;
import java.util.Set;
import java.util.HashSet;
import java.util.Arrays;

public class Inliner{
    public ArrayList<String> tokens;
    public int finalVarnum;

    public Inliner(ArrayList<String> tokens, int finalVarnum){
        Main.debug("Starting the inliner...");
        this.tokens = tokens;
        this.finalVarnum = finalVarnum;

        // convert multitoken operations into single token operations
        this.tokens = combineMultiTokenOperations(this.tokens);

        // break operations
        this.tokens = breakOperations(this.tokens);

        // convert returns
        this.tokens = convertReturns(this.tokens);

        // inline all functions
        //this.tokens = inlineFunctions(this.tokens);

        Main.debug("Inliner is finished!");
        Main.debug("Inliner output:");
        Main.debug(this.tokens.toString());
    }

    private ArrayList<String> combineMultiTokenOperations(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // multi-token operators
        // +=
        // /=
        // *=
        // <=
        // >=
        // !=
        // ==
        // %=
        // ^=
        // &=
        // |=
        // ~=
        //
        // -=
        // ->
        // &&
        // ||
        //
        // &&=
        // ||=
        
        for (int i=0; i<tokens.size(); i++){
            switch (tokens.get(i)){
                case "+":
                case "/":
                case "*":
                case "<":
                case ">":
                case "!":
                case "=":
                case "%":
                case "^":
                case "~":
                    if (i+1 < tokens.size()){
                        if (tokens.get(i+1).equals("=")){
                            result.add(tokens.get(i) + tokens.get(i+1));
                            i++;
                            continue;
                        }
                    }
                    result.add(tokens.get(i));
                    break;
                case "-":
                    if (i+1 < tokens.size()){
                        if (tokens.get(i+1).equals("=") || tokens.get(i+1).equals(">")){
                            result.add(tokens.get(i) + tokens.get(i+1));
                            i++;
                            continue;
                        }
                    }
                    result.add(tokens.get(i));
                    break;
                case "&":
                case "|":
                    if (i+1 < tokens.size()){
                        if (tokens.get(i+1).equals("=")){
                            result.add(tokens.get(i) + tokens.get(i+1));
                            i++;
                            continue;
                        }
                        if (tokens.get(i+1).equals(tokens.get(i))){
                            if (i+2 < tokens.size() && tokens.get(i+2).equals("=")){
                                result.add(tokens.get(i) + tokens.get(i+1) + tokens.get(i+2));
                                i+=2;
                                continue;
                            }
                            result.add(tokens.get(i) + tokens.get(i+1));
                            i++;
                            continue;
                        }
                    }
                    result.add(tokens.get(i));
                    break;

                default:
                    result.add(tokens.get(i));
                    break;
            }

        }

        return result;
    }


    private ArrayList<String> breakOperations(ArrayList<String> tokens){
        // break stuff from if statements
        ArrayList<String> result = breakOperationsFromIfs(tokens);

        // break stuff out of return statements
        result = breakOperationsFromReturns(result);
        
        // break stuff out of function calls
        
        // break stuff out of normal lines

        return result;
    }
    

    private ArrayList<String> breakOperationsFromIfs(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // make sure that there is at most 1 operation per line
        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "("};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));
        
        // break stuff out of if statements
        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("if")){
                ArrayList<String> betweenParenthesis = new ArrayList<>();
                // get tokens between the parenthesis
                if (i+1 < tokens.size() && tokens.get(i+1).equals("(")){
                    i += 2;
                    int openParens = 1;
                    while (i < tokens.size()){
                        if (tokens.get(i).equals("(")){
                            openParens++;
                        } else if (tokens.get(i).equals(")")){
                            openParens--;
                            if (openParens == 0){
                                break;
                            }
                        }
                        betweenParenthesis.add(tokens.get(i));
                        i++;
                    }
                }
                int endParenths = i;

                // see if there are more than one operator/function call
                int numOperators = 0;
                for (int j=0; j<betweenParenthesis.size(); j++){
                    if (operatorsSet.contains(betweenParenthesis.get(j))){
                        // this is an operator
                        numOperators++;
                    }
                }

                // we need to break this line out
                if (numOperators > 0){
                    result.add("#" + this.finalVarnum);
                    result.add("=");
                    for (int j=0; j<betweenParenthesis.size(); j++){
                        result.add(betweenParenthesis.get(j));
                    }
                    result.add(";");
                    result.add("if");
                    result.add("(");
                    result.add("#" + this.finalVarnum);
                    this.finalVarnum++;
                    i = endParenths;
                }

            }
            result.add(tokens.get(i));
        }


        return result;
    }


    private ArrayList<String> breakOperationsFromReturns(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // make sure that there is at most 1 operation per line
        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "("};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("return")){
                // get the tokens between return and ;
                ArrayList<String> betweenStatements = new ArrayList<>();
                i++;
                int startingIndex = i;
                while (i < tokens.size()){
                    if (tokens.get(i).equals(";")){
                        break;
                    }
                    betweenStatements.add(tokens.get(i));
                    i++;
                }
                int finalIndex = i;

                // if there are any operators, move to the line before
                int operatorCount = 0;
                for (int j=0; j<betweenStatements.size(); j++){
                    if (operatorsSet.contains(betweenStatements.get(j))){
                        operatorCount++;
                    }
                }

                if (operatorCount > 0){
                    result.add("#" + this.finalVarnum);
                    result.add("=");
                    for (int j=0; j<betweenStatements.size(); j++){
                        result.add(betweenStatements.get(j));
                    }
                    result.add(";");
                    result.add("return");
                    result.add("#" + this.finalVarnum);
                    result.add(";");

                    this.finalVarnum++;
                    i = finalIndex+1;
                } else {
                    result.add("return");
                    i = startingIndex;
                }
            }
            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> convertReturns(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // add an argument to each function that is a pointer to the return type

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }
        // instead of 

        return result;
    }


    private ArrayList<String> inlineFunctions(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        for (int i=0; i<tokens.size(); i++){
            // figure out if this token is a function definition
            int startingIndex = i;
            if (tokens.get(i).length() > 0){
                if (tokens.get(i).charAt(0) == '#'){
                    String functionName = tokens.get(i);
                    if (i+1 < tokens.size()){
                        if (tokens.get(i+1).equals("(")){
                            // tokens[i] is the function's name
                            // look for references to this function inside of its own scope
                            int openParens = 0;
                            while (i < tokens.size()){
                                if (tokens.get(i).equals("(")){
                                    openParens++;
                                } else if (tokens.get(i).equals(")")){
                                    openParens--;
                                    if (openParens == 0){
                                        break;
                                    }
                                }
                                i++;
                            }

                            i++;
                            if (i < tokens.size() && tokens.get(i).equals("{")){
                            } else {
                                i = startingIndex;
                                result.add(tokens.get(i));
                                continue;
                            }
                            
                            int openBraces = 0;
                            boolean isRecursive = false;
                            while (i < tokens.size()){
                                if (tokens.get(i).equals("{")){
                                    openBraces++;
                                } else if (tokens.get(i).equals("}")){
                                    openBraces--;
                                    if (openBraces == 0){
                                        break;
                                    }
                                } else if (tokens.get(i).equals(functionName)){
                                    // this is a recursive function
                                    isRecursive = true;
                                }
                                i++;
                            }


                        }
                    }
                }
            }
        }

        return result;
    }
}


