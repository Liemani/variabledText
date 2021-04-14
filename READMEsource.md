project name = variabledText
first title = File Format
second title = Feature
third title = Use
fourth title = Additional
coding time = ???
library = https://github.com/Liemani/cLibrary.git

# \(project name)

## Index

- \(first title)
- \(second title)
- \(third title)
- \(fourth title)

## \(first title)

```text
// no empty line
<variable identifier> = <value>
<variable identifier> = <value>
...

<text>
```

- `<variable identifier>` can't have ')' character.
- Between variable part and text part, exist only one empty line.

## post script

You can use this program recursively by using the output of this program as input, but not intended behaviour.

## \(second title)

- Default destination file extension is `.lmtdestination`.

## \(third title)

```zsh
% make
...
% ./\(project name) -o README.md READMEsource.md
%
```

`./\(project name) [-o \<destination file\>] \<source file\>`

## \(fourth title)

- I have another work to do, so i made this project ASAP, so codes are not good, if you want to see better code, check `cLibrary` link below, which is used for this project.
- I want to add some another feature, but not now.
  - Function to replace all a variable name to another name.
  - Support seperation between text file listed variables and text file to apply.
  - Program displaying value of variable at real titme.
- This README.md is also managed by using \(project name) (Created from READMEsource.md)

Time take: \(coding time)
Using library: \(library)