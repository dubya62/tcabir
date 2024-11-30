/**
 * Convert from function calls to jumps using a software stack
 */
import java.util.ArrayList;
import java.util.Stack;
import java.util.Set;
import java.util.HashSet;
import java.util.HashMap;
import java.util.Arrays;


public class Jumper{
    public ArrayList<String> tokens;
    public int finalVarnum;
    private HashMap<String, Integer> functionMap;
    private int lastLabel;
    private HashMap<String, Integer> functionStackSpaces;

    public Jumper(ArrayList<String> tokens, int finalVarnum){
        this.tokens = tokens;
        this.finalVarnum = finalVarnum;

        Main.debug("Starting the Jumper...");

        this.functionStackSpaces = new HashMap<>();

        this.lastLabel = getLastLabel(this.tokens);

        // the stack will just be an abstraction for now.
        // it will be completely disconnected from types and will use an index
        // to just get whatever local variable it needs
        // typing will be handled later

        // new things that can appear
        //     PUSH <val/var>
        //     RBP = <val>
        //     RSP = <val>
        //     STACK access RBP
        
        // all functions should end with a return statement
        this.tokens = addReturnStatements(this.tokens);

        // scan through the tokens looking for functions.
        // on return statement,
        //      RSP = RBP;
        //      RBP = STACK[RSP-1];
        //      jmp STACK[RSP-2]
        this.tokens = convertReturnStatements(this.tokens);

        // convert function headers to be labels and args to use stack
        this.tokens = convertFunctionHeaders(this.tokens);

        // Scan through the tokens looking for function calls
        // on function call,
        //      PUSH <new_label>;
        //      PUSH RBP;
        //      RBP = RSP;
        //      PUSH ARG2;
        //      PUSH ARG1;
        //      PUSH ARG0;
        //      RSP = RSP + <numVars>;
        //      jmp <function_code>;
        //      @<new_label>:

        this.tokens = convertFunctionCalls(this.tokens);


        // function calls are now all converted to jumps using the stack
        // clean up the code by removing unnecessary scopes, defs, and semicolons
        this.tokens = cleanUpRepresentation(this.tokens);


        Main.debug("Jumper is finished!");
        Main.debug("Jumper output:");
        Main.debug(this.tokens.toString());
    }


    private ArrayList<String> cleanUpRepresentation(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();
        // clean up the code by removing unnecessary scopes, defs, and semicolons
        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("def")){
                tokens.remove(i);
                i--;
            } else if (tokens.get(i).equals(";") && i+1 < tokens.size() && tokens.get(i+1).equals(";")){
                tokens.remove(i);
                i--;
            }
        }

        // remove unnecessary scopes (non-if statements)
        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("{")){
                boolean isIfStatement = false;
                if (i > 0 && tokens.get(i-1).equals(")")){
                    int startingIndex = i;
                    int openParens = 0;
                    while (i > 0){
                        if (tokens.get(i).equals(")")){
                            openParens++;
                        } else if (tokens.get(i).equals("(")){
                            openParens--;
                            if (openParens == 0){
                                break;
                            }
                        }
                        i--;
                    }
                    if (i > 0){
                        if (tokens.get(i-1).equals("if")){
                            isIfStatement = true;
                        }
                    }
                    i = startingIndex;
                } else if (i > 0 && tokens.get(i-1).equals("else")){
                    isIfStatement = true;
                }

                if (!isIfStatement){
                    // we can delete these braces
                    tokens.remove(i);
                    int openBraces = 1;
                    int returnIndex = i;
                    while (i < tokens.size()){
                        if (tokens.get(i).equals("{")){
                            openBraces++;
                        } else if (tokens.get(i).equals("}")){
                            openBraces--;
                            if (openBraces == 0){
                                tokens.remove(i);
                                break;
                            }
                        }
                        i++;
                    }
                    i = returnIndex-1;
                    continue;
                }

            }
        }

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }

        return result;
    }


    private int getLastLabel(ArrayList<String> tokens){
        int lastLabel = -1;
        for (int i=0; i<tokens.size(); i++){
            // look for next label value
            if (tokens.get(i).length() > 0){
                if (tokens.get(i).charAt(0) == '@'){
                    // this is a label. get the value
                    String integerPart = tokens.get(i).substring(1);
                    int labelValue = Integer.parseInt(integerPart);
                    if (labelValue > lastLabel){
                        lastLabel = labelValue;
                    }
                }
            }
        }
        lastLabel++;
        return lastLabel;
    }

    private ArrayList<String> convertFunctionHeaders(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        this.functionMap = new HashMap<>();

        String functionName = null;

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("(")){
                if (i > 0){
                    if (tokens.get(i-1).length() > 0 && tokens.get(i-1).charAt(0) == '#'){
                        HashMap<String, Integer> arguments = new HashMap<>();
                        int currentArgument = 0;
                        // this is a function definition
                        // token before is the function name
                        this.functionMap.put(tokens.get(i-1), this.lastLabel);
                        functionName = tokens.get(i-1);

                        // now map the arguments to the function
                        while (i < tokens.size()){
                            if (tokens.get(i).equals(")")){
                                break;
                            }
                            if (tokens.get(i).length() > 0 && tokens.get(i).charAt(0) == '#'){
                                arguments.put(tokens.get(i), currentArgument);
                                currentArgument++;
                            }
                            i++;
                        }

                        // we now know the arguments and label to jump to.
                        // delete the function header and replace it with the label.
                        while (i >= 0){
                            if (tokens.get(i).equals("{")){
                                break;
                            }
                            tokens.remove(i);
                            i--;
                        }
                        tokens.add(i, "@" + this.lastLabel);
                        i++;
                        tokens.add(i, ":");
                        i++;
                        this.lastLabel++;

                        // replace references to arguments with STACK references
                        int openBraces = 0;

                        HashMap<String, Integer> localVariables = new HashMap<>();
                        int variableNum = currentArgument;

                        while (i < tokens.size()){
                            if (tokens.get(i).equals("{")){
                                openBraces++;
                            } else if (tokens.get(i).equals("}")){
                                openBraces--;
                                if (openBraces == 0){
                                    break;
                                }
                            } else if (arguments.containsKey(tokens.get(i))){
                                tokens.set(i, "$" + arguments.get(tokens.get(i)));
                            } else if (tokens.get(i).length() > 0 && tokens.get(i).charAt(0) == '#'){
                                // this is a local variable
                                if (!localVariables.containsKey(tokens.get(i))){
                                    if (i > 0 && tokens.get(i-1).equals("def")){
                                        localVariables.put(tokens.get(i), variableNum);
                                        variableNum++;
                                        tokens.set(i, "$" + localVariables.get(tokens.get(i)));
                                    }
                                } else {
                                    tokens.set(i, "$" + localVariables.get(tokens.get(i)));
                                }
                                
                            }

                            i++;
                        }

                        this.functionStackSpaces.put(functionName, variableNum);
                    }
                }
            }
        }

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }

        result = breakMultipleOperations(result);
        result = removeExpressionDelimitters(result);

        return result;
    }


    private ArrayList<String> addReturnStatements(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("(")){
                if (i > 0){
                    if (tokens.get(i-1).length() > 0 && tokens.get(i-1).charAt(0) == '#'){
                        // this is a function def
                        int openBraces = 0;
                        while (i < tokens.size()){
                            // look for the end of the function block
                            if (tokens.get(i).equals("{")){
                                openBraces++;
                            } else if (tokens.get(i).equals("}")){
                                openBraces--;
                                if (openBraces == 0){
                                    break;
                                }
                            }
                            i++;
                        }
                        
                        if (i > 1){
                            if (!tokens.get(i-2).equals("return")){
                                // there is no return statement here
                                tokens.add(i, "return");
                                i++;
                                tokens.add(i, ";");
                                i++;
                            }
                        }


                    }
                }
            }
        }

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> convertReturnStatements(ArrayList<String> tokens){
        // scan through the tokens looking for functions.
        // on return statement,
        //      RSP = RBP;
        //      RBP = STACK[RSP-1];
        //      jmp STACK[RSP-2];
        ArrayList<String> result = new ArrayList<>();

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("return")){
                tokens.set(i, "RSP");
                i++;
                tokens.add(i, "=");
                i++;
                tokens.add(i, "RBP");
                i++;
                tokens.add(i, ";");
                i++;
                tokens.add(i, "def");
                i++;
                tokens.add(i, "#" + this.finalVarnum);
                i++;
                tokens.add(i, "=");
                i++;
                tokens.add(i, "STACK");
                i++;
                tokens.add(i, "access");
                i++;
                tokens.add(i, "(");
                i++;
                tokens.add(i, "RSP");
                i++;
                tokens.add(i, "-");
                i++;
                tokens.add(i, "2");
                i++;
                tokens.add(i, ")");
                i++;
                tokens.add(i, ";");
                i++;
                tokens.add(i, "RBP");
                i++;
                tokens.add(i, "=");
                i++;
                tokens.add(i, "STACK");
                i++;
                tokens.add(i, "access");
                i++;
                tokens.add(i, "(");
                i++;
                tokens.add(i, "RSP");
                i++;
                tokens.add(i, "-");
                i++;
                tokens.add(i, "1");
                i++;
                tokens.add(i, ")");
                i++;
                tokens.add(i, ";");
                i++;
                tokens.add(i, "jmp");
                i++;
                tokens.add(i, "#" + this.finalVarnum);

                this.finalVarnum++;
                
            }
        }

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }

        result = breakMultipleOperations(result);
        result = removeExpressionDelimitters(result);

        return result;
    }

    /**
     * Break up normal lines that have more than one operation using operator precedence
     */
    private ArrayList<String> breakMultipleOperations(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        String[] operators = {"%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "<=", ">=", "!=", "==", "&&", "||", ".", ">>", "<<", "=", "deref", "ref", "lognot", "bitnot", "call", "access", "pre++", "pre--", "post++", "post--", ","};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));


        // iterate through the lines of the file and use infix to postfix to break up the lines
        ArrayList<String> currentLine;
        for (int i=0; i<tokens.size(); i++){
            // each line should end with a semicolon or a {
            int operatorCount = 0;
            boolean wasNoEqual = true;
            int startingIndex = i;
            currentLine = new ArrayList<>();
            while (i < tokens.size()){
                if (tokens.get(i).equals("=")){
                    wasNoEqual = false;
                } else if (tokens.get(i).equals("{") || tokens.get(i).equals(";")){
                    // this is the end of a line
                    break;
                }

                if (operatorsSet.contains(tokens.get(i))){
                    operatorCount++;
                }

                currentLine.add(tokens.get(i));
                i++;
            }

            boolean containsFunctionDefinition = false;
            for (int j=0; j<currentLine.size(); j++){
                if (currentLine.get(j).equals("(")){
                    if (j > 0){
                        if (currentLine.get(j-1).length() > 0 && currentLine.get(j-1).charAt(0) == '#'){
                            containsFunctionDefinition = true;
                        }
                    }
                }
            }

            if ((operatorCount > 2 || (operatorCount > 1 && wasNoEqual)) && !(containsFunctionDefinition)){ 
                // this line needs to be broken up
                ArrayList<String> brokenLine = breakLineWithPostfix(currentLine);
                result.add("StartExpression");
                for (int j=0; j<brokenLine.size(); j++){
                    result.add(brokenLine.get(j));
                }
                result.add("EndExpression");
            } else {
                for (int j=0; j<currentLine.size(); j++){
                    if (containsFunctionDefinition){
                        result.add(currentLine.get(j));
                    } else if (!currentLine.get(j).equals("(") && !currentLine.get(j).equals(")")){
                        result.add(currentLine.get(j));
                    }
                }
            }

            if (i < tokens.size()){
                result.add(tokens.get(i));
            }
        }

        

        return result;
    }

    private ArrayList<String> breakLineWithPostfix(ArrayList<String> tokens){
        String[] operators = {"%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "<=", ">=", "!=", "==", "&&", "||", ".", ">>", "<<", "=", "deref", "ref", "lognot", "bitnot", "call", "access", "pre++", "pre--", "post++", "post--", ","};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        // Create a HashMap with each operator's precedence for ordering
        HashMap<String, Integer> precedences = new HashMap();

        precedences.put("post++", 30);
        precedences.put("post--", 30);

        precedences.put("call", 29);
        precedences.put("access", 28);

        precedences.put(".", 27);

        precedences.put("pre++", 26);
        precedences.put("pre--", 26);

        precedences.put("lognot", 25);
        precedences.put("bitnot", 25);
        precedences.put("deref", 24);
        precedences.put("ref", 23);

        precedences.put("*", 22);
        precedences.put("%", 22);
        precedences.put("/", 22);

        precedences.put("+", 21);
        precedences.put("-", 21);

        precedences.put("<<", 20);
        precedences.put(">>", 20);

        precedences.put("<", 19);
        precedences.put("<=", 19);
        precedences.put(">", 18);
        precedences.put(">=", 18);

        precedences.put("==", 17);
        precedences.put("!=", 17);

        precedences.put("&", 16);

        precedences.put("^", 15);

        precedences.put("|", 14);

        precedences.put("&&", 13);

        precedences.put("||", 12);


        precedences.put(",", 11);


        precedences.put("=", 10);

        System.out.println("BREAKING LINE: " + tokens.toString());

        ArrayList<String> postfixExpression = new ArrayList<>();
        Stack<String> operatorStack = new Stack<>();

        // handle if the first token is def
        boolean addDef = false;
        if (tokens.size() > 0){
            if (tokens.get(0).equals("def")){
                addDef = true;
                tokens.remove(0);
            }
        }

        for (int i=0; i<tokens.size(); i++){
            if (operatorsSet.contains(tokens.get(i))){
                // if the precedence of the current operator is higher than the operator on the top of the stack, 
                // or stack is empty, or stack contains a (  => push
                if (operatorStack.isEmpty() || operatorStack.peek().equals("(") || precedences.get(operatorStack.peek()) < precedences.get(tokens.get(i))){
                    operatorStack.push(tokens.get(i));
                } else {
                    // pop all operators from the stack that have a precedence higher than or equal to current operator, then push current operator
                    while (true){
                        if (operatorStack.isEmpty()){
                            break;
                        }
                        if (operatorStack.peek().equals("(") || precedences.get(operatorStack.peek()) < precedences.get(tokens.get(i))){
                            break;
                        }
                        postfixExpression.add(operatorStack.pop());
                    }
                    operatorStack.push(tokens.get(i));
                }
                
            } else if (tokens.get(i).equals("(")){
                // if (, push to stack
                operatorStack.push("(");
            } else if (tokens.get(i).equals(")")){
                // pop from the stack until (
                while (true){
                    if (operatorStack.isEmpty()){
                        System.out.println("Mismatched parenthesis...");
                        System.exit(1);
                    }

                    if (operatorStack.peek().equals("(")){
                        operatorStack.pop();
                        break;
                    }

                    postfixExpression.add(operatorStack.pop());

                }
            } else {
                // if this is an operand, put it in the postfix expression
                postfixExpression.add(tokens.get(i));
            }
        }

        while (!operatorStack.isEmpty()){
            postfixExpression.add(operatorStack.pop());
        }

        System.out.println("Resulting expression: " + postfixExpression.toString());


        ArrayList<String> result = new ArrayList<>();

        Stack<String> operandStack = new Stack<>();
        for (int i=0; i<postfixExpression.size(); i++){
            if (operatorsSet.contains(postfixExpression.get(i))){
                // this is an operator
                if (operandStack.isEmpty()){
                    System.out.println("Operand Stack was empty during evaluation of expression...");
                    System.exit(1);
                }
                String second = operandStack.pop();
                if (operandStack.isEmpty()){
                    System.out.println("Operand Stack was empty during evaluation of expression...");
                    System.exit(1);
                }
                String first = operandStack.pop();

                // if the operator is not =, create a new variable and set it to this expression
                if (!postfixExpression.get(i).equals("=")){
                    result.add("def");
                    result.add("#" + this.finalVarnum);
                    result.add("=");
                    operandStack.push("#" + this.finalVarnum);
                    this.finalVarnum++;
                } else if (addDef){
                    result.add("def");
                }
                result.add(first);
                result.add(postfixExpression.get(i));
                result.add(second);
                result.add(";");

            } else {
                // this is an operand
                operandStack.push(postfixExpression.get(i));
            }
        }
        while (!operandStack.isEmpty()){
            result.add(operandStack.pop());
            result.add(";");
        }

        System.out.println("Postfix Evalution: " + result.toString());

        return result;
    }


    private ArrayList<String> removeExpressionDelimitters(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();
        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("StartExpression") || tokens.get(i).equals("EndExpression")){
                continue;
            }
            result.add(tokens.get(i));
        }
        return result;
    }


    private ArrayList<String> convertFunctionCalls(ArrayList<String> tokens){
        // Scan through the tokens looking for function calls
        // on function call,
        //      PUSH <new_label>;
        //      PUSH RBP;
        //      RBP = RSP;
        //      PUSH ARG0;
        //      PUSH ARG1;
        //      PUSH ARG2;
        //      RSP = RSP + <numVars>;
        //      jmp <function_code>;
        //      @<new_label>:


        // we now know the next label value
        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("call")){
                int startingIndex = i;

                while (i < tokens.size()){
                    if (tokens.get(i).equals(";")){
                        i++;
                        break;
                    }
                    i++;
                }

                // we have reached the end of the call statement
                tokens.add(i, "PUSH");
                i++;
                tokens.add(i, "" + this.lastLabel);
                i++;
                tokens.add(i, ";");
                i++;
                tokens.add(i, "PUSH");
                i++;
                tokens.add(i, "RBP");
                i++;
                tokens.add(i, ";");
                i++;
                tokens.add(i, "RBP");
                i++;
                tokens.add(i, "=");
                i++;
                tokens.add(i, "RSP");
                i++;
                tokens.add(i, ";");
                i++;

                int returnIndex = i;

                // push the args
                // look backwards for the args to know which order to push them in (and what to push
                i = startingIndex;
                ArrayList<String> arguments = new ArrayList<>();
                Set<String> splittableArguments = new HashSet<>();

                arguments.add(tokens.get(i+1));
                splittableArguments.add(tokens.get(i+1));

                while (i > 0){
                    if (splittableArguments.contains(tokens.get(i))){
                        // look for tokens.get(i) = #<> , #<>
                        // and split if found. not splittable otherwise
                        splittableArguments.remove(tokens.get(i));
                        if (i+4 < tokens.size()){
                            if (tokens.get(i+3).equals(",")){
                                splittableArguments.add(tokens.get(i+2));
                                splittableArguments.add(tokens.get(i+4));

                                int theArgIndex = arguments.indexOf(tokens.get(i));
                                arguments.set(theArgIndex, tokens.get(i+4));
                                arguments.add(theArgIndex, tokens.get(i+2));

                                // remove this line
                                while (i < tokens.size() && !tokens.get(i).equals(";")){
                                    tokens.remove(i);
                                    startingIndex--;
                                    returnIndex--;
                                }
                                i--;
                                while (i > 0){
                                    if (tokens.get(i).equals(";") || tokens.get(i).equals(":") || tokens.get(i).equals("{") || tokens.get(i).equals("}")){
                                        break;
                                    }
                                    tokens.remove(i);
                                    startingIndex--;
                                    returnIndex--;
                                    i--;
                                }
                                continue;
                            }
                        }
                        if (splittableArguments.isEmpty()){
                            break;
                        }
                    }
                    i--;
                }

                i = returnIndex;

                for (int j=0; j<arguments.size(); j++){
                    tokens.add(i, "PUSH");
                    i++;
                    tokens.add(i, arguments.get(j));
                    i++;
                    tokens.add(i, ";");
                    i++;
                }


                // figure out the number of variables a function might need
                int numberOfVariables = this.functionStackSpaces.get(tokens.get(startingIndex-1));

                // figure out function label
                int functionLabel = this.functionMap.get(tokens.get(startingIndex-1));

                //      RSP = RSP + <numVars>;
                //      jmp <function_code>;
                //      @<new_label>:
                tokens.add(i, "RSP");
                i++;
                tokens.add(i, "=");
                i++;
                tokens.add(i, "RSP");
                i++;
                tokens.add(i, "+");
                i++;
                tokens.add(i, "" + numberOfVariables);
                i++;
                tokens.add(i, ";");
                i++;
                tokens.add(i, "jmp");
                i++;
                tokens.add(i, "" + functionLabel);
                i++;
                tokens.add(i, ";");
                i++;
                tokens.add(i, "@" + this.lastLabel);
                i++;
                tokens.add(i, ":");
                i++;

                // remove line where the function was called
                i = startingIndex;
                while(i > 0){
                    if (tokens.get(i).equals(";") || tokens.get(i).equals(":") || tokens.get(i).equals("{") || tokens.get(i).equals("}")){
                        break;
                    }
                    tokens.remove(i);
                    i--;
                }
                i++;
                while (i < tokens.size()){
                    if (tokens.get(i).equals(";")){
                        break;
                    }
                    tokens.remove(i);
                }



                this.lastLabel++;
            }
        }


        return tokens;
    }


}
