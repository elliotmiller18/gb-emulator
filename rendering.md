1. Call draw_line() function in run() when necessary, in this function we can determine if we can draw a line and if so where we should draw it
2. Get list of objects (not addresses, cache is king)
3. Read VRAM and if no object is present draw window, if no window is present draw object. Also check LCD control for help with this
4. Finally actually write the pixel object to the screen using SDL

Note that this isn’t fully accurate as we should really be using the pixel FIFO but i need to get this DONE!

Normally, we go one pixel per dot but I'm just going to do one scan line per pixels drawn * dot length instead for simplicity's sake. I really don't want to implement cycle by cycle OAM scanning and if I don't do that there's no need to do cycle by cycle pixel drawing

We draw objects first, right to left. Our first object drawn has the lowest priority and our last has the highest riority. This will be done AFTER drawing tilemap, so we only need to use priority in place