/*
 *     unblackedges.c
 *     by Jia Wen Goh (jgoh01) and Winston Tan (wtan02), 09/21/2017
 *
 *     Unblackedges removes black edges from a scanned image (or any
 *     given .pbm file).
 */

#include <stdio.h>
#include <stdlib.h>
#include <except.h>
#include <bit2.h>
#include <pnmrdr.h>

/* Exceptions */
const Except_T Error_arg = {"More than one argument given"};
const Except_T Error_fo  = {"Error opening file"};
const Except_T Error_pbm = {"Given file is not a valid .pbm"};

/* --- File Processing Functions --- */
void   process_file     (FILE* fp);
Bit2_T store_bitmap     (FILE* fp);
void   verify_bitmap    (Pnmrdr_mapdata pix_data);
Bit2_T edge_store       (Bit2_T page);

/* --- Black-Edge Functions --- */
void unblack_edges          (Bit2_T image, Bit2_T blackedge_map);
int  scan_black             (Bit2_T blackedge, Bit2_T page);
int  black_edge_verification(Bit2_T blackedge, Bit2_T page, int col, int row);

/* --- Output Functions --- */
void pbmwrite           (FILE *outputfp, Bit2_T bitmap);
void print_bit_row      (int col, int row, Bit2_T bit2, int bit, void* cl);

/*
 * [Name]:              main
 * [Purpose]:           main call; processes the .pbm file given on the cmnd
 *                      line/stdin (i.e. unblack edges of the given file)
 * [Parameters]:        1 int (argc), 1 char* (argv[])
 * [Returns]:           EXIT_SUCCESS if the supplied file is processed;
 *                      (note: an exception is raised if given file cannot
 *                      be opened)
 */
int main(int argc, char* argv[]) {
        if (argc > 2) {
                RAISE(Error_arg);
        } else if (argc != 1) {
                FILE* inputfp;
                inputfp = fopen(argv[1], "r");
                if (inputfp == NULL) {
                        RAISE(Error_fo);
                }

                process_file(inputfp);

                fclose(inputfp);
                exit(EXIT_SUCCESS);
        } else {
                process_file(stdin);
                exit(EXIT_SUCCESS);
        }
}

/*----------------------------------------------------------------*
 |                 File Processing Functions                      |
 *----------------------------------------------------------------*/
/*
 * [Name]:              process_file
 * [Purpose]:           Main file processing to unblacken edges for bit maps
 * [Parameters]:        FILE* (input file stream w/ the .pbm)
 * [Returns]:           void
 */
void process_file(FILE* inputfp)
{
        Bit2_T image = store_bitmap(inputfp);
        Bit2_T blackedge_map = edge_store(image);

        unblack_edges(image, blackedge_map);
        pbmwrite(stdout, image);

        Bit2_free(&image);
        Bit2_free(&blackedge_map);
}

/*
 * [Name]:              store_bitmap
 * [Purpose]:           Stores the given file into a Bit2
 *                      (note: if given .pbm isn't a valid bitmap, i.e.
 *                      given file is not of type bitmap, then raise an
 *                      exception)
 * [Parameters]:        FILE* (input file stream w/ the .pbm to be stored)
 * [Returns]:           Bit2_T (.pbm stored into a Bit2)
 */
Bit2_T store_bitmap(FILE* fp)
{
        Pnmrdr_T bitfile = Pnmrdr_new(fp);
        Pnmrdr_mapdata pix_data = Pnmrdr_data(bitfile);

        verify_bitmap(pix_data);

        Bit2_T bitmap = Bit2_new(pix_data.width, pix_data.height);
        for (int row = 0; row < (int)pix_data.height; row++) {
                for (int col = 0; col < (int)pix_data.width;  col++) {
                        Bit2_put(bitmap, col, row, Pnmrdr_get(bitfile));
                }
        }

        Pnmrdr_free(&bitfile);
        return bitmap;
}

/*
 * [Name]:              verify_bitmap
 * [Purpose]:           If the given .pbm isn't a valid bitmap, i.e.
 *                      given file is not of type bitmap, then raise an
 *                      exception
 * [Parameters]:        Pnmrdr_mapdata (the mapdata of the given file)
 * [Returns]:           void (but an exception is raised if the given file
 *                      isn't a bitmap)
 */
void verify_bitmap(Pnmrdr_mapdata pix_data)
{
        if (pix_data.type != Pnmrdr_bit) {
                RAISE(Error_pbm);
        }
}

/*
 * [Name]:              edge_store
 * [Purpose]:           Store black edges in another bitmap
 * [Parameters]:        Bit2_T (bitmap to be analyzed)
 * [Returns]:           Bit2_T (bitmap of blackedges)
 */
Bit2_T edge_store(Bit2_T page)
{
        Bit2_T blackedge = Bit2_new(Bit2_width(page), Bit2_height(page));

        for (int col = 0; col < Bit2_width(page); col++) {
                Bit2_put(blackedge, col, 0, Bit2_get(page, col, 0));
                Bit2_put(blackedge, col, Bit2_height(blackedge) - 1,
                         Bit2_get(page, col, Bit2_height(page) - 1));
        }
        for (int row = 0; row < Bit2_height(page); row++) {
                Bit2_put(blackedge, 0, row, Bit2_get(page, 0, row));
                Bit2_put(blackedge, Bit2_width(blackedge) - 1, row,
                         Bit2_get(page, Bit2_width(page) - 1, row));
        }

        int done = 0;
        while (!done) {
                done = 0;
                done = scan_black(blackedge, page);
        }
        return blackedge;
}

/*----------------------------------------------------------------*
 |               Black-Edge Processing Functions                  |
 *----------------------------------------------------------------*/
/*
 * [Name]:              unblack_edges
 * [Purpose]:           Chage black edge pixels in image to white
 * [Parameters]:        2 Bit2_T (bitmap to unblack, bitmap storing black edges)
 * [Returns]:           void
 */
void unblack_edges(Bit2_T image, Bit2_T blackedge_map)
{
        for (int row = 0; row < Bit2_height(blackedge_map); row++) {
                for (int col = 0; col < Bit2_width(blackedge_map); col++) {
                        if (Bit2_get(blackedge_map, col, row)) {
                                Bit2_put(image, col, row, 0);
                        }
                }
        }
}

/*
 * [Name]:              scan_black
 * [Purpose]:           Scan for remaining black edges
 * [Parameters]:        2 Bit2_T (bitmap to scan, bitmap to store black edges)
 * [Returns]:           int (if no black edges are detected, returns 1)
 */
int scan_black(Bit2_T blackedge, Bit2_T page)
{
        int done = 1;
        for (int row = 1; row < Bit2_height(blackedge) - 1; row++) {
                for (int col = 1; col < Bit2_width(blackedge) - 1; col++) {
                        /* Scan file from Top to Bottom */
                        if (black_edge_verification(blackedge, page,
                                                    col, row)) {
                                done = 0;
                        }
                        /* Scan file from Bottom up */
                        if (black_edge_verification(blackedge, page,
                            Bit2_width(page) - 1 - col,
                            Bit2_height(page) - 1 - row)) {
                                done = 0;
                        }
                }
        }
        return done;
}

/*
 * [Name]:              black_edge_verification
 * [Purpose]:           Verify if given bit (at col/row index) is a black edge
 * [Parameters]:        2 Bit2_T (bitmap to scan, bitmap to store black edges)
 *                      2 int (column and row indices)
 * [Returns]:           int (if given bit is a black edge, returns 1, else 0)
 */
int black_edge_verification(Bit2_T blackedge, Bit2_T page, int col, int row)
{
        /* If Bit2(page) == black && Bit2(blackedge) == white,
         *      > Check for surrounding black edges */
        if (Bit2_get(page, col, row) && !Bit2_get(blackedge, col, row)) {
                /* Check if neighbouring cells are black */
                if (Bit2_get(blackedge, col - 1, row) ||
                    Bit2_get(blackedge, col + 1, row) ||
                    Bit2_get(blackedge, col, row - 1) ||
                    Bit2_get(blackedge, col, row + 1)) {
                        Bit2_put(blackedge, col, row, 1);
                        return 1;
                }
        }

        return 0;
}

/*----------------------------------------------------------------*
 |                       Output Functions                         |
 *----------------------------------------------------------------*/
/*
 * [Name]:              pbmwrite
 * [Purpose]:           Prints unblacked file onto output file stream
 * [Parameters]:        1 FILE* (output stream), Bit2_T (bitmap to print)
 * [Returns]:           void
 */
void pbmwrite(FILE *outputfp, Bit2_T bitmap)
{
        fprintf(outputfp, "P1 \n");
        fprintf(outputfp, "# processed unblackedges file\n");
        fprintf(outputfp, "%d %d \n", Bit2_width(bitmap), Bit2_height(bitmap));
        Bit2_map_row_major(bitmap, print_bit_row, outputfp);
}

/*
 * [Name]:              print_bit_row
 * [Purpose]:           Output bitmap to designated stream using map_row_major
 * [Parameters]:        2 ints ( col and row indices), Bit2_T (bit2 to print)
 *                      int (bit to be printed), void* (output file stream)
 * [Returns]:           void
 */
void print_bit_row(int col, int row, Bit2_T bit2, int bit, void* cl)
{
        FILE* outputfp = (FILE*) cl;
        fprintf(outputfp, "%d", bit);

        /* Check for 70 character limit */
        int width = Bit2_width(bit2);
        if (width > 35) {
                if ((col + row * width + 1) % 35 == 0) {
                        fprintf(outputfp, "\n");
                } else {
                        fprintf(outputfp, " ");
                }
        } else {
                if (col == Bit2_width(bit2) - 1) {
                        fprintf(outputfp, "\n");
                } else {
                        fprintf(outputfp, " ");
                }
        }
}
