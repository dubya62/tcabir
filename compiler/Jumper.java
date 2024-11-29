/**
 * Convert from function calls to jumps using a software stack
 */
import java.util.ArrayList;

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

        // scan through the tokens looking for functions.
        // on return statement,
        //      RSP = RBP;
        //      RBP = STACK[RSP-1];
        //      jmp STACK[RSP-2]
        

        Main.debug("Jumper is finished!");
        Main.debug("Jumper output:");
        Main.debug(this.tokens.toString());
    }




}
