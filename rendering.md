1. Call draw_line() function in run() when necessary, in this function we can determine if we can draw a line and if so where we should draw it
2. Get list of objects (not addresses, cache is king)
3. Read VRAM and if no object is present draw window, if no window is present draw object. Also check LCD control for help with this
4. Finally actually write the pixel object to the screen using SDL

Note that this isn’t fully accurate as we should really be using the pixel FIFO but i need to get this DONE!