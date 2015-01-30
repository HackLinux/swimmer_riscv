/*
 * Copyright (c) 2015, Masayuki Kimura
 * All rights reserved.
 *
 *     Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Masayuki Kimura nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
n *     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL MASAYUKI KIMURA BE LIABLE FOR ANY
 *     DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *      LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *     ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "./inst_operand.h"
#include "./inst_print.h"

extern char *inst_strings [];
extern operandList inst_operand[];

void PrintInst (uint32_t inst_hex, uint32_t inst_idx,
                char *str_out, const uint32_t length,
                riscvEnv env)
{
    char *str_head = str_out;
    char *inst_str = inst_strings[inst_idx];
    uint32_t replace_idx = 0;
    while (inst_str[0] != '\0') {
        if (replace_idx > inst_operand[inst_idx].size) {
            fprintf (stderr, "<Internal Error: instruction format index exceeded\n");
            exit (EXIT_FAILURE);
        }
        if (inst_str[0] == '@') {
            int i;
            uint32_t msb_bit = inst_operand[inst_idx].msb_lst[replace_idx];
            uint32_t lsb_bit = inst_operand[inst_idx].lsb_lst[replace_idx];
            uint32_t type    = inst_operand[inst_idx].type_lst[replace_idx];
            uint32_t length_field = msb_bit - lsb_bit + 1;
            length_field = length_field % 4 == 0 ? (length_field / 4) : (length_field / 4 + 1);
            uint32_t operand_bit =  ExtractBitField (inst_hex, msb_bit, lsb_bit);
            uint8_t disp_array[8];
            uint32_t divide_base;
            switch (type) {
            case operandTypeHex : divide_base = 16; break;
            case operandTypeDec : divide_base = 10; break;
            case operandTypeSB  : divide_base = 16; break;
            case operandTypeUJ  : divide_base = 16; break;
            }
            for (i = 0; i < length_field; i++) {
                disp_array[i] = operand_bit % divide_base;
                operand_bit = operand_bit / divide_base;
            }
            switch (type) {
            case operandTypeHex : sprintf (str_head, "0x"); str_head +=2; break;
            case operandTypeDec : sprintf (str_head, "r");  str_head +=1; break;
            case operandTypeSB  : sprintf (str_head, "0x"); str_head +=2; break;
            case operandTypeUJ  : sprintf (str_head, "0x"); str_head +=2; break;
            }

            for (i = length_field - 1; i >= 0; i--) {
                if (divide_base == 16) {
                    str_head[0] = (disp_array[i] < 10) ? disp_array[i] + 0x30 : disp_array[i] + 0x57;
                } else if (divide_base == 10) {
                    str_head[0] = disp_array[i] + 0x30;
                }
                str_head ++;
            }
            replace_idx ++;
        } else {
            sprintf (str_head, "%c", inst_str[0]);
            str_head++;
        }
        inst_str++;
    }
    str_head[0] = '\0'; str_head ++;
}


void PrintOperand (riscvEnv env)
{
    uint32_t trace_count;
    for (trace_count = 0; trace_count < env->trace->max; trace_count ++) {
        switch (env->trace->trace_type[trace_count]) {
        case trace_regwrite :
            if (env->trace->trace_addr[trace_count] == REG_PC) {
                fprintf (env->dbgfp, "pc<=%08x ",
                         env->trace->trace_value[trace_count]);
            } else {
                fprintf (env->dbgfp, "r%02d<=%08x ",
                         env->trace->trace_addr[trace_count],
                         env->trace->trace_value[trace_count]);
            }
            break;
        case trace_regread :
            fprintf (env->dbgfp, "r%02d=>%08x ",
                     env->trace->trace_addr[trace_count],
                     env->trace->trace_value[trace_count]);
            break;
        case trace_memwrite :
            fprintf (env->dbgfp, "(%08x)<=%08x ",
                     env->trace->trace_addr[trace_count],
                     env->trace->trace_value[trace_count]);
            break;
        case trace_memread :
            fprintf (env->dbgfp, "(%08x)=>%08x ",
                     env->trace->trace_addr[trace_count],
                     env->trace->trace_value[trace_count]);
            break;
        }
    }
}
