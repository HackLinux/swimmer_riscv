/*
 * Copyright (c) 2012, Masayuki Kimura
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

#include "./env.h"
#include "./trace.h"

traceInfo TraceInfo (void)
{
    traceInfo trace = (traceInfo)checked_malloc (sizeof (traceInfo *));
    clearTraceInfo (trace);
    return trace;
}


void clearTraceInfo (traceInfo trace)
{
    trace->max = 0;
    trace->isbranch = false;
}


void RecordTraceGRegRead  (traceInfo trace, RegAddr_t reg, Word_t value)
{
    uint32_t max = trace->max;
    if (max < TRACE_MAX) {
        trace->trace_type [max] = trace_regread;

        trace->trace_addr [max] = reg;
        trace->trace_value[max] = value;

        trace->max++;
    }

    return;
}


void RecordTraceGRegWrite (traceInfo trace, RegAddr_t reg, Word_t value)
{
    uint32_t max = trace->max;
    if (max < TRACE_MAX) {
        trace->trace_type [max] = trace_regwrite;

        trace->trace_addr [max] = reg;
        trace->trace_value[max] = value;

        trace->max++;
    }

    if (reg == REG_PC) {
        trace->isbranch = true;
    }

    return;
}


void RecordTraceMemRead (traceInfo trace, Addr_t addr, Word_t value, Size_t size)
{
    uint32_t max = trace->max;
    if (max < TRACE_MAX) {
        trace->trace_type [max] = trace_memread;

        trace->trace_addr [max] = addr;
        trace->trace_value[max] = value;

        trace->max++;
    }

    return;
}


void RecordTraceMemWrite (traceInfo trace, Addr_t addr, Word_t value, Size_t size)
{
    uint32_t max = trace->max;
    if (max < TRACE_MAX) {
        trace->trace_type [max] = trace_memwrite;

        trace->trace_addr [max] = addr;
        trace->trace_value[max] = value;

        trace->max++;
    }

    return;
}
