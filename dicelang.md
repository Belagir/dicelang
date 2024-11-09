# Syntax

```
# bases
R : 4
R : 1d3
R : 4d6 + 1 - d20
R : [1, 2, 2, 3, 4, 10]
R : 2d20 + [3,4]
R : 2d20 * 1d6
R : (3d6 + 1.5) / 1d4
```
Evaluation of formulas (the translation from a string to a collection of normalized weighted values) is done gradually, with caching of the formula's parts for later reuse. Though the interpreter should keep track of the total formula present in a variable for later mutation functions. Maybe the variables are only support for text replacement ?

Addition/substraction concatenates two distributions, and adds weight to values that compare equal. Multiplication/division results in the combination of the two distributions.

```
# modification
R : 4d6-1d20
R : R-2d6
# R : (4d6-1d20)-2d6
```
Re-affecting a formula to the same name works without breaking anything.

```
# comparison
R: 2d6 > 3
R: 1d20+4 <= 18
R: 3d4 > 1d6
R: 1d12 = [ 3, 6, 9, 1 ]
```
Comparing formulas with a scalar is straightforward. Comparing two formulas is less so. The members are moved around to create a new equation where an operand is a scalar, e.g. `2d4+6 > d6` simply becomes `2d4+6-d6 > 0`.

```
# accessing stuff
R[0]
R[2]
# etc

M : mean(R)
V : variance(R)
```
With `[]`, we can access stuff inside a formula's distribution. accessing out of bound is caught and stops the interpreter with an error.

```
# inout functions
write(R)
R : read()
print(R)
```
`write` outputs a formula to stdout, while `read` will take a formula from stdin. If `read` does not receive a valid dice formula, then the interpreter stops with an error.

```
# mutation functions
highest{X, R}
lowest{X, R}
count{X, R}
```
Mutation functions change how a formula is processed by the interpreter. `highest` takes the `X` highest throws in the formula, with `lowest` doing the opposite. `count` will output a formula representing the probability of getting each possible quantity of `X`.

# Scripting language details

- Token `line_end` separates statements

- Token `file_end` ends the script

- Token `identifier` names a variable or function

- Token `value` encodes a scalar

- Token `value_real` encodes a scalar with a decimal part

- Token `separator` lives between function arguments and between array declarations

- Token `binary_operand` lives between two expressions
  - Token `addition`
  - Token `substraction`

- Token `unary_operand` lives before an expression
  - Token `d`

- Token `designator` links a variable to an expression

- Token `paired_token` requires an opening token and an ending token
  - Token `open_sq_bracket`
  - Token `close_sq_bracket`
  - Token `open_bracket`
  - Token `close_bracket`
  - Token `open_parenthesis`
  - Token `close_parenthesis`
