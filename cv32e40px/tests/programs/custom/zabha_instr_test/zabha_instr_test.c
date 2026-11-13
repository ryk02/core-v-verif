/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                         //
//  This test program was written by following the official OpenHW's docs provided for "CORE-V Verification Strategy"      //
//  as of 19/10/2026                                                                                                       //
//                                                                                                                         //
//  Author: Riccardo Giani (UniBo)                                                                                         //
//  Description: this testcase takes into account the atomic instructions belonging to the RV32A Zabha extension           //
//               (AMOSWAP.[B|H], AMOADD.[B|H], AMOAND.[B|H], AMOOR.[B|H], AMOXOR.[B|H], AMOMAX[U].[B|H], AMOMIN[U].[B|H])  //
//                                                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Global variables on which the Byte (.b) atomic operations will be executed
volatile int8_t mem_b_swap, mem_b_add, mem_b_and, mem_b_or, mem_b_xor, mem_b_max, mem_b_min;
volatile uint8_t mem_b_maxu, mem_b_minu;

// Global variables on which the Halfword (.h) atomic operations will be executed
volatile int16_t mem_h_swap, mem_h_add, mem_h_and, mem_h_or, mem_h_xor, mem_h_max, mem_h_min;
volatile uint16_t mem_h_maxu, mem_h_minu;

int main(int argc, char *argv[])
{
    int32_t original_val;
    uint32_t original_val_unsigned;
    int test_failed = 0;

    // ==========================================
    //            BYTE (.B) INSTRUCTIONS
    // ==========================================

    // --- 1) TEST AMOSWAP.B ---
    //
    // Syntax: amoswap.b rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_b_swap' in 'original_val', 
    // then unconditionally writes 'swap_val_b' into memory
    mem_b_swap = 10;
    int8_t swap_val_b = 25;

    __asm__ volatile (
        "amoswap.b %0, %1, (%2)"
        : "=&r" (original_val)                    // %0: rd
        : "r" (swap_val_b), "r" (&mem_b_swap)     // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    if (original_val == 10 && mem_b_swap == 25) {
        printf("\nAMO_SWAP.B: PASSED\n");
    } else {
        printf("\nAMO_SWAP.B: FAILED\n");
        printf("Expected: original_val=10, memory_data=25\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_b_swap);
        test_failed = 1;
    }

    // --- 2) TEST AMOADD.B ---
    //
    // Syntax: amoadd.b rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_b_add' in 'original_val', 
    // then sums 'add_val_b' and saves the new result in memory
    mem_b_add = 10;
    int8_t add_val_b = 5;

    __asm__ volatile (
        "amoadd.b %0, %1, (%2)"
        : "=&r" (original_val)                  // %0: rd
        : "r" (add_val_b), "r" (&mem_b_add)     // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    if (original_val == 10 && mem_b_add == 15) {
        printf("\nAMO_ADD.B: PASSED\n");
    } else {
        printf("\nAMO_ADD.B: FAILED\n");
        printf("Expected: original_val=10, memory_data=15\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_b_add);
        test_failed = 1;
    }

    // --- 3) TEST AMOAND.B ---
    //
    // Syntax: amoand.b rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_b_and' in 'original_val', 
    // then performs a bitwise AND with 'and_val_b' and saves the new result in memory
    mem_b_and = 12;        // b1100
    int8_t and_val_b = 10; // b1010

    __asm__ volatile (
        "amoand.b %0, %1, (%2)"
        : "=&r" (original_val)                  // %0: rd
        : "r" (and_val_b), "r" (&mem_b_and)     // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    if (original_val == 12 && mem_b_and == 8) {
        printf("\nAMO_AND.B: PASSED\n");
    } else {
        printf("\nAMO_AND.B: FAILED\n");
        printf("Expected: original_val=12, memory_data=8\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_b_and);
        test_failed = 1;
    }

    // --- 4) TEST AMOOR.B ---
    //
    // Syntax: amoor.b rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_b_or' in 'original_val', 
    // then performs a bitwise OR with 'or_val_b' and saves the new result in memory
    mem_b_or = 12;        // b1100
    int8_t or_val_b = 10; // b1010

    __asm__ volatile (
        "amoor.b %0, %1, (%2)"
        : "=&r" (original_val)                // %0: rd
        : "r" (or_val_b), "r" (&mem_b_or)     // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    if (original_val == 12 && mem_b_or == 14) {
        printf("\nAMO_OR.B: PASSED\n");
    } else {
        printf("\nAMO_OR.B: FAILED\n");
        printf("Expected: original_val=12, memory_data=14\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_b_or);
        test_failed = 1;
    }

    // --- 5) TEST AMOXOR.B ---
    //
    // Syntax: amoxor.b rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_b_xor' in 'original_val', 
    // then performs a bitwise XOR with 'xor_val_b' and saves the new result in memory
    mem_b_xor = 12;        // b1100
    int8_t xor_val_b = 6;  // b0110

    __asm__ volatile (
        "amoxor.b %0, %1, (%2)"
        : "=&r" (original_val)                  // %0: rd
        : "r" (xor_val_b), "r" (&mem_b_xor)     // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    if (original_val == 12 && mem_b_xor == 10) {
        printf("\nAMO_XOR.B: PASSED\n");
    } else {
        printf("\nAMO_XOR.B: FAILED\n");
        printf("Expected: original_val=12, memory_data=10\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_b_xor);
        test_failed = 1;
    }

    // --- 6) TEST AMOMAX.B ---
    //
    // Syntax: amomax.b rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_b_max' in 'original_val', 
    // then performs a signed MAX comparison with 'max_val_b' and saves the new result in memory
    mem_b_max = -10;     
    int8_t max_val_b = -2;

    __asm__ volatile (
        "amomax.b %0, %1, (%2)"
        : "=&r" (original_val)                  // %0: rd
        : "r" (max_val_b), "r" (&mem_b_max)     // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    if (original_val == -10 && mem_b_max == -2) {
        printf("\nAMO_MAX.B: PASSED\n");
    } else {
        printf("\nAMO_MAX.B: FAILED\n");
        printf("Expected: original_val=-10, memory_data=-2\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_b_max);
        test_failed = 1;
    }

    // --- 7) TEST AMOMAXU.B ---
    //
    // Syntax: amomaxu.b rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_b_maxu' in 'original_val', 
    // then performs an unsigned MAX comparison with 'maxu_val_b' and saves the new result in memory
    mem_b_maxu = 10;     
    uint8_t maxu_val_b = 20;

    __asm__ volatile (
        "amomaxu.b %0, %1, (%2)"
        : "=&r" (original_val_unsigned)           // %0: rd
        : "r" (maxu_val_b), "r" (&mem_b_maxu)     // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    if (original_val_unsigned == 10 && mem_b_maxu == 20) {
        printf("\nAMO_MAXU.B: PASSED\n");
    } else {
        printf("\nAMO_MAXU.B: FAILED\n");
        printf("Expected: original_val=10, memory_data=20\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val_unsigned, (long)mem_b_maxu);
        test_failed = 1;
    }

    // --- 8) TEST AMOMIN.B ---
    //
    // Syntax: amomin.b rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_b_min' in 'original_val', 
    // then performs a signed min comparison with 'min_val_b' and saves the new result in memory
    mem_b_min = -10;     
    int8_t min_val_b = -20;

    __asm__ volatile (
        "amomin.b %0, %1, (%2)"
        : "=&r" (original_val)                  // %0: rd
        : "r" (min_val_b), "r" (&mem_b_min)     // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    if (original_val == -10 && mem_b_min == -20) {
        printf("\nAMO_MIN.B: PASSED\n");
    } else {
        printf("\nAMO_MIN.B: FAILED\n");
        printf("Expected: original_val=-10, memory_data=-20\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_b_min);
        test_failed = 1;
    }

    // --- 9) TEST AMOMINU.B ---
    //
    // Syntax: amominu.b rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_b_minu' in 'original_val', 
    // then performs an unsigned min comparison with 'minu_val_b' and saves the new result in memory
    mem_b_minu = 10;     
    uint8_t minu_val_b = 7;

    __asm__ volatile (
        "amominu.b %0, %1, (%2)"
        : "=&r" (original_val_unsigned)           // %0: rd
        : "r" (minu_val_b), "r" (&mem_b_minu)     // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    // --- Self-Checking ---
    if (original_val_unsigned == 10 && mem_b_minu == 7) {
        printf("\nAMO_MINU.B: PASSED\n");
    } else {
        printf("\nAMO_MINU.B: FAILED\n");
        printf("Expected: original_val=10, memory_data=7\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val_unsigned, (long)mem_b_minu);
        test_failed = 1;
    }


    // ==========================================
    //          HALFWORD (.H) INSTRUCTIONS
    // ==========================================

    // --- 10) TEST AMOSWAP.H ---
    //
    // Syntax: amoswap.h rd, rs2, (rs1)
    //
    // the instruction loads the actual value of 'mem_h_swap' in 'original_val', 
    // then unconditionally writes 'swap_val_h' into memory
    mem_h_swap = 10;
    int16_t swap_val_h = 25;

    __asm__ volatile (
        "amoswap.h %0, %1, (%2)"
        : "=&r" (original_val)                 // %0: rd
        : "r" (swap_val_h), "r" (&mem_h_swap)  // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    if (original_val == 10 && mem_h_swap == 25) {
        printf("\nAMO_SWAP.H: PASSED\n");
    } else {
        printf("\nAMO_SWAP.H: FAILED\n");
        printf("Expected: original_val=10, memory_data=25\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_h_swap);
        test_failed = 1;
    }

    // --- 11) TEST AMOADD.H ---
    //
    // Syntax: amoadd.h rd, rs2, (rs1)
    //
    // the instruction loads the actual value of 'mem_h_add' in 'original_val', 
    // then sums 'add_val_h' and saves the new result in memory
    mem_h_add = 10;
    int16_t add_val_h = 5;

    __asm__ volatile (
        "amoadd.h %0, %1, (%2)"
        : "=&r" (original_val)               // %0: rd
        : "r" (add_val_h), "r" (&mem_h_add)  // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    if (original_val == 10 && mem_h_add == 15) {
        printf("\nAMO_ADD.H: PASSED\n");
    } else {
        printf("\nAMO_ADD.H: FAILED\n");
        printf("Expected: original_val=10, memory_data=15\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_h_add);
        test_failed = 1;
    }

    // --- 12) TEST AMOAND.H ---
    //
    // Syntax: amoand.h rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_h_and' in 'original_val', 
    // then performs a bitwise AND with 'and_val_h' and saves the new result in memory
    mem_h_and = 12;         // b1100
    int16_t and_val_h = 10; // b1010

    __asm__ volatile (
        "amoand.h %0, %1, (%2)"
        : "=&r" (original_val)               // %0: rd
        : "r" (and_val_h), "r" (&mem_h_and)  // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    if (original_val == 12 && mem_h_and == 8) {
        printf("\nAMO_AND.H: PASSED\n");
    } else {
        printf("\nAMO_AND.H: FAILED\n");
        printf("Expected: original_val=12, memory_data=8\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_h_and);
        test_failed = 1;
    }

    // --- 13) TEST AMOOR.H ---
    //
    // Syntax: amoor.h rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_h_or' in 'original_val', 
    // then performs a bitwise OR with 'or_val_h' and saves the new result in memory
    mem_h_or = 12;         // b1100
    int16_t or_val_h = 10; // b1010

    __asm__ volatile (
        "amoor.h %0, %1, (%2)"
        : "=&r" (original_val)             // %0: rd
        : "r" (or_val_h), "r" (&mem_h_or)  // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    if (original_val == 12 && mem_h_or == 14) {
        printf("\nAMO_OR.H: PASSED\n");
    } else {
        printf("\nAMO_OR.H: FAILED\n");
        printf("Expected: original_val=12, memory_data=14\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_h_or);
        test_failed = 1;
    }

    // --- 14) TEST AMOXOR.H ---
    //
    // Syntax: amoxor.h rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_h_xor' in 'original_val', 
    // then performs a bitwise XOR with 'xor_val_h' and saves the new result in memory
    mem_h_xor = 12;         // b1100
    int16_t xor_val_h = 6;  // b0110

    __asm__ volatile (
        "amoxor.h %0, %1, (%2)"
        : "=&r" (original_val)               // %0: rd
        : "r" (xor_val_h), "r" (&mem_h_xor)  // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    if (original_val == 12 && mem_h_xor == 10) {
        printf("\nAMO_XOR.H: PASSED\n");
    } else {
        printf("\nAMO_XOR.H: FAILED\n");
        printf("Expected: original_val=12, memory_data=10\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_h_xor);
        test_failed = 1;
    }

    // --- 15) TEST AMOMAX.H ---
    //
    // Syntax: amomax.h rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_h_max' in 'original_val', 
    // then performs a signed MAX comparison with 'max_val_h' and saves the new result in memory
    mem_h_max = -10;     
    int16_t max_val_h = -2;

    __asm__ volatile (
        "amomax.h %0, %1, (%2)"
        : "=&r" (original_val)               // %0: rd
        : "r" (max_val_h), "r" (&mem_h_max)  // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    if (original_val == -10 && mem_h_max == -2) {
        printf("\nAMO_MAX.H: PASSED\n");
    } else {
        printf("\nAMO_MAX.H: FAILED\n");
        printf("Expected: original_val=-10, memory_data=-2\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_h_max);
        test_failed = 1;
    }

    // --- 16) TEST AMOMAXU.H ---
    //
    // Syntax: amomaxu.h rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_h_maxu' in 'original_val', 
    // then performs a signed MAX comparison with 'maxu_val_h' and saves the new result in memory
    mem_h_maxu = 10;     
    uint16_t maxu_val_h = 20;

    __asm__ volatile (
        "amomaxu.h %0, %1, (%2)"
        : "=&r" (original_val_unsigned)        // %0: rd
        : "r" (maxu_val_h), "r" (&mem_h_maxu)  // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    if (original_val_unsigned == 10 && mem_h_maxu == 20) {
        printf("\nAMO_MAXU.H: PASSED\n");
    } else {
        printf("\nAMO_MAXU.H: FAILED\n");
        printf("Expected: original_val=10, memory_data=20\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val_unsigned, (long)mem_h_maxu);
        test_failed = 1;
    }

    // --- 17) TEST AMOMIN.H ---
    //
    // Syntax: amomin.h rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_h_min' in 'original_val', 
    // then performs a signed min comparison with 'min_val_h' and saves the new result in memory
    mem_h_min = -10;     
    int16_t min_val_h = -20;

    __asm__ volatile (
        "amomin.h %0, %1, (%2)"
        : "=&r" (original_val)               // %0: rd
        : "r" (min_val_h), "r" (&mem_h_min)  // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    if (original_val == -10 && mem_h_min == -20) {
        printf("\nAMO_MIN.H: PASSED\n");
    } else {
        printf("\nAMO_MIN.H: FAILED\n");
        printf("Expected: original_val=-10, memory_data=-20\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_h_min);
        test_failed = 1;
    }

    // --- 18) TEST AMOMINU.H ---
    //
    // Syntax: amominu.h rd, rs2, (rs1)
    //
    // Description:
    // the instruction loads the actual value of 'mem_h_minu' in 'original_val', 
    // then performs a signed min comparison with 'minu_val_h' and saves the new result in memory
    mem_h_minu = 10;     
    uint16_t minu_val_h = 7;

    __asm__ volatile (
        "amominu.h %0, %1, (%2)"
        : "=&r" (original_val_unsigned)        // %0: rd
        : "r" (minu_val_h), "r" (&mem_h_minu)  // %1: rs2, %2: rs1 (address)
        : "memory"
    );

    if (original_val_unsigned == 10 && mem_h_minu == 7) {
        printf("\nAMO_MINU.H: PASSED\n");
    } else {
        printf("\nAMO_MINU.H: FAILED\n");
        printf("Expected: original_val=10, memory_data=7\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val_unsigned, (long)mem_h_minu);
        test_failed = 1;
    }

    // --- Final Result Report ---
    if(test_failed) {
        printf("\nZABHA_INSTR_TEST: FAILED\n");
        return EXIT_FAILURE;
    } else {
        printf("\nZABHA_INSTR_TEST: PASSED\n");
        return EXIT_SUCCESS;
    }
}
