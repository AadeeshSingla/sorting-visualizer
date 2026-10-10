/*
 * ============================================================================
 *  TERMINAL SORTING VISUALIZER  (Bubble Sort)
 * ============================================================================
 *  Build:  gcc -Wall -Wextra -o sort_visualizer sort_visualizer.c
 *  Run:    ./sort_visualizer
 *
 *  Constraints respected:
 *    - only a basic 1D int array, if-else, and for/while loops
 *    - no structs, no malloc, no complex data structures
 *    - one small helper function (draw_frame) so the drawing code is not
 *      copy-pasted; everything else lives in main()
 *
 *  Mental model of memory:
 *
 *      arr[] is a contiguous block of ints:
 *
 *        index :   [0]   [1]   [2]   [3]  ...
 *        value :    5     2     9     1
 *
 *      The CPU can't swap two memory cells directly. It needs a third
 *      storage spot (the temp variable) to hold one value while the other
 *      one is overwritten:
 *
 *          tmp      = arr[j];       // 1. save arr[j] before it's destroyed
 *          arr[j]   = arr[j + 1];   // 2. overwrite arr[j] with its neighbour
 *          arr[j+1] = tmp;          // 3. put the saved value in the neighbour
 *
 *      Every time that happens, we clear the screen and redraw the bars.
 * ============================================================================
 */

#include <stdio.h>
#include <unistd.h>   /* usleep() */

#define MAX_N      20        /* maximum number of elements              */
#define MAX_VALUE  999       /* largest accepted value                  */
#define MAX_BAR    50        /* longest bar, in '#' characters          */
#define DELAY_US   300000    /* pause between frames (microseconds)     */

/* ANSI escape codes */
#define CLEAR_SCREEN "\033[H\033[J"   /* cursor to top-left, clear below */
#define GREEN        "\033[32m"       /* elements just swapped           */
#define CYAN         "\033[36m"       /* elements in final position      */
#define RESET        "\033[0m"

/*
 * draw_frame: clears the terminal and draws one bar per array element.
 *
 *   arr         - the array being sorted (read-only here)
 *   n           - number of elements
 *   hot         - index of the LEFT cell of the pair that was just swapped
 *                 (cells hot and hot+1 are highlighted); -1 = no highlight
 *   sorted_from - every index >= sorted_from is already in its final
 *                 position (bubble sort locks in one element per pass)
 *   pass        - current pass number
 *   swaps       - total memory swaps so far
 *   compares    - total comparisons so far
 *   max_val     - biggest value in the array, used to scale the bars
 */
void draw_frame(int arr[], int n, int hot, int sorted_from,
                int pass, int swaps, int compares, int max_val)
{
    int i, k, len;

    printf(CLEAR_SCREEN);
    printf("BUBBLE SORT  |  pass %d  |  swaps %d  |  comparisons %d\n",
           pass, swaps, compares);
    printf("%sgreen%s = just swapped   %scyan%s = final position\n\n",
           GREEN, RESET, CYAN, RESET);

    for (i = 0; i < n; i++) {
        /* ---- decide bar length (scale down if the numbers are big) ---- */
        if (max_val <= MAX_BAR) {
            len = arr[i];
        } else {
            len = arr[i] * MAX_BAR / max_val;
        }
        if (arr[i] > 0 && len == 0) {
            len = 1;                       /* never hide a non-zero value */
        }

        /* ---- label: memory slot index and the value stored there ---- */
        printf("arr[%2d] = %3d | ", i, arr[i]);

        /* ---- pick a colour for this bar ---- */
        if (hot >= 0 && (i == hot || i == hot + 1)) {
            printf(GREEN);                 /* these two cells were swapped */
        } else if (i >= sorted_from) {
            printf(CYAN);                  /* already locked in place      */
        }

        /* ---- the bar itself ---- */
        for (k = 0; k < len; k++) {
            putchar('#');
        }
        printf(RESET "\n");
    }

    /* Show which swap just happened (the memory operation). */
    printf("\n");
    if (hot >= 0) {
        printf("tmp = arr[%d]; arr[%d] = arr[%d]; arr[%d] = tmp;\n",
               hot, hot, hot + 1, hot + 1);
    } else {
        printf("(no swap in this frame)\n");
    }

    fflush(stdout);   /* make sure the frame appears before we sleep */
}

int main(void)
{
    int arr[MAX_N];        /* the array we will sort IN PLACE            */
    int n = 0;             /* how many elements the user wants           */
    int i, j;              /* loop counters                              */
    int tmp;               /* third storage cell used for swapping       */
    int c;                 /* used to flush bad input                    */
    int ok;                /* return value of scanf                      */
    int max_val;           /* largest element (for bar scaling)          */
    int swaps = 0;         /* total swaps performed                      */
    int compares = 0;      /* total comparisons performed                */
    int swapped;           /* did this pass swap anything?               */
    int sorted_from;       /* indices >= this are finished               */
    int pass;              /* pass counter (for display)                 */

    /* ------------------------------------------------------------------
     *  INPUT: how many numbers?
     * ------------------------------------------------------------------ */
    printf("=== Terminal Sorting Visualizer ===\n\n");

    while (n < 2 || n > MAX_N) {
        printf("How many numbers do you want to sort (2-%d)? ", MAX_N);
        ok = scanf("%d", &n);
        if (ok == EOF) {
            return 1;                      /* input closed, give up      */
        }
        if (ok != 1) {
            n = 0;                         /* not a number: ask again    */
            c = getchar();
            while (c != '\n' && c != EOF) {   /* discard the bad line    */
                c = getchar();
            }
        }
    }

    /* ------------------------------------------------------------------
     *  INPUT: the numbers themselves, stored into consecutive cells
     * ------------------------------------------------------------------ */
    printf("Enter %d integers between 0 and %d (space or newline separated):\n",
           n, MAX_VALUE);

    i = 0;
    while (i < n) {
        ok = scanf("%d", &arr[i]);
        if (ok == EOF) {
            return 1;
        }
        if (ok == 1 && arr[i] >= 0 && arr[i] <= MAX_VALUE) {
            i++;                           /* valid: move to next cell   */
        } else {
            printf("  Invalid value for arr[%d]. Try again: ", i);
            if (ok != 1) {
                c = getchar();
                while (c != '\n' && c != EOF) {
                    c = getchar();
                }
            }
        }
    }

    /* Find the maximum once so bars can be scaled to fit the terminal. */
    max_val = arr[0];
    for (i = 1; i < n; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }

    /* Initial frame: the unsorted array exactly as stored in memory. */
    sorted_from = n;                       /* nothing is sorted yet      */
    draw_frame(arr, n, -1, sorted_from, 0, swaps, compares, max_val);
    usleep(DELAY_US * 3);                  /* longer pause to read it    */

    /* ------------------------------------------------------------------
     *  BUBBLE SORT
     *
     *  Each pass walks left to right comparing neighbours arr[j], arr[j+1].
     *  If they are out of order we swap them, so the largest remaining
     *  value "bubbles" to the right end. After pass p, the last p cells
     *  are in their final positions, so each pass can stop one cell sooner.
     * ------------------------------------------------------------------ */
    for (pass = 1; pass < n; pass++) {
        swapped = 0;

        for (j = 0; j < n - pass; j++) {
            compares++;

            if (arr[j] > arr[j + 1]) {
                /* ---------- THE MEMORY SWAP ---------- */
                tmp        = arr[j];       /* 1. stash left value        */
                arr[j]     = arr[j + 1];   /* 2. right value moves left  */
                arr[j + 1] = tmp;          /* 3. stashed value moves right */
                /* -------------------------------------- */

                swaps++;
                swapped = 1;

                /* Redraw: memory just changed, so show the new state. */
                draw_frame(arr, n, j, sorted_from,
                           pass, swaps, compares, max_val);
                usleep(DELAY_US);
            }
        }

        /* The biggest unsorted value has reached index n - pass. */
        sorted_from = n - pass;

        /* No swaps in a full pass means the array is already sorted. */
        if (swapped == 0) {
            sorted_from = 0;
            break;
        }
    }

    /* Final frame: everything is in its final position. */
    draw_frame(arr, n, -1, 0, pass, swaps, compares, max_val);
    printf("\nDone! Sorted with %d swaps and %d comparisons.\n",
           swaps, compares);

    return 0;
}
