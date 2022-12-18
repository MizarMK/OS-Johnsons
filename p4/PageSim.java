/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 4
 * Class:       CSI 4337
 ************************************************/

import java.io.FileInputStream;
import java.util.Scanner;

/**
 * Tests 2-layer page table with TLB and its corresponding memory access activities
 */
public class PageSim {

    /**
     * Driver testing page table functionality with various inputs passed from files/stdin
     * @param args unused args
     */
    public static void main(String[] args) {
        Scanner in;
        try {
            //in = new Scanner(new FileInputStream("testinputs/input4.txt"));
            in = new Scanner(System.in);
            int outerPg, innerPg, offset, physCost, tlbCost, tlbCap, address;
            TLB tlb;

            /// Read stdin for TLB details
            try {
                String[] addressInfo = in.nextLine().split(" ");
                if (addressInfo.length != 3) {
                    System.err.println("ERROR: invalid number of TLB details; exiting...");
                    System.exit(1);
                }
                outerPg = Integer.parseInt(addressInfo[0]);
                innerPg = Integer.parseInt(addressInfo[1]);
                offset = Integer.parseInt(addressInfo[2]);

                if (outerPg < 1 || innerPg < 1 || offset < 1) {
                    System.err.println("ERROR: TLB address details must be positive; exiting...");
                    System.exit(2);
                }

                int totSize = outerPg + innerPg + offset;
                if (totSize > 32) {
                    System.err.println("ERROR: TLB address details exceeds 32 bits");
                    System.exit(2);
                }

                physCost = Integer.parseInt(in.nextLine());
                if (physCost < 0) {
                    System.err.println("ERROR: Memory Access cost must be non-negative");
                    System.exit(2);
                }

                String[] tlbInfo = in.nextLine().split(" ");
                tlbCap = Integer.parseInt(tlbInfo[0]);
                tlbCost = Integer.parseInt(tlbInfo[1]);
                if (tlbCost < 0 || tlbCap < 0) {
                    System.err.println("ERROR: TLB cost or size details invalid");
                    System.exit(2);
                }

                /// create new object representing two-layer page memory access struct w/ TLB
                tlb = new TLB(outerPg, innerPg, offset, physCost, tlbCost, tlbCap);

                /// for remaining address inputs, replicate memory accesses on the address w/ paging
                while (in.hasNextLine()) { // read addresses from stdin
                    try {
                        address = Integer.parseInt(in.nextLine());
                        if (address < 0) {
                            System.err.println("ERROR: address input may not be negative");
                            System.exit(2);
                        }
                        tlb.accessMemory(address);
                    } catch (NumberFormatException e) {
                        System.err.println("ERROR: address contains invalid character");
                        System.exit(2);
                    }
                }
                tlb.outputTlbAccess();

            } catch (NumberFormatException e) {
                System.err.println("ERROR: unable to convert TLB detail input to integer");
                System.exit(2);
            } catch (Exception e) {
                System.err.println("ERROR: unable to completely read TLB input details; exiting...");
                System.exit(1);
            }
        } catch (Exception e) {
        System.err.println("ERROR: File doesn't exist; exiting...");
        System.exit(4);
        }
    }
}
