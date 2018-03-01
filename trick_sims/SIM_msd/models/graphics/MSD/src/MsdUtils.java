/*
 * Trick
 * 2018 (c) National Aeronautics and Space Administration (NASA)
 * Programmers: Scott P. Fennell
 */



public class MsdUtils {

    public static void  printHelpText() {
        System.out.println(
                "----------------------------------------------------------------------\n"
                        + "usage: java jar CannonDisplay.jar <port-number>\n"
                        + "----------------------------------------------------------------------\n"
        );
    }
}

class TrickSimMode {
    public static final int INIT = 0;
    public static final int FREEZE = 1;
    public static final int RUN = 5;
}