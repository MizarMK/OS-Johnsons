/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 4
 * Class:       CSI 4337
 ************************************************/

import java.util.ArrayList;
import java.util.Collections;

/**
 * Represents 2-layer page table with TLB and its corresponding memory access activities
 */
public class TLB {
    /** size (in bits) of outer page index */
    int outerPgSize;

    /** size (in bits) of inner page index */
    int innerPgSize;

    /** size (in bits) of offset index */
    int offsetSize;

    /** time to access address from phys mem */
    int physCost;

    /** time needed to access address from tlb */
    int tlbCost;

    /** max number of addresses able to be stored in tlb */
    int tlbCap;

    /** total time taken referencing addresses from accessMemory() calls */
    int accessTime;

    /** total number of addresses referenced using accessMemory */
    int numAccesses;

    /** List representing cached addresses in TLB */
    ArrayList<Integer> tlbTable;

    /** List representing outer page table*/
    ArrayList<Integer> outerPageTbl;

    /** List representing inner page table*/
    ArrayList<Integer> innerPageTbl;

    /** List representing physical memory blocks*/
    ArrayList<Integer> physMem;

    /**
     * creates 2-layer page table obj with TLB functionality given costs and bit sizes passed
     * @param outerPg number of bytes representing outer page in address
     * @param innerPg number of bytes representing inner page in address
     * @param offset number of bytes representing offset in address
     * @param physCost cost of accessing physical mem for r/w ops
     * @param tlbCost cost of accessing TLB to find physical mem frame for r/w ops
     * @param tlbCap maximum number of addresses allowed in TLB cache
     */
    public TLB(int outerPg, int innerPg, int offset, int physCost, int tlbCost, int tlbCap) {
        this.outerPgSize = outerPg;
        this.innerPgSize = innerPg;
        this.offsetSize = offset;
        this.physCost = physCost;
        this.tlbCost = tlbCost;
        this.tlbCap = tlbCap;

        this.accessTime = 0;
        this.numAccesses = 0;
        this.tlbTable = new ArrayList<>(tlbCap);
        this.outerPageTbl = new ArrayList<>(Collections.nCopies((int) Math.pow(2., (double) outerPg), -1));
        this.innerPageTbl = new ArrayList<>(Collections.nCopies((int) Math.pow(2., (double) innerPg), -1));
        this.physMem = new ArrayList<>(Collections.nCopies((int) Math.pow(2., (double) offset), -1));
    }

    /**
     * Method replicating a memory access using the TLB and page tables; increments accessTime by appropriate amount
     * @param address address attempting to be accessed
     */
    public void accessMemory(int address) {
        if (pageAddress(address).length != 3) {
            System.err.println("ERROR: deriving paging from input address; exiting...");
            System.exit(2);
        }
        /// query TLB; if address is already cached, increment our access time by TLB cost, else query pg table(s)
        storeLRU(searchTlb(address), address);

    }

    /**
     * find address in TLB
     * @param address address to find in TLB cache
     * @return return index of address in TLB cache, -1 if TLB miss
     */
    public int searchTlb(int address) {
        for (int i = 0; i <this.tlbTable.size(); i++) {
            int [] a = pageAddress(address);
            int [] b = pageAddress(tlbTable.get(i));
            if (a[0] == b[0] && a[1] == b[1])
                return i;
        }
        return -1;
    }

    /**
     * store address in TLB cache; replace LRU address if TLB is full
     * @param cachedNdx index of address if TLB hit, moves address to most recent ndx
     * @param address address to add to TLB cache
     */
    public void storeLRU(int cachedNdx, int address) {
        int [] paging = pageAddress(address);

        // if we have the memory in the TLB, we may directly access the phys memFrame
        if (cachedNdx != -1) {
            tlbTable.remove(cachedNdx);
            this.tlbTable.add(address); // move address to start (most recent access) of tlbTable
            this.accessTime += tlbCost;

        }
        else { // remove least recently used item from TLB and add recent address to the end (most recent)
            /// tlb miss -- store the missed addr in the TLB using LRU
            if (tlbTable.size() == tlbCap)
                tlbTable.remove(0);
            tlbTable.add(address);
            this.accessTime += tlbCost;

            /// simulate r/w from outer pg table
            //if (outerPageTbl.get(paging[0]) == -1) {
                this.outerPageTbl.set(paging[0], paging[1]);
                this.accessTime += physCost;
            //}

            /// simulate r/w from inner pg table
            //if (innerPageTbl.get(outerPageTbl.get(paging[0])) == -1) {
                this.innerPageTbl.set(outerPageTbl.get(paging[0]), paging[2]);
                this.accessTime += physCost;
            //}
        }
        /// access phys memory at given offset from inner pg table
        int sim = this.physMem.get(this.innerPageTbl.get(this.outerPageTbl.get(paging[0])));
        this.accessTime += physCost;
        numAccesses++;
    }

    /**
     * bit shift address input to find paging
     * @param address address to bit shift
     * @return array of ints containing indexing for outer,inner, and phys mem lists
     */
    public int [] pageAddress(int address) {
        int out, in = address, offset = address;
        int tot = outerPgSize + innerPgSize + offsetSize;

        int possibleAddrs = (int) Math.pow(2, tot) - 1;

        if (possibleAddrs < address) {
            System.err.println("ERROR: this address cannot be represented using the address args passed; exiting...");
            //return new int[] {0,0,0};
            System.exit(1);
        }

        out = address >> (innerPgSize + offsetSize); // shift out inner pg bits and offset bits

        for (int i = tot-1; i >= tot-outerPgSize; i--) // clear outer pg bits and shift out offset bits
            in = (in & ~(1 << i));
        in = in >> offsetSize;

        for (int i = tot-1; i >= tot-outerPgSize-innerPgSize; i--) // clear outer pg and inner pg bits
            offset = offset & ~(1 << i);

        return new int[] {out, in, offset};
    }

    /**
     * output access time necessary for all accessMemory() calls thus far
     */
    public void outputTlbAccess() {
        System.out.print(accessTime + " ");
        System.out.printf("%.1f", (float) accessTime/numAccesses); // FIXME one significant decimal ONLY
        System.out.println();
    }
}
