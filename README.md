# Terminal Sorting Visualizer

A minimalist, zero-dependency CLI tool to visualize how sorting algorithms manipulate memory in real-time. Built entirely with fundamental C—just basic arrays, loops, and conditionals. No bloated libraries, no dynamic allocation, just raw logic.

## What It Does
Instead of dry running arrays on paper, this program lets you input a sequence of numbers and renders live ASCII bars (`#`) directly in your terminal. As the algorithm swaps or shifts elements, the terminal clears and redraws, creating a crisp frame-by-frame animation of the sorting process. 

## Features
* **Raw C Implementation:** Built strictly using `if-else` branching, fundamental loops, and static 1D arrays. Standard GCC compatible.
* **Live Memory Visualization:** Watch the exact moment elements swap positions. 
* **Zero Overhead:** Uses standard ANSI escape codes for screen clearing and `<unistd.h>` for frame pacing.
