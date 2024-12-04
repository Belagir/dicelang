# Dicelang

Dicelang is an interpreter to evaluate dice rolls (that you would find in a TTRPG) to visualize odds of getting different values given some formula. It is heavily inspired by [anydice](https://anydice.com/), because I wanted a version of it that would run locally on my machine, and that I would control the oddities and quirks of the script.

## Usage

### Getting dicelang from source

To build from source, you will need `make` and `gcc-13`.
Clone the github repo to some comfortable place in your computer. Make sure the submodules are initialised :

```sh
$ git clone --recurse-submodules ...
```

Then hop in the dicelang folder and run `make`, first for the support library and then for the actual dicelang program :

```sh
$ make -C unstandard
$ make
```

The standalone `dicelang` executable is now in the `bin/` directory.

### Using the program

To interpret a script, supply a file to the executable :

```sh
$ ./dicelang path/to/some-file.dicescript
```

> More way of interacting with the program are coming in the future.

### Live interpreter

> This will come in the future.

## Language

Dicelang is the simplest it can be (or at least tries, dream big little script !). There is only one type of variable in the language, and it is the *distribution*.

A *distribution* is a set of unique real values, each associated to a weight, where the total sum of the weights amount to 1. Those weights are managed as counters under the hood, counting the number of realisations of some value in the sum of all counters.

> Comments in dicelang are prefixed by a `#`. Any character after a `#` is ignored until the end of the line.

A program is made of a succession of statements separated by new line characters (`\n`). Each statement is either a function call or a variable assignment. Those use expressions made of dice declarations and scalar values.

### Variables

Variables hold distributions. You can use all alphanumeric characters to build your variable names, plus `_` to add some breathing space (starting an identifier with a `_` is not permitted though, as I am keeping it for an eventual reserved namespace). Once a variable has been given a value, you can use it in place of an expression in further assignements and function calls.

To assign some value to a variable, use this syntax : `VAR_NAME : EXPR`.
```
some_d20 : 1d20 + 3
another_dice : 2d8
DISTR : some_d20 + another_dice
print(2 * DISTR)
```

### Expressions

Expressions describe a mathematical operation, and may be enclosed in parenthesis. They are made of operands (whole numbers, dice declarations, or variable names) separated by operators (`+`, `-`, `d`, `*`).
Here are some examples of expressions :

- **"Scalars"** `R : 4` to handle single values ;
- **Dices** `R : 3d6` to create distributions of odds ;
- **Addition & substraction** `R : 3d6 + 4 - 1d2` to combine distributions ;
- **Multiplication** `R : 5 * (1d20 + 4)` to repeat some expression.

> Warning : for now the **multiplication is not commutative**. This might change, but given the nature of distributions I might take a little time before figuring it out.

> In the future, will be added :
> - **Arrays** `R : [1, 2, 1d20, 3, 1d3 + 4]` to build arbitrary distributions ;
> - **Comparisons** `R : 1d20 > [13, 15]` to filter distributions against some test.

### Built-in functions

The language uses a set of functions to interact with world and extend the realm of possibilities.
Available functions are :

- `print(EXPR)` will print in stdout the distribution described by the expression `EXPR`.

> In the future, will be added :
> - `mean(R)` to get the mean of a distribution ;
> - `variance(R)` to get the mean square deviation of a distribution ;
> - `write(R)` to output the distribution in some other coherent manner (csv, png, etc) ;
> - `read()` to read from stdin ;
> - `highest(X, R)` to compute the highest X rolls in a distribution ;
> - `lowest(X, R)` to compute the lowest X rolls in a distribution ;
> - `count(X, R)` to count the rolls giving X in a distribution .
