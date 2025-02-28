X16Cell
=======

Well, it works about as well as Excel does....

In following my tradition of decimal spreadsheet programs, here is this abomination. This implements a twelve digit decimal floating point number. The numerics are ... questionable, to say the least, but it shouldn't be noticeable so long as you are only using the program to manage the budget of a normal person. The math, at this time, only supports round-to-zero.


Numbers
-------

Numbers are eight byte floating point. They inefficiently store the sign in one byte. The exponent can range from -49 to 49 (so that multiplication and division can never overflow a byte). The significand (please don't call it a mantissa) is twelve packed BCD digits. It is normalized, and there is no subnormal support. Zero is unsigned, and infinity and NaN are combined into a single "Err" value. As said before, it only supports round-to-zero.

I do want to point out that the division algorithm in use is the one outlined for use with the Comptometer mechanical adding machine. It looked (and is) actually pretty appropriate for floating point. It's really crazy that Comptometers are half as rare as Curtas, but are a tenth the price on the antique market.


Limits
------

There are 100 rows (0 to 99) and twenty-six columns (A - Z, this is an update that will make old save files load funny). Each cell can have 120 bytes of data in it: the string representation of the cell can be a maximum of 120 bytes. Cell calculation follows the recalculation strategy and will gladly use stale values if that cell hasn't been recomputed yet (and it doesn't do the "normal spreadsheet" thing of doing multiple passes).  
The formula evaluation engine has two stacks: one for data and one for operations. The data stack is fifteen values deep, and the operation stack is thirty one values deep. If I understand the manual correctly, these stacks are larger than a late 90's budget graphing calculator.


Commands
--------

Update: you now need to press `x` _twice_ to delete the current cell's contents.

* `WASD` and Cursor keys - Navigate the sheet
* `z` or Home - Go to cell A0
* `[` - Decrease the size of the current column
* `]` - Increase the size of the current column
* `'` or `"` - Insert a label in this cell, overwriting previous contents
* `=` or `+` - Insert a formula in this cell, overwriting previous contents
* `!` - Recalculate sheet
* `xx` - Delete/clear the current cell's contents
* `xr` - Delete/clear the current row's contents
* `xc` - Delete/clear the current column's contents
* `e` - Edit the current cell (if it has contents)
* `ii` - Insert a cell at this location, pushing current cells right and deleting the right-most cell.
* `ir` - Insert a new row at this row, pushing current rows down and deleting the bottom-most row.
* `ic` - Insert a new column at this column, pushing current columns right and deleting the right-most column.
* `oo` - Insert a cell at this location, pushing current cells down and deleting the bottom-most cell.
* `or` - Insert a new row below this row, pushing current rows down and deleting the bottom-most row.
* `oc` - Insert a new column to the right of this column, pushing current columns right and deleting the right-most column.
* `uu` - Remove the current cell, shifting existing cells left
* `uo` - Remove the current cell, shifting existing cells up
* `ur` - Remove the current row, shifting existing rows up
* `uc` - Remove the current column, shifting existing columns left
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

The save file format is *very* basic: it outputs the characters needed to recreate the sheet from input. As such, the load function merges (and overwrites) the loaded file into whatever is currently loaded.
