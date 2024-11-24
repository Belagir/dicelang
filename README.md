# Dicelang

Abstract goes here

## Usage

Usage goes here

### Scripting

...

### Live interpreter

...

## Language

Dicelang is the simplest it can be (or at least tries, dream big little script !). There is only one type of variable in the language, and it is the *distribution*. A *distribution* is a set of unique real values, each associated to a weight, where the total sum of the weights amount to 1.

> Comments in dicelang are prefixed by a `#`. Any character after a `#` is ignored.

### Variables

### Expressions

*"Scalars"* `R : 4`
*Dices* `R : 3d6`
*Arrays* `R : [1, 2, 1d20, 3, 4]`
*Addition & substraction* `R : 3d6+4`
*Multiplication & division* `R : 5 * [1, 2]`
*Comparing* `R : 1d20 > [13, 15]`

### Built-in functions

```
M : mean(R)
V : variance(R)
write(R)
R : read()
print(R)
```

### Built-in mutations

```
T : highest{X, R}
T : lowest{X, R}
T : count{X, R}
```
