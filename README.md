X16Cell
=======

Well, it works about as well as Excel does....

In following my tradition of decimal spreadsheet programs, here is this abomination. This implements an eight digit decimal floating point number. The numerics are ... sloppy, to say the least, but it shouldn't be noticeable so long as you are only using the program to manage the budget of a normal person. Not a millionaire. The math, at this time, only supports round-to-zero* (*round-to-zero is not always round-to-zero, see further notes).


Numbers
-------

Numbers are 6 byte floating point. They inefficiently store the sign in one byte. The exponent can range from -49 to 49 (so that multiplication and division can never overflow a byte). The significand (please don't call it a mantissa) is eight packed BCD digits. It is normalized, and there is no subnormal support. Zero is unsigned, and infinity and NaN are combined into a single "Err" value. As said before, it only supports round-to-zero. However, because it doesn't use a sticky digit, `1 - 1e-15` is not `0.99999999` like it ought to be. That is the only deviation from round-to-zero that I am aware of.

I do want to point out that the division algorithm in use is the one outlined for use with the Comptometer mechanical adding machine. It looked (and is) actually pretty appropriate for floating point. It's really crazy that Comptometers are half as rare as Curtas, but are a tenth the price on the antique market.


Limits
------

There are 100 rows (0 to 99) and twenty columns (A - T). Each cell can have 120 bytes of data in it: the string representation of the cell can be a maximum of 120 bytes. Cell calculation follows the recalculation strategy and will gladly use stale values if that cell hasn't been recomputed yet (and it doesn't do the "normal spreadsheet" thing of doing multiple passes).  
The formula evaluation engine has two stacks: one for data and one for operations. The data stack is fifteen values deep, and the operation stack is thirty one values deep. If I understand the manual correctly, these stacks are larger than a late 90's budget graphing calculator.

Update: there are now 26 columns.


Commands
--------

* `WASD` and Cursor keys - Navigate the sheet
* `z` or Home - Go to cell A0
* `[` - Decrease the size of the current column
* `]` - Increase the size of the current column
* `'` or `"` - Insert a label in this cell, overwriting previous contents
* `=` or `+` - Insert a formula in this cell, overwriting previous contents
* `!` - Recalculate sheet
* `x` - Delete the current cell's contents
* `e` - Edit the current cell (if it has contents)
* `q` - Quit: you must follow a `q` with a `y` to really quit.
* `,` - Toggle comma as the output decimal separator
* `j` - Toggle row/column-major recalculation
* `k` - Toggle left-to-right or right-to-left recalculation
* `l` - Toggle top-to-bottom or bottom-to-top recalculation
* `n` - Save file (use contents of current cell as file name)
* `m` - "Load" file (use contents of current cell as file name)
* `ENTER` - Exit edit mode


Formulas
--------

Formulas are very basic infix notation:  
* `5 + 2 * 3 - 7 / (3 + -5)`  
You can call functions using the really old notation:  
* `3 + @sum(A0; 2; 3; A2:B4)`  
Cells are reference-able by name (but note that anchoring `$A$12` is *not* supported):  
* `A2`  
Cell ranges are only accepted by certain functions (SUM, AVERAGE, COUNT, MIN, and MAX):  
* `@sum(A0:c5)`


Functions
---------

The list of functions:
* AVERAGE
* SUM
* COUNT
* MIN
* MAX
* ROUND
* TRUNC
* ABS


Files
-----

The save file format is *very* basic: it outputs the characters needed to recreate the sheet from input. As such, the load function merges (and overwrites) the loaded file into whatever is currently loaded. It is very basic, but even that made me fight with my compiler due to my 16K (now 14K) table.
