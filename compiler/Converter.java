/**
 * The purpose of this file is to handle the final conversion from C
 * to the IR
 */
import java.util.ArrayList;
import java.util.Stack;

public class Converter{
    public ArrayList<String> tokens;
    private int currentLabel;

    public Converter(ArrayList<String> tokens){
        Main.debug("Starting the converter...");
        this.tokens = tokens;

        // Convert breaks into jumps to the end of loops
        this.tokens = convertBreaks(this.tokens);

        // Convert while loops to if statements and jumps
        this.tokens = convertWhileLoops(this.tokens);

        // Convert for loops to if statements and jumps
        this.tokens = convertForLoops(this.tokens);

        Main.debug("Converter Finished!");
        Main.debug("Converter output:");
        Main.debug(this.tokens.toString());
    }

    private ArrayList<String> convertBreaks(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        Stack<Integer> braceLevels = new Stack<>();
        int openBraces = 0;

        for (int i=0; i<tokens.size(); i++){

            if (tokens.get(i).equals("for")){
                braceLevels.push(openBraces);
            } else if (tokens.get(i).equals("while")){
                braceLevels.push(openBraces);
            } else if (tokens.get(i).equals("switch")){
                braceLevels.push(openBraces);
            } else if (tokens.get(i).equals("{")){
                openBraces++;
            } else if (tokens.get(i).equals("}")){
                openBraces--;

                if (!braceLevels.empty() && openBraces == braceLevels.peek()){
                    braceLevels.pop();
                    result.add("}");
                    result.add("@" + currentLabel);
                    result.add(":");
                    currentLabel++;
                    continue;
                }
            } else if (tokens.get(i).equals("break")){
                result.add("jmp");
                result.add("" + (currentLabel));
                continue;
            } 
            result.add(tokens.get(i));
        }

        return result;
    }

    private ArrayList<String> convertWhileLoops(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        int i=0;
        while (i < tokens.size()){
            // if the current token is while
            if (tokens.get(i).equals("while")){
                // put a label before this
                result.add("@" + currentLabel);
                result.add(":");

                // put an if instead of a while
                result.add("if");

                int startingIndex = i+1;
                int openBraces = 0;

                while (i < tokens.size()){
                    if (tokens.get(i).equals("{")){
                        openBraces++;
                    } else if (tokens.get(i).equals("}")){
                        openBraces--;
                        if (openBraces == 0){
                            // this is the ending brace
                            tokens.add(i, "jmp");
                            i++;
                            tokens.add(i, "" + currentLabel);
                            i++;
                            tokens.add(i, ";");
                            break;
                        }
                    }
                    i++;
                }

                // look for the ending brace and put a jmp @currentLabel

                currentLabel++;
                i = startingIndex;
            }

            result.add(tokens.get(i));
            i++;
        }

        return result;
    }

    private ArrayList<String> convertForLoops(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        int i=0;
        while (i < tokens.size()){
            if (tokens.get(i).equals("for")){
                // next token should be parenthesis
                if (i+1 >= tokens.size()){
                    System.out.println("Expected '(' after 'for'...");
                    System.exit(1);
                } 
                i++;
                i++;

                // get initializers (stuff before first semicolon)
                ArrayList<String> initializers = new ArrayList<>();

                while (i < tokens.size()){
                    if (tokens.get(i).equals(";")){
                        break;
                    }
                    initializers.add(tokens.get(i));
                    i++;
                }
                i++;

                // get conditions (stuff between first and second semicolon)
                ArrayList<String> conditions = new ArrayList<>();

                while (i < tokens.size()){
                    if (tokens.get(i).equals(";")){
                        break;
                    }
                    conditions.add(tokens.get(i));
                    i++;
                }
                i++;

                // get incrementers (stuff after second semicolon)
                ArrayList<String> incrementers = new ArrayList<>();
                int openParens = 1;

                while (i < tokens.size()){
                    if (tokens.get(i).equals(")")){
                        openParens--;
                        if (openParens == 0){
                            break;
                        }
                    } else if (tokens.get(i).equals("(")){
                        openParens++;
                    }
                    incrementers.add(tokens.get(i));
                    i++;
                }
                int startingIndex = i;

                // add the initializers
                for (int j=0; j<initializers.size(); j++){
                    result.add(initializers.get(j));
                }
                result.add(";");

                // add the label
                result.add("@" + currentLabel);
                result.add(":");
                
                // add the if statement
                result.add("if");
                result.add("(");

                // put the conditions in the if statement
                for (int j=0; j<conditions.size(); j++){
                    result.add(conditions.get(j));
                }
                result.add(")");

                // find the ending brace. put the incrementers and jump back
                int openBraces = 0;
                while (i < tokens.size()){
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
                for (int j=0; j<incrementers.size(); j++){
                    tokens.add(i, incrementers.get(j));
                    i++;
                }
                tokens.add(i, ";");
                i++;
                tokens.add(i, "jmp");
                i++;
                tokens.add(i, "" + currentLabel);
                i++;
                tokens.add(i, ";");
                
                i = startingIndex+1;
                currentLabel++;
            }

            result.add(tokens.get(i));
            i++;
        }

        return result;
    }



}
