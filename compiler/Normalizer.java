/**
 * The purpose of this file is to remove unnecessary tokens from the representation
 */
import java.util.ArrayList;
import java.util.Set;
import java.util.HashSet;

public class Normalizer{
    public ArrayList<String> tokens;

    public Normalizer(ArrayList<String> tokens){
        this.tokens = tokens;

        // remove comments and multi-line comments that are not in strings
        this.tokens = removeComments(this.tokens);

        // combine strings into single tokens
        this.tokens = combineStrings(this.tokens);

        // remove tabs, newlines, and spaces
        this.tokens = removeWhitespace(this.tokens);

        // combine floats into single tokens
        this.tokens = combineFloats(this.tokens);

        Main.debug("Normalizer has finished!");
        Main.debug("Normalizer output:"); 
        Main.debug(this.tokens.toString());

    }

    /**
     * Remove all comments and multi-line comments
     */
    private ArrayList<String> removeComments(ArrayList<String> tokens){
        Main.debug("Removing all comments...");
        ArrayList<String> result = new ArrayList<>();

        int comment = 0;
        int multiLine = 0;
        int quotes = 0;
        int charQuote = 0;
        for (int i=0; i<tokens.size(); i++){

            // keeping track of whether or not we are in comments
            if (tokens.get(i).equals("/")){
                if (comment == 1 && multiLine == 1){
                    if (i > 0){
                        if (tokens.get(i-1).equals("*")){
                            comment = 0;
                            multiLine = 0;
                            continue;
                        }
                    }
                }
                if (i+1 < tokens.size()){
                    if (tokens.get(i+1).equals("/")){
                        // this is a single line comment
                        if (comment == 0 && quotes == 0){
                            comment = 1;
                            multiLine = 0;
                        }
                    } else if (tokens.get(i+1).equals("*")){
                        // this is a multi-line comment
                        if (comment == 0 && quotes == 0){
                            comment = 1;
                            multiLine = 1;
                        }
                    }
                }
            } 

            // keeping track of whether or not we are in quotes
            else if (tokens.get(i).equals("\"")){
                if (quotes == 0 && charQuote == 0){
                    quotes = 1;
                } else if (quotes == 1 && charQuote == 0){
                    quotes = 0;
                }
            } else if (tokens.get(i).equals("'")){
                if (quotes == 0 && charQuote == 0){
                    charQuote = 1;
                } else if (quotes == 0 && charQuote == 1){
                    charQuote = 0;
                }
            }

            // keeping track of newlines
            else if (tokens.get(i).equals("\n")){
                // end non multi-line comments
                if (comment == 1 && multiLine == 0){
                    comment = 0;
                }
                // throw syntax error if in string and last char is not \
                if (quotes == 1 && i > 0){
                    if (!tokens.get(i-1).equals("\\")){
                        System.out.println("Syntax error. Unmatched \"...");
                        System.exit(1);
                    }
                }
            }

            // if not in a comment, keep the token
            if (comment == 0){
                result.add(tokens.get(i));
            }
        }

        Main.debug("Finished removing comments!");
        return result;
    }

    /**
     * Combine Strings into single tokens
     */
    private ArrayList<String> combineStrings(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        String curr = "";

        int quotes = 0;
        int charQuotes = 0;
        for (int i=0; i<tokens.size(); i++){
            
            if (tokens.get(i).equals("\"")){
                if (charQuotes == 0 && quotes == 0){
                    quotes = 1;
                } else if (charQuotes == 0 && quotes == 1) {
                    quotes = 0;
                    curr += tokens.get(i);
                    result.add(curr);
                    curr = "";
                    continue;
                }
            } else if (tokens.get(i).equals("'")){
                if (charQuotes == 0 && quotes == 0){
                    charQuotes = 1;
                } else if (charQuotes == 1 && quotes == 0) {
                    charQuotes = 0;
                    curr += tokens.get(i);
                    result.add(curr);
                    curr = "";
                    continue;
                }

            } else if (tokens.get(i).equals("\\")){
                if (i+1 < tokens.size()){
                    if (tokens.get(i+1).equals("\n")){
                        i++;
                        continue;
                    }
                } 
            }

            if (quotes == 1 || charQuotes == 1){
                curr += tokens.get(i);
            } else {
                result.add(tokens.get(i));
            }
            
        }

        return result;
    }


    private ArrayList<String> removeWhitespace(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        Set<String> theSet = new HashSet<>();
        theSet.add(" ");
        theSet.add("\n");
        theSet.add("\t");

        for (int i=0; i<tokens.size(); i++){
            if (theSet.contains(tokens.get(i))){
            } else {
                result.add(tokens.get(i));
            }
        }

        return result;
    }


    private ArrayList<String> combineFloats(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals(".")){
                // if this is a dot and the tokens before and after are ints, combine them
                try{
                    Integer.parseInt(tokens.get(i-1));
                    Integer.parseInt(tokens.get(i+1));

                    result.set(result.size()-1, tokens.get(i-1) + tokens.get(i) + tokens.get(i+1));

                    i++;
                    continue;
                } catch (Exception e){
                }
            }

            result.add(tokens.get(i));
        }
        


        return result;
    }

}
