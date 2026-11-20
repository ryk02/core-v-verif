/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                         //
//  This test program was written by following the official OpenHW's docs provided for "CORE-V Verification Strategy"      //
//  as of 19/11/2026                                                                                                       //
//                                                                                                                         //
//  Author: Riccardo Giani (UniBo)                                                                                         //
//  Description: this testcase takes into account the atomic instructions belonging to the RV32A Zalrsc extension          //
//               (LR.W, SC.W)                                                                                              //
//                                                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// --- VERY IMPORTANT ---
// this test MUST be ran using the top wrapper cv32e40px_top_zalrsc.sv,
// where 1 core cv32e40px and 1 obi_atop_resolver are instanciated
// (see the QUICK USAGE TIP at the beginning of the top wrapper).

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Global variables on which the Word (.w) load-reserved / store-conditional operations will be executed
volatile int32_t mem_w_lrsc __attribute__((aligned(4)));
volatile int32_t mem_w_a __attribute__((aligned(4)));
volatile int32_t mem_w_b __attribute__((aligned(4)));

int main(int argc, char *argv[])
{
    int32_t loaded_val;
    int32_t sc_status;
    int test_failed = 0;

    ////////////////////////////////////
    //                                //
    //  STANDARD FUNCTIONAL BEHAVIOR  //
    //                                //
    ////////////////////////////////////

    printf("\n===== STANDARD FUNCTIONAL BEHAVIOR =====\n");

    // --- 1) TEST LR.W & SC.W ---
    //
    // Syntax: lr.w rd, (rs1)
    //         sc.w rd, rs2, (rs1)
    //
    // Description:
    // lr.w loads the actual value of 'mem_w_lrsc' into 'loaded_val' and registers a reservation on its address;
    // sc.w then conditionally writes 'store_val_w' into memory and stores 0 into 'sc_status' upon success
    mem_w_lrsc = 10;
    int32_t store_val_w = 25;

    __asm__ volatile (
        "lr.w %0, (%3)\n\t"
        "sc.w %1, %2, (%3)"
        : "=&r" (loaded_val), "=&r" (sc_status)       // %0: rd (lr.w), %1: rd (sc.w)
        : "r" (store_val_w), "r" (&mem_w_lrsc)        // %2: rs2, %3: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    if (loaded_val == 10 && sc_status == 0 && mem_w_lrsc == 25) {
        printf("\nLR_SC.W: PASSED\n");
    } else {
        printf("\nLR_SC.W: FAILED\n");
        printf("Expected: loaded_val=10, sc_status=0, memory_data=25\n");
        printf("Actual:   loaded_val=%ld, sc_status=%ld, memory_data=%ld\n", (long)loaded_val, (long)sc_status, (long)mem_w_lrsc);
        test_failed = 1;
    }

    // --- 2) TEST SC.W WITHOUT PRECEDING LR.W ---
    //
    // Syntax: sc.w rd, rs2, (rs1)
    //
    // Description:
    // sc.w is executed with no active reservation: it must fail (sc_status != 0)
    // and leave 'mem_w_a' unchanged
    mem_w_a = 42;
    store_val_w = 99;
    sc_status = 0;

    __asm__ volatile (
        "sc.w %0, %1, (%2)"
        : "=&r" (sc_status)                     // %0: rd (status)
        : "r" (store_val_w), "r" (&mem_w_a)     // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    if (sc_status != 0 && mem_w_a == 42) {
        printf("\nSC_NO_LR.W: PASSED\n");
    } else {
        printf("\nSC_NO_LR.W: FAILED\n");
        printf("Expected: sc_status!=0, memory_data=42\n");
        printf("Actual:   sc_status=%ld, memory_data=%ld\n",
               (long)sc_status, (long)mem_w_a);
        test_failed = 1;
    }

    // --- 3) TEST SC.W ON DIFFERENT ADDRESS ---
    //
    // Syntax: lr.w rd, (rs1)
    //         sc.w rd, rs2, (rs1_other)
    //
    // Description:
    // lr.w places a reservation on 'mem_w_a', then sc.w targets 'mem_w_b';
    // sc.w must fail (sc_status != 0) and neither memory location must change
    mem_w_a = 10;
    mem_w_b = 20;
    store_val_w = 77;
    sc_status = 0;

    __asm__ volatile (
        "lr.w %0, (%3)\n\t"
        "sc.w %1, %2, (%4)"
        : "=&r" (loaded_val), "=&r" (sc_status)                 // %0: rd (lr.w), %1: rd (sc.w)
        : "r" (store_val_w), "r" (&mem_w_a), "r" (&mem_w_b)     // %2: rs2, %3: addr A, %4: addr B
        : "memory"
    );

    // --- Self-Checking ---
    if (loaded_val == 10 && sc_status != 0 && mem_w_a == 10 && mem_w_b == 20) {
        printf("\nLR_SC_DIFF_ADDR.W: PASSED\n");
    } else {
        printf("\nLR_SC_DIFF_ADDR.W: FAILED\n");
        printf("Expected: loaded_val=10, sc_status!=0, mem_w_a=10, mem_w_b=20\n");
        printf("Actual:   loaded_val=%ld, sc_status=%ld, mem_w_a=%ld, mem_w_b=%ld\n",
               (long)loaded_val, (long)sc_status, (long)mem_w_a, (long)mem_w_b);
        test_failed = 1;
    }

    // --- 4) TEST DOUBLE CONSECUTIVE SC.W ---
    //
    // Syntax: lr.w rd, (rs1)
    //         sc.w rd1, rs2_1, (rs1)
    //         sc.w rd2, rs2_2, (rs1)
    //
    // Description:
    // the first sc.w must succeed (sc_status_1 == 0) and clear the reservation;
    // the second sc.w must fail (sc_status_2 != 0) without overwriting memory
    int32_t sc_status_1, sc_status_2;
    int32_t store_val_1 = 30;
    int32_t store_val_2 = 40;
    mem_w_a = 15;

    __asm__ volatile (
        "lr.w %0, (%5)\n\t"
        "sc.w %1, %3, (%5)\n\t"
        "sc.w %2, %4, (%5)"
        : "=&r" (loaded_val), "=&r" (sc_status_1), "=&r" (sc_status_2)  // %0: lr rd, %1: sc1 rd, %2: sc2 rd
        : "r" (store_val_1), "r" (store_val_2), "r" (&mem_w_a)          // %3: val1, %4: val2, %5: addr A
        : "memory"
    );

    // --- Self-Checking ---
    if (loaded_val == 15 && sc_status_1 == 0 && sc_status_2 != 0 && mem_w_a == 30) {
        printf("\nLR_DOUBLE_SC.W: PASSED\n");
    } else {
        printf("\nLR_DOUBLE_SC.W: FAILED\n");
        printf("Expected: loaded_val=15, sc_status_1=0, sc_status_2!=0, memory_data=30\n");
        printf("Actual:   loaded_val=%ld, sc_status_1=%ld, sc_status_2=%ld, memory_data=%ld\n",
               (long)loaded_val, (long)sc_status_1, (long)sc_status_2, (long)mem_w_a);
        test_failed = 1;
    }

    // --- 5) TEST RESERVATION OVERWRITE (LR.W -> LR.W -> SC.W) ---
    //
    // Syntax: lr.w rd1, (rs1_a)
    //         lr.w rd2, (rs1_b)
    //         sc.w rd3, rs2, (rs1_b)
    //
    // Description:
    // the second lr.w replaces the reservation on 'mem_w_a' with 'mem_w_b';
    // sc.w on 'mem_w_b' must succeed (sc_status == 0) and update 'mem_w_b'
    int32_t loaded_val_a, loaded_val_b;
    mem_w_a = 100;
    mem_w_b = 200;
    store_val_w = 250;

    __asm__ volatile (
        "lr.w %0, (%4)\n\t"
        "lr.w %1, (%5)\n\t"
        "sc.w %2, %3, (%5)"
        : "=&r" (loaded_val_a), "=&r" (loaded_val_b), "=&r" (sc_status) // %0: rd A, %1: rd B, %2: sc rd
        : "r" (store_val_w), "r" (&mem_w_a), "r" (&mem_w_b)             // %3: val, %4: addr A, %5: addr B
        : "memory"
    );

    // --- Self-Checking ---
    if (loaded_val_a == 100 && loaded_val_b == 200 && sc_status == 0 &&
        mem_w_a == 100 && mem_w_b == 250) {
        printf("\nLR_OVERWRITE_SC.W: PASSED\n");
    } else {
        printf("\nLR_OVERWRITE_SC.W: FAILED\n");
        printf("Expected: loaded_a=100, loaded_b=200, sc_status=0, mem_w_a=100, mem_w_b=250\n");
        printf("Actual:   loaded_a=%ld, loaded_b=%ld, sc_status=%ld, mem_w_a=%ld, mem_w_b=%ld\n",
               (long)loaded_val_a, (long)loaded_val_b, (long)sc_status, (long)mem_w_a, (long)mem_w_b);
        test_failed = 1;
    }

    // --- 6) TEST LR.W / SC.W ORDERING SUFFIXES (.AQ, .RL, .AQRL) ---
    //
    // Syntax: lr.w.aq / sc.w.rl / lr.w.aqrl / sc.w.aqrl
    //
    // Description:
    // verifies that the decoder and LSU properly handle acquire/release ordering bits
    int32_t loaded_1, loaded_2;
    mem_w_a = 50;
    store_val_1 = 60;
    store_val_2 = 70;

    __asm__ volatile (
        "lr.w.aq   %0, (%6)\n\t"
        "sc.w.rl   %1, %4, (%6)\n\t"
        "lr.w.aqrl %2, (%6)\n\t"
        "sc.w.aqrl %3, %5, (%6)"
        : "=&r" (loaded_1), "=&r" (sc_status_1), "=&r" (loaded_2), "=&r" (sc_status_2)
        : "r" (store_val_1), "r" (store_val_2), "r" (&mem_w_a)
        : "memory"
    );

    // --- Self-Checking ---
    if (loaded_1 == 50 && sc_status_1 == 0 && loaded_2 == 60 && sc_status_2 == 0 && mem_w_a == 70) {
        printf("\nLR_SC_AQRL.W: PASSED\n");
    } else {
        printf("\nLR_SC_AQRL.W: FAILED\n");
        printf("Expected: loaded_1=50, sc_1=0, loaded_2=60, sc_2=0, memory_data=70\n");
        printf("Actual:   loaded_1=%ld, sc_1=%ld, loaded_2=%ld, sc_2=%ld, memory_data=%ld\n",
               (long)loaded_1, (long)sc_status_1, (long)loaded_2, (long)sc_status_2, (long)mem_w_a);
        test_failed = 1;
    }

    // --- 7) TEST LR.W / SC.W WITH X0 DESTINATION ---
    //
    // Syntax: lr.w zero, (rs1)
    //         sc.w zero, rs2, (rs1)
    //
    // Description:
    // verifies proper execution when discarding results into hardwired-zero register x0
    mem_w_a = 88;
    store_val_w = 99;

    __asm__ volatile (
        "lr.w zero, (%1)\n\t"
        "sc.w zero, %0, (%1)"
        :
        : "r" (store_val_w), "r" (&mem_w_a)
        : "memory"
    );

    // --- Self-Checking ---
    if (mem_w_a == 99) {
        printf("\nLR_SC_X0.W: PASSED\n");
    } else {
        printf("\nLR_SC_X0.W: FAILED\n");
        printf("Expected: memory_data=99\n");
        printf("Actual:   memory_data=%ld\n", (long)mem_w_a);
        test_failed = 1;
    }

    // --- Final Result Report ---
    if (test_failed) {
        printf("\nZALRSC_INSTR_TEST: FAILED\n");
        return EXIT_FAILURE;
    } else {
        printf("\nZALRSC_INSTR_TEST: PASSED\n");
        return EXIT_SUCCESS;
    }
}
