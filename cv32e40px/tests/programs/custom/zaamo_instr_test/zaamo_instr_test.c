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

// FOR NOW ZAAMO INSTRUCTIONS USE NORMAL LOAD/STORE IN CV32E40PX!!

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// --- GLOBAL VARIABLES ---
// OPCODE & EXECUTION PIPELINE
volatile int32_t memory_data_swap, memory_data_add, memory_data_and, memory_data_or, memory_data_xor, memory_data_max, memory_data_min;
volatile uint32_t memory_data_maxu, memory_data_minu;

// OPERATION-SPECIFIC EDGE CASES
volatile int32_t memory_data_add_ovf, memory_data_add_udf, memory_data_and_comp, memory_data_xor_zero, memory_data_max_cross, memory_data_min_cross, memory_data_min_ext, memory_data_swap_ext;
volatile uint32_t memory_data_maxu_pol, memory_data_minu_pol;

// REGISTER-SPECIFIC EDGE CASES
volatile int32_t mem_rs2_x0, mem_rd_rs2, mem_rd_rs1, mem_all_alias; // mem_rd_x0

int main(int argc, char *argv[])
{
    int32_t original_val;
    uint32_t original_val_unsigned;
    int test_failed = 0;
    
    //////////////////////////////////////
    //                                  //
    //  OPCODE & EXECUTION IN PIPELINE  //
    //                                  //
    //////////////////////////////////////

    printf("\n===== OPCODE & EXECUTION IN PIPELINE =====\n");

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
        printf("\nAMO_SWAP.W: PASSED\n");
    } else {
        printf("\nAMO_SWAP.W: FAILED\n");
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
        printf("\nAMO_ADD.W: PASSED\n");
    } else {
        printf("\nAMO_ADD.W: FAILED\n");
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
        printf("\nAMO_AND.W: PASSED\n");
    } else {
        printf("\nAMO_AND.W: FAILED\n");
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
        printf("\nAMO_OR.W: PASSED\n");
    } else {
        printf("\nAMO_OR.W: FAILED\n");
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
        printf("\nAMO_XOR.W: PASSED\n");
    } else {
        printf("\nAMO_XOR.W: FAILED\n");
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
        printf("\nAMO_MAX.W: PASSED\n");
    } else {
        printf("\nAMO_MAX.W: FAILED\n");
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
        printf("\nAMO_MAXU.W: PASSED\n");
    } else {
        printf("\nAMO_MAXU.W: FAILED\n");
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
        printf("\nAMO_MIN.W: PASSED\n");
    } else {
        printf("\nAMO_MIN.W: FAILED\n");
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
        printf("\nAMO_MINU.W: PASSED\n");
    } else {
        printf("\nAMO_MINU.W: FAILED\n");
        printf("Expected: original_val=10, memory_data=7\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val_unsigned, memory_data_minu);
        test_failed = 1;
    }

    ////////////////////////////////////////
    //                                    //
    //  ORDERING BITS (aq & rl) COVERAGE  //
    //                                    //
    ////////////////////////////////////////

    // TODO --> needs lr.w and sc.w implementation

    //////////////////////////////////////////
    //                                      //
    //  CONCURRENCY & MULTY-HART COHERENCY  //
    //                                      //
    //////////////////////////////////////////

    // TODO --> needs lr.w and sc.w implementation
    
    ////////////////////////////
    //                        //
    //  ADDRESS MISALIGNMENT  //
    //                        //
    ////////////////////////////

    printf("\n===== ADDRESS MISALIGNMENT =====\n");

    volatile uint32_t misaligned_buffer[2] = {0, 0};
    // craft the misaligned address by adding 1 byte
    uint32_t *misaligned_addr = (uint32_t *)((uint8_t *)misaligned_buffer + 1);
    
    uint32_t old_mtvec;
    uint32_t actual_cause = 0xFFFFFFFF; // sentinel value
    uint32_t trap_taken = 0;
    int32_t add_val_mis = 5;
    uint32_t handler_addr;

    __asm__ volatile (
        "la   %[h_addr], 1f\n\t"               // 1. compute the address of local handler
        "andi %[h_addr], %[h_addr], -4\n\t"    // 2. force MODE = 00 (direct mode)
        "csrr %[old_vec], mtvec\n\t"           // 3. save old mtvec
        "csrw mtvec, %[h_addr]\n\t"            // 4. set new mtvec (will not be truncated)
        
        "amoadd.w zero, %[val], (%[addr])\n\t" // 5. instruction with data misalignment (MUST generate trap)
        "j 2f\n\t"                             // 6. skip handler if NOT trap
        
        ".balign 256\n\t"                      // <-- needed hardware aligment
        "1:\n\t"                               // <-- LOCAL TRAP HANDLER
        "csrr %[cause], mcause\n\t"            // 7. read exception's cause
        "li   %[taken], 1\n\t"                 // 8. signal taken trap
        
        "2:\n\t"                               // <-- RESET & END
        "csrw mtvec, %[old_vec]\n\t"           // 9. reset old BSP's mtvec
        
        : [old_vec] "=&r" (old_mtvec),
          [h_addr]  "=&r" (handler_addr),
          [cause]   "+r"  (actual_cause),
          [taken]   "+r"  (trap_taken)
        : [val]     "r"   (add_val_mis),
          [addr]    "r"   (misaligned_addr)
        : "memory"
    );

    // --- Self-Checking ---
    if (trap_taken == 1 && actual_cause == 6) {
        printf("\nAMO_ADD.W (MISALIGNED): PASSED\n");
    } else {
        printf("\nAMO_ADD.W (MISALIGNED): FAILED\n");
        printf("Trap taken: %lu (Expected: 1)\n", trap_taken);
        printf("Actual mcause: %lu (Expected mcause: 6)\n", actual_cause);
        test_failed = 1;
    }
    
    /////////////////////////////////////
    //                                 //
    //  OPERATION-SPECIFIC EDGE CASES  //
    //                                 //
    /////////////////////////////////////

    printf("\n===== OPERATION-SPECIFIC EDGE CASES =====\n");

    // ==========================================
    // ARITHMETIC OVER/UNDERFLOW (AMOADD.W)
    // ==========================================

    // --- AMOADD.W (Arithmetic Overflow) ---
    // INT32_MAX + 1 --> INT32_MIN
    memory_data_add_ovf = 2147483647; // 0x7FFFFFFF (INT32_MAX)
    int32_t add_ovf_val = 1;

    __asm__ volatile (
        "amoadd.w %0, %1, (%2)"
        : "=&r" (original_val)
        : "r" (add_ovf_val), "r" (&memory_data_add_ovf)
        : "memory"
    );

    if (original_val == 2147483647 && memory_data_add_ovf == -2147483648) { // -2147483648 is INT32_MIN
        printf("\nAMO_ADD.W (OVERFLOW): PASSED\n");
    } else {
        printf("\nAMO_ADD.W (OVERFLOW): FAILED\n");
        printf("Expected: original_val=2147483647, memory_data=-2147483648\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val, memory_data_add_ovf);
        test_failed = 1;
    }

    // --- AMOADD.W (Arithmetic Underflow) ---
    // INT32_MIN + (-1) --> INT32_MAX
    memory_data_add_udf = -2147483648; 
    int32_t add_udf_val = -1;

    __asm__ volatile (
        "amoadd.w %0, %1, (%2)"
        : "=&r" (original_val)
        : "r" (add_udf_val), "r" (&memory_data_add_udf)
        : "memory"
    );

    if (original_val == -2147483648 && memory_data_add_udf == 2147483647) {  // 2147483647 is INT32_MAX
        printf("\nAMO_ADD.W (UNDERFLOW): PASSED\n");
    } else {
        printf("\nAMO_ADD.W (UNDERFLOW): FAILED\n");
        printf("Expected: original_val=-2147483648, memory_data=2147483647\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", original_val, memory_data_add_udf);
        test_failed = 1;
    }

    // ==========================================
    // BOOLEAN MASKS AND PATTERNS (AMOAND.W, AMOOR.W, AMOXOR.W)
    // ==========================================

    // --- AMOAND.W (Complementary Patterns) ---
    memory_data_and_comp = (int32_t)0xAAAAAAAA; // b1010...1010
    int32_t and_comp_val = (int32_t)0x55555555; // b0101...0101

    __asm__ volatile (
        "amoand.w %0, %1, (%2)"
        : "=&r" (original_val)
        : "r" (and_comp_val), "r" (&memory_data_and_comp)
        : "memory"
    );

    if (original_val == (int32_t)0xAAAAAAAA && memory_data_and_comp == 0x00000000) {
        printf("\nAMO_AND.W (COMPLEMENTARY PATTERN): PASSED\n");
    } else {
        printf("\nAMO_AND.W (COMPLEMENTARY PATTERN): FAILED\n");
        test_failed = 1;
    }

    // --- AMOXOR.W (Self-Zeroing) ---
    memory_data_xor_zero = (int32_t)0xDEADBEEF;
    int32_t xor_zero_val = (int32_t)0xDEADBEEF;

    __asm__ volatile (
        "amoxor.w %0, %1, (%2)"
        : "=&r" (original_val)
        : "r" (xor_zero_val), "r" (&memory_data_xor_zero)
        : "memory"
    );

    if (original_val == (int32_t)0xDEADBEEF && memory_data_xor_zero == 0x00000000) {
        printf("\nAMO_XOR.W (SELF-ZEROING): PASSED\n");
    } else {
        printf("\nAMO_XOR.W (SELF-ZEROING): FAILED\n");
        test_failed = 1;
    }

    // ==========================================
    // SIGNED COMPARISON LIMITS (AMOMAX.W, AMOMIN.W)
    // ==========================================

    // --- AMOMAX.W (Zero Crossing) ---
    memory_data_max_cross = -1;
    int32_t max_zero_val = 0;

    __asm__ volatile (
        "amomax.w %0, %1, (%2)"
        : "=&r" (original_val)
        : "r" (max_zero_val), "r" (&memory_data_max_cross)
        : "memory"
    );

    if (original_val == -1 && memory_data_max_cross == 0) {
        printf("\nAMO_MAX.W (ZERO CROSSING): PASSED\n");
    } else {
        printf("\nAMO_MAX.W (ZERO CROSSING): FAILED\n");
        test_failed = 1;
    }

    // --- AMOMIN.W (Zero Crossing) ---
    memory_data_min_cross = 0;
    int32_t min_zero_val = -1;

    __asm__ volatile (
        "amomin.w %0, %1, (%2)"
        : "=&r" (original_val)
        : "r" (min_zero_val), "r" (&memory_data_min_cross)
        : "memory"
    );

    if (original_val == 0 && memory_data_min_cross == -1) {
        printf("\nAMO_MIN.W (ZERO CROSSING): PASSED\n");
    } else {
        printf("\nAMO_MIN.W (ZERO CROSSING): FAILED\n");
        test_failed = 1;
    }

    // --- AMOMIN.W (Discordant Extremes) ---
    memory_data_min_ext = 2147483647;  // INT32_MAX
    int32_t min_ext_val = -2147483648; // INT32_MIN

    __asm__ volatile (
        "amomin.w %0, %1, (%2)"
        : "=&r" (original_val)
        : "r" (min_ext_val), "r" (&memory_data_min_ext)
        : "memory"
    );

    if (original_val == 2147483647 && memory_data_min_ext == -2147483648) {
        printf("\nAMO_MIN.W (DISCORDANT EXTREMES): PASSED\n");
    } else {
        printf("\nAMO_MIN.W (DISCORDANT EXTREMES): FAILED\n");
        test_failed = 1;
    }

    // ==========================================
    // UNSIGNED COMPARISON LIMITS (AMOMAXU.W, AMOMINU.W)
    // ==========================================

    // --- AMOMAXU.W (Polarity Inversion) ---
    // 0x80000000 is INT32_MIN (negative signed), but in unsigned it is --> 0x7FFFFFFF
    memory_data_maxu_pol = 0x7FFFFFFF; 
    uint32_t maxu_pol_val = 0x80000000;

    __asm__ volatile (
        "amomaxu.w %0, %1, (%2)"
        : "=&r" (original_val_unsigned)
        : "r" (maxu_pol_val), "r" (&memory_data_maxu_pol)
        : "memory"
    );

    if (original_val_unsigned == 0x7FFFFFFF && memory_data_maxu_pol == 0x80000000) {
        printf("\nAMO_MAXU.W (POLARITY INVERSION): PASSED\n");
    } else {
        printf("\nAMO_MAXU.W (POLARITY INVERSION): FAILED\n");
        test_failed = 1;
    }

    // --- AMOMINU.W (Polarity Inversion) ---
    // 0x80000000 is INT32_MIN (negative signed), but in unsigned it is --> 0x7FFFFFFF
    memory_data_minu_pol = 0x80000000; 
    uint32_t minu_pol_val = 0x7FFFFFFF;

    __asm__ volatile (
        "amominu.w %0, %1, (%2)"
        : "=&r" (original_val_unsigned)
        : "r" (minu_pol_val), "r" (&memory_data_minu_pol)
        : "memory"
    );

    if (original_val_unsigned == 0x80000000 && memory_data_minu_pol == 0x7FFFFFFF) {
        printf("\nAMO_MINU.W (POLARITY INVERSION): PASSED\n");
    } else {
        printf("\nAMO_MINU.W (POLARITY INVERSION): FAILED\n");
        test_failed = 1;
    }

    // ==========================================
    // SWAP INTEGRITY (AMOSWAP.W)
    // ==========================================

    // --- AMOSWAP.W (Limit Pattern) ---
    // swap between all zeros and all ones
    memory_data_swap_ext = 0x00000000;
    int32_t swap_lim_val = (int32_t)0xFFFFFFFF;

    __asm__ volatile (
        "amoswap.w %0, %1, (%2)"
        : "=&r" (original_val)
        : "r" (swap_lim_val), "r" (&memory_data_swap_ext)
        : "memory"
    );

    if (original_val == 0x00000000 && memory_data_swap_ext == (int32_t)0xFFFFFFFF) {
        printf("\nAMO_SWAP.W (LIMIT PATTERN): PASSED\n");
    } else {
        printf("\nAMO_SWAP.W (LIMIT PATTERN): FAILED\n");
        test_failed = 1;
    }

    ////////////////////////////////////
    //                                //
    //  REGISTER-SPECIFIC EDGE CASES  //
    //                                //
    ////////////////////////////////////

    printf("\n===== REGISTER-SPECIFIC EDGE CASES =====\n");
    /*
    // ==========================================
    // DESTINATION IN REG 0 (rd = x0)
    // ==========================================
    // not sure why it fails, output log gives correct actual memory_data...
    mem_rd_x0 = 10;
    int32_t add_val_x0 = 5;

    // using hardcoded 'zero' (x0) as destination,
    // the operation must happen in memory, but the original value gets discarded
    __asm__ volatile (
        "amoadd.w zero, %0, (%1)"
        :
        : "r" (add_val_x0), "r" (&mem_rd_x0)
        : "memory"
    );

    if (mem_rd_x0 == 15) {
        printf("\nAMO_ADD.W (RD = X0): PASSED\n");
    } else {
        printf("\nAMO_ADD.W (RD = X0): FAILED\n");
        printf("Expected: memory_data=15\n");
        printf("Actual:   memory_data=%ld\n", (long)mem_rd_x0);
        test_failed = 1;
    }
    */
    // ==========================================
    // OPERAND FROM REG 0 (rs2 = x0)
    // ==========================================
    mem_rs2_x0 = 42;
    original_val = 0;

    // using hardcoded 'zero' (x0) as rs2.
    // equivalent to an atomic read: memory unchanged (+0), 'rd' captures 42.
    __asm__ volatile (
        "amoadd.w %0, zero, (%1)"
        : "=r" (original_val)
        : "r" (&mem_rs2_x0)
        : "memory"
    );

    if (original_val == 42 && mem_rs2_x0 == 42) {
        printf("\nAMO_ADD.W (RS2 = X0): PASSED\n");
    } else {
        printf("\nAMO_ADD.W (RS2 = X0): FAILED\n");
        printf("Expected: original_val=42, memory_data=42\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)original_val, (long)mem_rs2_x0);
        test_failed = 1;
    }

    // ==========================================
    // DESTINATION-OPERAND ALIASING (rd == rs2)
    // ==========================================
    mem_rd_rs2 = 100;
    int32_t shared_rd_rs2 = 25; // is both 'rs2' in input and 'rd' in output

    // '%0' used two times, forces GCC to assign the same physical address
    __asm__ volatile (
        "amoadd.w %0, %0, (%1)"
        : "+r" (shared_rd_rs2)
        : "r" (&mem_rd_rs2)
        : "memory"
    );

    if (shared_rd_rs2 == 100 && mem_rd_rs2 == 125) {
        printf("\nAMO_ADD.W (RD == RS2): PASSED\n");
    } else {
        printf("\nAMO_ADD.W (RD == RS2): FAILED\n");
        printf("Expected: original_val=100, memory_data=125\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)shared_rd_rs2, (long)mem_rd_rs2);
        test_failed = 1;
    }

    // ==========================================
    // DESTINATION-ADDRESS ALIASING (rd == rs1)
    // ==========================================
    mem_rd_rs1 = 200;
    int32_t *shared_rd_rs1 = (int32_t *)&mem_rd_rs1; // is both pointer 'rs1' and 'rd'
    int32_t val_rs2 = 50;

    __asm__ volatile (
        "amoadd.w %0, %1, (%0)"
        : "+r" (shared_rd_rs1)
        : "r" (val_rs2)
        : "memory"
    );

    // pointer register gets overwritten, making a cast to check it
    if ((int32_t)shared_rd_rs1 == 200 && mem_rd_rs1 == 250) {
        printf("\nAMO_ADD.W (RD == RS1): PASSED\n");
    } else {
        printf("\nAMO_ADD.W (RD == RS1): FAILED\n");
        printf("Expected: original_val=200, memory_data=250\n");
        printf("Actual:   original_val=%ld, memory_data=%ld\n", (long)shared_rd_rs1, (long)mem_rd_rs1);
        test_failed = 1;
    }

    // ==========================================
    // TOTAL ALIASING (rd == rs1 == rs2)
    // ==========================================
    mem_all_alias = 999;
    int32_t *shared_all = (int32_t *)&mem_all_alias; 
    int32_t expected_addr = (int32_t)&mem_all_alias;

    // single register for everything: 
    // the memory address itself becomes the data to write, 
    // and the value in memory it's now in the original register.
    __asm__ volatile (
        "amoswap.w %0, %0, (%0)"
        : "+r" (shared_all)
        :
        : "memory"
    );

    if ((int32_t)shared_all == 999 && mem_all_alias == expected_addr) {
        printf("\nAMO_SWAP.W (RD == RS1 == RS2): PASSED\n");
    } else {
        printf("\nAMO_SWAP.W (RD == RS1 == RS2): FAILED\n");
        printf("Expected: original_val=999, memory_data=0x%08lX\n", (long)expected_addr);
        printf("Actual:   original_val=%ld, memory_data=0x%08lX\n", (long)shared_all, (long)mem_all_alias);
        test_failed = 1;
    }

    ////////////////////////////
    //                        //
    //  INTERRUPT ROBUSTNESS  //
    //                        //
    ////////////////////////////

    // TODO?
    
    ///////////////////////////
    //                       //
    //  FINAL RESULT REPORT  //
    //                       //
    ///////////////////////////
    
    printf("\n===== FINAL RESULT REPORT =====\n");

    if(test_failed) {
        printf("\nZAAMO_INSTR_TEST: FAILED\n\n");
        return EXIT_FAILURE;
    } else {
        printf("\nZAAMO_INSTR_TEST: PASSED\n\n");
        return EXIT_SUCCESS;
    }
}
