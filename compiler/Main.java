/**
 * The purpose of this file is to parse a C source file
 * and convert it fully to the internal representation
 */
import java.util.ArrayList;

public class Main{
    static boolean DEBUG = true;

    public static void main(String[] args){
        if (args.length < 1){
            System.out.println("Please supply a filename to compile...");
            return;
        }

        String filename = args[0];

        Main.debug("Compiling " + filename + "...");

        // Lex the file and get the tokens
        Lexer lexer = new Lexer(filename);
        ArrayList<String> tokens = lexer.tokens;

        // normalize the tokens in the file to a simpler version
        Normalizer normalizer = new Normalizer(tokens);
        tokens = normalizer.tokens;

        // simplify the representation
        Simplifier simplifier = new Simplifier(tokens);
        tokens = simplifier.tokens;

        // conver the representation to IR
        Converter converter = new Converter(tokens);
        tokens = converter.tokens;
    }

    public static void debug(String message){
        if (Main.DEBUG){
            System.out.println(message);
        }
    }

}
