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

#include <unistd.h>
#include <string.h>
#include "./swimmer_main.h"
#include "./simulation.h"
#include "./env.h"

int main (int argc, char *argv[])
{
    FILE *hexfp;
    FILE *debugfp = stdout;

    char debug_out = false;    //
    char *debug_filename = NULL,
        *input_filename = NULL;

    /*!
     * variables for getopt
     */
    int     ch;
    extern char *optarg;
    extern int  optind, opterr;
    uint32_t  max_cycle = 65536;   // max cycle (default is 65536)

    while ((ch = getopt(argc, argv, "h:o:c:")) != -1){
        switch (ch){
        case 'h':  // hex file
            input_filename = optarg;
            break;
        case 'o':  // output file
            debug_filename = optarg;
            debug_out = true;
            break;
        case 'c':  // max cycle
            max_cycle = atoi (optarg);
            break;
        default:
            usage(stderr);
        }
    }
    argc -= optind;
    argv += optind;
    if (input_filename == NULL) {
        fprintf (stderr, "Please specify input binary file\n\n");
        display_info (stderr);
        usage (stderr);
        exit (EXIT_FAILURE);
    }

    // opening debug out
    if (debug_out == true) {
        if ((debugfp = fopen(debug_filename, "w")) == NULL) {
            perror ("fopen");
            exit (EXIT_FAILURE);
        }
    }

    // opening input binary file
    if ((hexfp = fopen(input_filename, "r")) == NULL) {
        perror ("fopen");
        exit (EXIT_FAILURE);
    }

    display_info (stdout);

    // for instruction simulation mode
    riscvEnv env = CreateNewRISCVEnv (debugfp);
    env->max_cycle = max_cycle;  // set maximum cycle

    LoadSrec (hexfp, env);

    // simulation start
    FormatOperand ();
    env->pc = 0x00000000;
    int count;
    for (count = 0; count < env->max_cycle; count++) {
        StepSimulation (1, env);
    }

    fclose (hexfp);
    return 0;
}

/*!
 * print mipsim start file
 * \param fp   file pointer
 */
void printStart (FILE *fp)
{
    fprintf (fp, "// Swimmer-RISCV\n");
    fprintf (fp, "//  Version %s Revision %s\n", VERSION, REVISION);
    return;
}


/*!
 * display usage
 * \param fp   file pointer to be displaying of usage information
 */
void usage (FILE *fp)
{
    fprintf (fp, "\n");
    fprintf (fp, "  usage : swimmer_riscv -h <filename>\n\n");
    fprintf (fp, "Options\n");
    fprintf (fp, "    -c <int>   : simulation step\n");
    fprintf (fp, "    -o <log>   : log file name\n");

    return;
}

/*!
 * display usage
 * \param fp   file pointer to be displaying of usage information
 */
void display_info (FILE *fp)
{
    fprintf (fp, "Swimmer-RISCV\n");
    fprintf (fp, "  Version %s Revision %s\n", VERSION, REVISION);
    fprintf (fp, "  developed by Masayuki Kimura <masayuki.kimura.1986@gmail.com>\n");
    return;
}
