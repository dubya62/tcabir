/**
 * The purpose of this file is to ignore unnecessary code (code that does not directly change the output of the program
 */
import java.util.ArrayList;

public class Ignorer{
    public ArrayList<String> tokens;

    public Ignorer(ArrayList<String> tokens){
        Main.debug("Starting the ignorer...");
        this.tokens = tokens;

        // the only important things to keep are reads, writes, sleeps, getrandoms, and return statements in the main function


        Main.debug("Ignorer is finished!");
        Main.debug("Ignorer output:");
        Main.debug(this.tokens.toString());
    }
}
