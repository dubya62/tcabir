/**
 * The purpose of this file is to break C source code up into tokens.
 */
import java.util.ArrayList;
import java.util.Scanner;
import java.io.File;
import java.util.Set;
import java.util.HashSet;
import java.util.Arrays;

public class Lexer{
    public String fileData;
    public ArrayList<String> tokens;

    public Lexer(String filename){
        Main.debug("Starting the Lexer...");

        // first, open the file up and read its contents
        this.fileData = readFile(filename);

        Main.debug("File contents: ");
        Main.debug(this.fileData);

        // next, break the file up into tokens
        this.tokens = breakIntoTokens(this.fileData);

        Main.debug("Tokenizer output: ");
        Main.debug(this.tokens.toString());

        Main.debug("Lexer finished!");
    }

    /**
     * Read a file and return its contents as a single String
     */
    private String readFile(String filename){
        Main.debug("Attempting to read the file...");
        String result = "";
        try{
            File theFile = new File(filename);
            Scanner scanner = new Scanner(theFile);
            while (scanner.hasNextLine()){
                String thisLine = scanner.nextLine();
                // replace the \n that gets eaten up by nextLine
                result += thisLine + "\n";
            }
        } catch (Exception e){
            System.out.println("Failed to read " + filename + ". Make sure this file exists.");
            System.exit(1);
        }
        Main.debug("Finished reading the file...");
        return result;
    }

    private ArrayList<String> breakIntoTokens(String theFile){
        Main.debug("Breaking the file into tokens...");
        ArrayList<String> result = new ArrayList<>();

        // create a set of break chars
        Character[] breakCharCollection = {'~', '!', '#', '%', '^', '&', '*', '(', ')', '-', '+', '=', '{', '}', '[', ']', '|', '\'', '"', ';', ':', '/', '?', '.', ',', '<', '>', '\n', '\t', '\\', ' '};

        Set<Character> breakChars = new HashSet<>();
        breakChars.addAll(Arrays.asList(breakCharCollection));
    
        String curr = "";
        for (int i=0; i<theFile.length(); i++){
            if (breakChars.contains(theFile.charAt(i))){
                if (curr.length() > 0){
                    result.add(curr);
                }
                result.add("" + theFile.charAt(i));
                curr = "";
            } else {
                curr += theFile.charAt(i);
            }

        }

        Main.debug("Finshed breaking the file into tokens!");
        return result;
    }
}
