/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                     //
//  This test program was written by following the official OpenHW's docs provided for "CORE-V Verification Strategy"  //
//  as of 19/10/2026                                                                                                   //
//                                                                                                                     //
//  Author: Riccardo Giani (UniBo)                                                                                     //
//  Description: this testcase takes into account the atomic instructions belonging to the RV32A Zaamo extension       //
//               (AMOSWAP.W, AMOADD.W, AMOAND.W, AMOOR.W, AMOXOR.W, AMOMAX[U].W, AMOMIN[U].W)                          //
//                                                                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// global variables on which the atomic operation will be executed
volatile int32_t memory_data_swap, memory_data_add, memory_data_and, memory_data_or, memory_data_xor, memory_data_max, memory_data_min;
volatile uint32_t memory_data_maxu, memory_data_minu;

int main(int argc, char *argv[])
{
    int32_t original_val;
    uint32_t original_val_unsigned;
    int test_failed = 0;
    
    // --- 1) TEST AMOSWAP.W ---
    //
    // Syntax: amoswap.w rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'memory_data_swap' in 'original_val', 
    // then unconditionally writes 'swap_val' into memory
    memory_data_swap = 10;
    int32_t swap_val = 25;

    __asm__ volatile (
        "amoswap.w %0, %1, (%2)"
        : "=&r" (original_val)                    // %0: rd
        : "r" (swap_val), "r" (&memory_data_swap) // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    // Original value (rd) before swap should be 10.
    // New value in memory should be 25.
    if (original_val == 10 && memory_data_swap == 25) {
        printf("\nAMO_SWAP: PASSED\n");
    } else {
        printf("\nAMO_SWAP: FAILED\n");
        printf("Expected: original_val=10, memory_data=25\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val, memory_data_swap);
        test_failed = 1;
    }

    // --- 2) TEST AMOADD.W ---
    //
    // Syntax: amoadd.w rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'memory_data_add' in 'original_val', 
    // then sums 'add_val' and saves the new result in memory
    memory_data_add = 10;
    int32_t add_val = 5;

    __asm__ volatile (
        "amoadd.w %0, %1, (%2)"
        : "=&r" (original_val)                  // %0: rd
        : "r" (add_val), "r" (&memory_data_add) // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    // Original value (rd) before add should be 10.
    // New value in memory should be 10 + 5 = 15.
    if (original_val == 10 && memory_data_add == 15) {
        printf("\nAMO_ADD: PASSED\n");
    } else {
        printf("\nAMO_ADD: FAILED\n");
        printf("Expected: original_val=10, memory_data=15\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val, memory_data_add);
        test_failed = 1;
    }

    // --- 3) TEST AMOAND.W ---
    //
    // Syntax: amoand.w rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'memory_data_and' in 'original_val', 
    // then performs a bitwise AND with 'and_val' and saves the new result in memory
    memory_data_and = 12; // b1100
    int32_t and_val = 10; // b1010

    __asm__ volatile (
        "amoand.w %0, %1, (%2)"
        : "=&r" (original_val)                  // %0: rd
        : "r" (and_val), "r" (&memory_data_and) // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    // Original value (rd) before and should be 12.
    // New value in memory should be 10 & 12 = 8.
    if (original_val == 12 && memory_data_and == 8) {
        printf("\nAMO_AND: PASSED\n");
    } else {
        printf("\nAMO_AND: FAILED\n");
        printf("Expected: original_val=12, memory_data=8\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val, memory_data_and);
        test_failed = 1;
    }

    // --- 4) TEST AMOOR.W ---
    //
    // Syntax: amoor.w rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'memory_data_or' in 'original_val', 
    // then performs a bitwise OR with 'or_val' and saves the new result in memory
    memory_data_or = 12; // b1100
    int32_t or_val = 10; // b1010

    __asm__ volatile (
        "amoor.w %0, %1, (%2)"
        : "=&r" (original_val)                // %0: rd
        : "r" (or_val), "r" (&memory_data_or) // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    // Original value (rd) before or should be 12.
    // New value in memory should be 10 | 12 = 14.
    if (original_val == 12 && memory_data_or == 14) {
        printf("\nAMO_OR: PASSED\n");
    } else {
        printf("\nAMO_OR: FAILED\n");
        printf("Expected: original_val=12, memory_data=14\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val, memory_data_or);
        test_failed = 1;
    }

    // --- 5) TEST AMOXOR.W ---
    //
    // Syntax: amoxor.w rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'memory_data_xor' in 'original_val', 
    // then performs a bitwise XOR with 'xor_val' and saves the new result in memory
    memory_data_xor = 12; // b1100
    int32_t xor_val = 6;  // b0110

    __asm__ volatile (
        "amoxor.w %0, %1, (%2)"
        : "=&r" (original_val)                  // %0: rd
        : "r" (xor_val), "r" (&memory_data_xor) // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    // Original value (rd) before xor should be 12.
    // New value in memory should be 6 ^ 12 = 10.
    if (original_val == 12 && memory_data_xor == 10) {
        printf("\nAMO_XOR: PASSED\n");
    } else {
        printf("\nAMO_XOR: FAILED\n");
        printf("Expected: original_val=12, memory_data=10\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val, memory_data_xor);
        test_failed = 1;
    }

    // --- 6) TEST AMOMAX.W ---
    //
    // Syntax: amomax.w rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'memory_data_max' in 'original_val_signed', 
    // then performs a signed MAX comparison with 'max_val' and saves the new result in memory
    memory_data_max = -10;     
    int32_t max_val = -2;

    __asm__ volatile (
        "amomax.w %0, %1, (%2)"
        : "=&r" (original_val)                  // %0: rd
        : "r" (max_val), "r" (&memory_data_max) // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    // Original value (rd) before max should be -10.
    // New value in memory should be max(-10,-2) = -2.
    if (original_val == -10 && memory_data_max == -2) {
        printf("\nAMO_MAX: PASSED\n");
    } else {
        printf("\nAMO_MAX: FAILED\n");
        printf("Expected: original_val=-10, memory_data=-2\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val, memory_data_max);
        test_failed = 1;
    }

    // --- 7) TEST AMOMAXU.W ---
    //
    // Syntax: amomaxu.w rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'memory_data_maxu' in 'original_val', 
    // then performs an unsigned MAX comparison with 'maxu_val' and saves the new result in memory
    memory_data_maxu = 10;     
    uint32_t maxu_val = 20;

    __asm__ volatile (
        "amomaxu.w %0, %1, (%2)"
        : "=&r" (original_val_unsigned)           // %0: rd
        : "r" (maxu_val), "r" (&memory_data_maxu) // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    // Original value (rd) before maxu should be 10.
    // New value in memory should be maxu(10,20) = 20.
    if (original_val_unsigned == 10 && memory_data_maxu == 20) {
        printf("\nAMO_MAXU: PASSED\n");
    } else {
        printf("\nAMO_MAXU: FAILED\n");
        printf("Expected: original_val=10, memory_data=20\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val_unsigned, memory_data_maxu);
        test_failed = 1;
    }

    // --- 8) TEST AMOMIN.W ---
    //
    // Syntax: amomin.w rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'memory_data_min' in 'original_val_signed', 
    // then performs a signed min comparison with 'min_val' and saves the new result in memory
    memory_data_min = -10;     
    int32_t min_val = -20;

    __asm__ volatile (
        "amomin.w %0, %1, (%2)"
        : "=&r" (original_val)                  // %0: rd
        : "r" (min_val), "r" (&memory_data_min) // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    // Original value (rd) before min should be -10.
    // New value in memory should be min(-10,-20) = -20.
    if (original_val== -10 && memory_data_min == -20) {
        printf("\nAMO_MIN: PASSED\n");
    } else {
        printf("\nAMO_MIN: FAILED\n");
        printf("Expected: original_val=-10, memory_data=-20\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val, memory_data_min);
        test_failed = 1;
    }

    // --- 9) TEST AMOMINU.W ---
    //
    // Syntax: amominu.w rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'memory_data_minu' in 'original_val', 
    // then performs an unsigned min comparison with 'minu_val' and saves the new result in memory
    memory_data_minu = 10;     
    uint32_t minu_val = 7;

    __asm__ volatile (
        "amominu.w %0, %1, (%2)"
        : "=&r" (original_val_unsigned)           // %0: rd
        : "r" (minu_val), "r" (&memory_data_minu) // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    // Original value (rd) before minu should be 10.
    // New value in memory should be minu(10,7) = 7.
    if (original_val_unsigned == 10 && memory_data_minu == 7) {
        printf("\nAMO_MINU: PASSED\n");
    } else {
        printf("\nAMO_MINU: FAILED\n");
        printf("Expected: original_val=10, memory_data=7\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val_unsigned, memory_data_minu);
        test_failed = 1;
    }

    // --- Final Result Report ---
    if(test_failed) {
        printf("\nAMO_INSTR_TEST: FAILED\n");
        return EXIT_FAILURE;
    } else {
        printf("\nAMO_INSTR_TEST: PASSED\n");
        return EXIT_SUCCESS;
    }
}
