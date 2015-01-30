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

#pragma once

#include <stdint.h>
#include "./basic.h"

#define TRACE_MAX 32

typedef enum {trace_regwrite,
              trace_regread,
              trace_memread,
              trace_memwrite} traceType;


typedef struct __traceInfo *traceInfo;

struct __traceInfo {
    uint32_t max;
    bool     isbranch;

    traceType trace_type [TRACE_MAX];

    /* for Register Read/Write */
    Addr_t    trace_addr [TRACE_MAX];
    Word_t    trace_value[TRACE_MAX];
};


traceInfo TraceInfo (void);
void clearTraceInfo (traceInfo);
void RecordTraceGRegRead  (traceInfo, RegAddr_t, Word_t);
void RecordTraceGRegWrite (traceInfo, RegAddr_t, Word_t);
void RecordTraceMemRead   (traceInfo, Addr_t, Word_t, Size_t);
void RecordTraceMemWrite  (traceInfo, Addr_t, Word_t, Size_t);
