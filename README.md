# Calculator

This is a simple command line calculator written in C language.

## How to use it

To use this calculator, you can clone the repository to local, then compile the source file `calculator.c`, and run the generated executable file:

```powershell
> git clone https://github.com/ragerAssist/calculator.git
> cd .\calculator\
> gcc .\calculator.c -o .\calculator.exe
> .\calculator.exe
```

When entering an expression, you can enter multiple lines. Each line represents a mathematical expression. For each line you enter, if the entered expression is correct, the result will be output immediately. You can end the input by typing `CTRL+Z`.

In addition, you can also pass a parameter to the program to specify the path of the file containing the mathematical expression. The program will calculate the mathematical expression contained in the file. If there are multiple mathematical expressions, each mathematical expression will be calculated separately, and the calculation results will be output in separate lines:

```powershell
> .\calculator.exe "path\to\your\file"
```

## What does it support

The calculator supports standard mathematical operations and a set of functions.

### Operators

* `+` Addition / Positive sign
* `-` Subtraction / Negative sign
* `*` Multiplication
* `/` Division

### Functions

* `sin(...)` Sine (angle, not radian)
* `cos(...)` Cosine (angle, not radian)
* `sqrt(...)` Square root

## What causes an error to occur

Any illegal token will cause an error, the following are all legal tokens:

* Parentheses: `(`, `)` 
* Numeric operand: `32`, `3.2`, `32.`
* Operators: `+`, `-`, `*`, `/`
* Functions: `sin`, `cos`, `sqrt`

It should be noted that:

* Parentheses must match
* The numeric operand can be an integer or a decimal, the decimal point is not allowed to appear in the first position, that is, `.32` is an illegal token
* `+` and `-` can be unary operators or binary operators, `*` and `/` must be binary operators
* The divisor cannot be zero
* The function call must have a pair of parentheses, that is, `sin30` is wrong























