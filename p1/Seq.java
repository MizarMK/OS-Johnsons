import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOError;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Scanner;

public class Seq {
    public static volatile long CURRENT_MAX = Integer.MIN_VALUE;

    public static volatile int START_NDX = 0;

    public static volatile int END_NDX = 0;

    public static final boolean PROCLIST = true;

    /**
     * Finds if a character array is numeric
     * @param number character array of numbers (and potentially non-numeric chars)
     * @returns true if the char* contains only digits, false otherwise
     */
    public static boolean isNumber(String number) {
        int i = 0;

        // checking for negative numbers
        if (number.charAt(0) == '-')
            i = 1;
        for (; i < number.length(); i++) {
            // if (number[i] > '9' || number[i] < '0')
            if (!Character.isDigit(number.charAt(i)))
                return false;
        }
        return true;
    }


    /**
     * Read File of integer input or from stdin
     * @param fname filename
     * @param argc number of args
     * @return number of bytes read from file (or stdin)
     */
    static ArrayList<Integer> readFile (String fname, int argc) {
        int i, c = 0;
        ArrayList<Integer> array = new ArrayList<>();
        String num = null;
        if (argc > 1) {
            BufferedReader reader;
            try {
               reader = new BufferedReader(new FileReader(fname));
               num = reader.readLine();
               while (num != null) {
                   if (!isNumber(num)) {
                       System.err.println("Bad number input; exiting");
                       System.exit(4);
                   }
                   i = Integer.parseInt(num);
                   array.add(i);
                   c++;

                   // read next line
                   num = reader.readLine();
               }
            } catch (IOException e) {
                System.err.println("File failed to open exiting...");
                System.exit(3);
            }
        }
        else if (argc == 1) {
            Scanner scan = new Scanner(System.in);
            System.out.println("Enter integers in array (CTRL-D for end input):");
            while (scan.hasNext()) {
                try {
                    num = scan.next();
                } catch (Exception e) {
                    System.err.println("Bad Input");
                    System.exit(4);
                }
                i = Integer.parseInt(num);
                array.add(i);
                c++;
            }
        }
        if (c < 1) {
            System.err.println("File empty; exiting...");
            System.exit(3);
        }
        return array;
    }

    public static void main(String []args) {
        if (args.length != 1 && args.length != 2) {
            System.err.println("Usage: ./Seq <Number of procs> <Filename>");
            System.exit(1);
        }
        int processes = Integer.parseInt(args[0]);
        if (!isNumber(args[0]) || processes < 1) {
            System.err.println("Invalid process input");
            System.exit(2);
        }

        String fname;
        if (args.length == 2)
            fname = args[1];
        else
            fname = "";

        int i;
        ArrayList<Integer> array = readFile(fname, args.length);
        Thread [] threads = new Thread[processes];

        for (i = 0; i < processes; i++) {
            try {
                Runnable r = new ChildThread(array.size(), i, processes, array, PROCLIST);
                threads[i] = new Thread(r);
                threads[i].start();
            } catch (Exception e) {
                System.err.println("Error in thread creation; exiting...");
                System.exit(5);
            }
        }
        for (i = 0; i < processes; i++) {
            try {
                threads[i].join();
            } catch (Exception e) {
                System.err.println("Error in thread wait; exiting...");
                System.exit(5);
            }
        }
        System.out.println("Maximum Sum: " + CURRENT_MAX
                + "[" + START_NDX + ","
                + END_NDX + "]");
    }
}
