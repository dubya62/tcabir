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

    public Jumper(ArrayList<String> tokens, int finalVarnum){
        this.tokens = tokens;
        this.finalVarnum = finalVarnum;

        Main.debug("Starting the Jumper...");

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


        Main.debug("Jumper is finished!");
        Main.debug("Jumper output:");
        Main.debug(this.tokens.toString());
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


}
