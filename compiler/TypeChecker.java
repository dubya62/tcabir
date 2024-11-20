/**
 * The purpose of this file is to handle type checking
 * It will figure out the type of each variable and remove type declarations
 */
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Set;
import java.util.HashSet;
import java.util.Arrays;

public class TypeChecker{
    public ArrayList<String> tokens;
    public HashMap<String, String> types;

    public TypeChecker(ArrayList<String> tokens){
        Main.debug("Starting the type checker...");
        this.tokens = tokens;

        // infer the types of each variable
        this.types = inferTypes(this.tokens);

        Main.debug("Type checker finished!");
        Main.debug("Type checker output:");
        Main.debug(this.tokens.toString());
        Main.debug("Types:");
        Main.debug(this.types.toString());
    }

    /**
     * Extract the types of each variable from the tokens
     * throw an error if there are mutliple declarations of the same variable
     */
    public HashMap<String, String> inferTypes(ArrayList<String> tokens){
        // #0 : int
        HashMap<String, String> result = new HashMap<>();
        ArrayList<String> resultTokens = new ArrayList<>();

        String[] builtinArray = {"char", "short", "long", "int", "float", "double", "void"};
        Set<String> builtins = new HashSet<>();
        builtins.addAll(Arrays.asList(builtinArray));

        int varNameIndex; 

        for (int i=0; i<tokens.size(); i++){
            varNameIndex = i;
            if (builtins.contains(tokens.get(i))){
                if (i+1 < tokens.size() && (tokens.get(i+1).equals(")") || tokens.get(i+1).equals(","))){
                    resultTokens.add(tokens.get(i));
                    continue;
                }
                // the next value is what is being typed
                int startingIndex = i;
                String resultingType = tokens.get(i);
                i++;
                while (i < tokens.size()){
                    if (!builtins.contains(tokens.get(i))){
                        if (!tokens.get(i).equals("[") && !tokens.get(i).equals("]") && !tokens.get(i).equals("*")){
                            break;
                        }
                    }
                    resultingType += tokens.get(i);
                    i++;
                }
                String varName = tokens.get(i);
                varNameIndex = i;
                if (i < tokens.size()){
                    if (result.containsKey(tokens.get(i))){
                        System.out.println("ERROR: multiple declarations of " + tokens.get(i));
                        System.exit(1);
                    } else {
                        if (i+1 < tokens.size() && tokens.get(i+1).equals("[")){
                            int openBrackets = 0;
                            i++;
                            while (i < tokens.size()){
                                resultingType += tokens.get(i);
                                if (tokens.get(i).equals("[")){
                                    openBrackets++;
                                } else if (tokens.get(i).equals("]")){
                                    openBrackets--;
                                    if (openBrackets == 0){
                                        break;
                                    }
                                }
                                i++;
                            }
                        }

                        result.put(varName, resultingType);
                        resultTokens.add("def");
                    }
                }
            } else if (tokens.get(i).equals("struct")){
                if (i+2 < tokens.size()){
                    if (!tokens.get(i+2).equals("{")){
                        result.put(tokens.get(i+2), tokens.get(i) + tokens.get(i+1));
                        i+=2;
                        varNameIndex = i;
                    } else {
                        result.put(tokens.get(i+1), tokens.get(i));
                        i++;
                        varNameIndex = i;
                    }
                    resultTokens.add("def");
                }
            }
            
            resultTokens.add(tokens.get(varNameIndex));
        }

        this.tokens = resultTokens;
        return result;
    }
}

