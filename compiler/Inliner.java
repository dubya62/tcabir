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
        //
        // <<
        // >>
        // <<=
        // >>=
        
        for (int i=0; i<tokens.size(); i++){
            switch (tokens.get(i)){
                case "+":
                case "/":
                case "*":
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
                case "<":
                case ">":
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

        result = convertPrefixAndPostfix(result);

        return result;
    }


    private ArrayList<String> convertPrefixAndPostfix(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // combine the tokens
        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("-") || tokens.get(i).equals("+")){
                if (i+1 < tokens.size()){
                    if (tokens.get(i).equals(tokens.get(i+1))){
                        result.add(tokens.get(i) + tokens.get(i+1));
                        i++;
                        continue;
                    }
                }
            }
            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> breakOperations(ArrayList<String> tokens){
        // break stuff from if statements
        ArrayList<String> result = breakOperationsFromIfs(tokens);

        // break stuff out of return statements
        result = breakOperationsFromReturns(result);
        
        // break stuff out of function calls
        result = breakOperationsFromFunctionCalls(result);

        // convert unary operators to binary
        result = convertUnaryOperators(result);
        result = convertCallsAndAccesses(result);
        
        // break stuff out of normal lines
        //result = breakMultipleOperations(result);

        return result;
    }

    
    private ArrayList<String> convertUnaryOperators(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // unary operators:
        // * - 0 deref
        // & - 0 ref
        // ! - 0 lognot
        // ~ - 0 bitnot
        // - - 0 -
        String[] unaryOperators = {"*", "&", "!", "~", "-", "++", "--"};
        Set<String> unaryOperatorsSet = new HashSet<>();
        unaryOperatorsSet.addAll(Arrays.asList(unaryOperators));
        
        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "=", ";", "++", "--"};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        for (int i=0; i<tokens.size(); i++){
            // find unary operators
            if (unaryOperatorsSet.contains(tokens.get(i))){
                // if the thing before this is an operator, this is unary
                if (tokens.get(i).equals("++") || tokens.get(i).equals("--")){
                    if (i > 0 && tokens.get(i-1).length() > 0 && tokens.get(i-1).charAt(0) == '#'){
                        // postfix
                        result.add(tokens.get(i));
                        result.add("0");
                    } else {
                        // prefix
                        result.add("0");
                        result.add(tokens.get(i));
                    }
                    continue;
                } else if (i > 0 && operatorsSet.contains(tokens.get(i-1))){
                    result.add("0");
                    switch(tokens.get(i)){
                        case "*":
                            result.add("deref");
                            break;
                        case "&":
                            result.add("ref");
                            break;
                        case "!":
                            result.add("lognot");
                            break;
                        case "~":
                            result.add("bitnot");
                            break;
                        case "-":
                            result.add("-");
                            break;
                    }
                    continue;
                }
            }

            result.add(tokens.get(i));
        }

        return result;
    }

    private ArrayList<String> convertCallsAndAccesses(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "=", "deref", "ref", "lognot", "bitnot"};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        // () - call
        // [] - access

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("(") || tokens.get(i).equals("[")){
                // token before needs to be a var
                if (i > 0){
                    if (tokens.get(i-1).length() > 0 && tokens.get(i-1).charAt(0) == '#'){
                        // cannot be a definition
                        if (i-1 > 0 && tokens.get(i-2).equals("def")){
                        } else {
                            switch(tokens.get(i)){
                                case "(":
                                    result.add("call");
                                    break;
                                case "[":
                                    result.add("access");
                                    // replace the [ with (
                                    tokens.set(i, "(");
                                    int returnIndex = i;
                                    int openBrackets = 1;
                                    while (i < tokens.size()){
                                        if (tokens.get(i).equals("[")){
                                            openBrackets++;
                                        } else if (tokens.get(i).equals("]")){
                                            openBrackets--;
                                            if (openBrackets == 0){
                                                tokens.set(i, ")");
                                                break;
                                            }
                                        }
                                        i++;
                                    }
                                    i = returnIndex;
                                    break;
                            }


                        }

                    }
                }
            }

            result.add(tokens.get(i));
        }

        return result;
    }
    

    private ArrayList<String> breakOperationsFromIfs(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // make sure that there is at most 1 operation per line
        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "="};
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
                    result.add("def");
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
        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "="};
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
                    result.add("def");
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


    private ArrayList<String> breakOperationsFromFunctionCalls(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "="};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        ArrayList<String> newArgs = new ArrayList<>();

        for (int i=0; i < tokens.size(); i++){
            // need (not def)(variable_name)(...)
            if (tokens.get(i).length() > 0){
                if (tokens.get(i).charAt(0) == '#'){
                    if (i > 0){
                        if (!tokens.get(i-1).equals("def")){
                            if (i+1 < tokens.size() && tokens.get(i+1).equals("(")){
                                int placePoint = i;
                                while (placePoint > 0){
                                    if (tokens.get(placePoint).equals(";") || tokens.get(placePoint).equals("{")){
                                        placePoint++;
                                        break;
                                    }
                                    placePoint--;
                                }

                                // stuff inside the parenthesis are the arguments
                                int startingIndex = i;
                                i++;
                                int openParens = 0;
                                ArrayList<String> currentArg = new ArrayList<>();
                                while (i < tokens.size()){
                                    if (tokens.get(i).equals("(")){
                                        openParens++;
                                        if (openParens == 1){
                                            i++;
                                            continue;
                                        }
                                    } else if ((tokens.get(i).equals(",") || tokens.get(i).equals(")")) && openParens == 1){
                                        // this breaks up args
                                        // if there are any operators, replace this arg with new token
                                        int numOperators = 0;
                                        for (int j=0; j<currentArg.size(); j++){
                                            if (operatorsSet.contains(currentArg.get(j))){
                                                numOperators++;
                                            }
                                        }
                                        
                                        if (numOperators > 0){
                                            currentArg.add(0, "=");
                                            currentArg.add(0, "#" + this.finalVarnum);
                                            currentArg.add(0, "def");
                                            newArgs.add("#" + this.finalVarnum);
                                            this.finalVarnum++;
                                            currentArg = breakOperationsFromFunctionCalls(currentArg);
                                            for (int j=0; j<currentArg.size(); j++){
                                                result.add(placePoint, currentArg.get(j));
                                                placePoint++;
                                            }
                                            result.add(placePoint, ";");
                                            placePoint++;
                                        } else {
                                            for (int j=0; j<currentArg.size(); j++){
                                                newArgs.add(currentArg.get(j));
                                            }
                                        }


                                        currentArg = new ArrayList<>();

                                        if (tokens.get(i).equals(")")){
                                            break;
                                        }
                                        i++;
                                        continue;
                                    } else if (tokens.get(i).equals(")")){
                                        openParens--;
                                        if (openParens == 0){
                                            break;
                                        }
                                    }

                                    currentArg.add(tokens.get(i));

                                    i++;
                                }
                                int finalIndex = i+1;
                                i = startingIndex;

                                result.add(tokens.get(i));
                                result.add("(");
                                for (int j=0; j < newArgs.size(); j++){
                                    if (j != 0){
                                        result.add(",");
                                    }
                                    result.add(newArgs.get(j));
                                }
                                result.add(")");
                                i = finalIndex;

                                if (finalIndex >= tokens.size()){
                                    break;
                                }
                            }
                        }
                    }
                }
            }


            result.add(tokens.get(i));
        }

        return result;
    }


    /**
     * Break up normal lines that have more than one operation using operator precedence
     */
    private ArrayList<String> breakMultipleOperations(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "=", "deref", "ref", "lognot", "bitnot", "call", "access", "++", "--"};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        // TODO: 
        // Create a HashMap with each operator's precedence for ordering


        return result;
    }


    private ArrayList<String> convertReturns(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // add an argument to each function that is a pointer to the return type

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("def")){
                if (i+2 < tokens.size() && tokens.get(i+1).length() > 0 && tokens.get(i+1).charAt(0) == '#'){
                    if (tokens.get(i+2).equals("(")){
                        result.add(tokens.get(i));
                        result.add(tokens.get(i+1));
                        result.add(tokens.get(i+2));
                        result.add("def");
                        result.add("#" + this.finalVarnum);
                        // TODO: make type of this variable a pointer to the return type
                        if (i+3 < tokens.size() && !tokens.get(i+3).equals(")")){
                            result.add(",");
                        }
                        i += 2;
                        int returnIndex = i;

                        // TODO: look for all returns inside this function and replace
                        //      return x;
                        // WITH:
                        //      *returnVar = x
                        //      return;

                        this.finalVarnum++;
                        continue;
                    }
                }
            }
            result.add(tokens.get(i));
        }

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


