# variabledText

## Index

- File Format
- Feature
- Use
- Additional

## File Format

```text
// no empty line
\import <file name to import>
<variable identifier> = <value>
<variable identifier> = <value>
...

<text>
```

- You can read variable from other files.
- `<variable identifier>` can't have ')' character.
- Between variable part and text part, exist only one empty line.

## Feature

- Default destination file extension is `.lmtdestination`.

## Use

```zsh
% make
...
% ./variabledText -o README.md READMEsource.md
%
```

`./variabledText [-o <destination file>] <source file>`

## Additional

- You can use this program recursively by using the output of this program as input, but not intended behaviour.
- I have another work to do, so i made this project ASAP, so codes are not good, if you want to see better code, check `cLibrary` link below, which is used for this project.
- I want to add some another feature, but not now.
  - Function to replace all a variable name to another name.
  - Support seperation between text file listed variables and text file to apply.
  - Program displaying value of variable at real titme.
- This README.md is also managed by using variabledText (Created from READMEsource.md)

Using library: [https://github.com/Liemani/cLibrary.git](https://github.com/Liemani/cLibrary.git)
