/**
 * The purpose of this file is to inline all functions and convert recursive functions into iterative versions
 */
import java.util.ArrayList;

public class Inliner{
    public ArrayList<String> tokens;

    public Inliner(ArrayList<String> tokens){
        Main.debug("Starting the inliner...");
        this.tokens = tokens;

        // inline all functions
        this.tokens = inlineFunctions(this.tokens);

        Main.debug("Inliner is finished!");
        Main.debug("Inliner output:");
        Main.debug(this.tokens.toString());
    }


    private ArrayList<String> inlineFunctions(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        for (int i=0; i<tokens.size(); i++){
            // figure out if this token is a function definition
        }

        return result;
    }
}
