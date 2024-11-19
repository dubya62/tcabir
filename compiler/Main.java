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
        int finalVarnum = simplifier.finalVarnum;

        // convert the representation to IR
        Converter converter = new Converter(tokens);
        tokens = converter.tokens;

        // figure out the types of each variable and remove declarations
        TypeChecker typeChecker = new TypeChecker(tokens);
        tokens = typeChecker.tokens;

        // inline all functions
        Inliner inliner = new Inliner(tokens, finalVarnum);
        tokens = inliner.tokens;

        // Ignore unnecessary operations
        Ignorer ignorer = new Ignorer(tokens);
        tokens = ignorer.tokens;
    }

    public static void debug(String message){
        if (Main.DEBUG){
            System.out.println(message);
        }
    }

}
