/******************************************************************************
 *
 * File:        enkf_update.c        
 *
 * Created:     12/2012
 *
 * Author:      Pavel Sakov
 *              Bureau of Meteorology
 *
 * Description:
 *
 * Revisions:
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "version.h"
#include "definitions.h"
#include "utils.h"
#include "enkfprm.h"
#include "dasystem.h"

int calcspread = 0;
int leavetiles = 0;
int outputinc = 0;

/**
 */
static void usage()
{
    enkf_printf("  Usage: enkf_update <prm file> [<options>]\n");
    enkf_printf("  Options:\n");
    enkf_printf("  --calculate-spread\n");
    enkf_printf("      calculate ensemble spread and write to %s\n", FNAME_SPREAD);
    enkf_printf("  --describe-prm-format [main|model|grid]\n");
    enkf_printf("      describe format of a parameter file and exit\n");
    enkf_printf("  --direct-write\n");
    enkf_printf("      write fields directly to the output file (default: write to tiles first)\n");
    enkf_printf("  --leave-tiles\n");
    enkf_printf("      do not delete tiles\n");
    enkf_printf("  --output-increment\n");
    enkf_printf("      output analysis increment (default: output analysis)\n");
    enkf_printf("  --separate-output\n");
    enkf_printf("      write results to new files (default: append to forecast files)\n");
    enkf_printf("  --version\n");
    enkf_printf("      print version and exit\n");

    exit(0);
}

/**
 */
static void parse_commandline(int argc, char* argv[], char** fname)
{
    int i;

    if (argc < 2)
        usage();

    i = 1;
    while (i < argc) {
        if (argv[i][0] != '-') {
            if (*fname == NULL) {
                *fname = argv[i];
                i++;
                continue;
            } else
                usage();
        } else if (strcmp(argv[i], "--calculate-spread") == 0) {
            calcspread = 1;
            i++;
            continue;
        } else if (strcmp(argv[i], "--describe-prm-format") == 0) {
            if (i < argc - 1) {
                if (strcmp(argv[i + 1], "main") == 0)
                    enkfprm_describeprm();
                else if (strcmp(argv[i + 1], "model") == 0)
                    model_describeprm();
                else if (strcmp(argv[i + 1], "grid") == 0)
                    grid_describeprm();
                else
                    usage();
            } else
                enkfprm_describeprm();
            exit(0);
        } else if (strcmp(argv[i], "--direct-write") == 0) {
            enkf_directwrite = 1;
            i++;
            continue;
        } else if (strcmp(argv[i], "--leave-tiles") == 0) {
            leavetiles = 1;
            i++;
            continue;
        } else if (strcmp(argv[i], "--output-increment") == 0) {
            outputinc = 1;
            i++;
            continue;
        } else if (strcmp(argv[i], "--separate-output") == 0) {
            enkf_separateout = 1;
            i++;
            continue;
        } else if (strcmp(argv[i], "--version") == 0) {
            enkf_printversion();
            exit(0);
        } else
            usage();
    }

    if (*fname == NULL)
        enkf_quit("command line: parameter file not specified");
}

/**
 */
int main(int argc, char* argv[])
{
    char* fname_prm = NULL;
    enkfprm* prm = NULL;
    dasystem* das = NULL;

    parse_commandline(argc, argv, &fname_prm);

    enkf_init(&argc, &argv);
    enkf_printf("  running UPDATE for EnKF version %s:\n", ENKF_VERSION);
    enkf_printtime("  ");

    enkf_printf("  reading system specs from \"%s\":\n", fname_prm);
    prm = enkfprm_read(fname_prm);
    enkfprm_print(prm, "    ");

    enkf_printf("  initialising the system:\n");
    das = das_create(prm);
    enkfprm_destroy(prm);

    if (outputinc)
        das->target = TARGET_INCREMENT;

    if (nprocesses == 1 && !enkf_directwrite) {
        enkf_printf("  nproc = 1 -> using direct write\n");
        enkf_directwrite = 1;
    } else if (!enkf_directwrite)
        enkf_printf("  using assembled write\n");
    else
        enkf_printf("  using direct write\n");

    if (das->mode == MODE_ENKF)
        enkf_printf("  updating the ensemble:\n");
    else if (das->mode == MODE_ENOI)
        enkf_printf("  updating the model state:\n");
    das_update(das, calcspread, leavetiles);

    das_destroy(das);

    enkf_finish();

    return 0;
}
