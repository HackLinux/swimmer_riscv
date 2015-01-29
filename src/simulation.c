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
 *     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
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
#include "./inst_decoder.h"
#include "./env.h"
#include "./inst_print.h"

extern void (* const inst_exec_func[])(uint32_t, riscvEnv);

/*!
 * step instruction
 */
void StepSimulation (uint32_t stepCount, riscvEnv env)
{
    for (; stepCount > 0; stepCount - 1) {
        clearTraceInfo (env->trace);
        Word_t    inst_hex = LoadMemory (PCRead (env), Size_Word, env);
        fprintf (env->dbgfp, "%08x : ", inst_hex);
        uint32_t  inst_idx = RISCV_DEC (inst_hex);
        if (inst_idx == -1) {
            fprintf (env->dbgfp, "<Error: instruction is not decoded. %08x\n", inst_hex);
        }
        inst_exec_func[inst_idx] (inst_hex, env);
        fprintf (env->dbgfp, " %30s\n", inst_strings[inst_idx]);
        AdvanceStep (env);
    }
    return;
}
