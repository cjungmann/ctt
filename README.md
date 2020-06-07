# Project C Terminal Tools

This project creates a library of C-language tools that can
help with the development console-resident C language programs.

This library will probably grow as I continue to learn and use
new techniques for writing command line programs.  For now, I
am migrating a few utilities into this single project so I can
make these tools available with a single library inclusion.

## Heap vs Stack memory

I am trying to design the interfaces to be memory-agnostic,
so a developer can decide to use either stack or heap memory.
Sometimes this may lead to somewhat contorted interfaces, but
I think it's worth preserving the option to use any type of
memory.

## Library Contents

This list is growing.  As I write this, there are three modules
in the library:

1. **line_reader** for buffered line reading.  The buffer size
   can be changed.

2. **key_reader** reads a single keystroke to a buffer.  This
   is useful for control or function keys that return multiple
   characters for a keypress, such as an arrow key.

3. **procopts** this is a new name and new interpretation of my
   earlier [clargs](https://github.com/cjungmann/clargs) project.
   The developer creates an array of structs that the module
   uses to interpret a user's input.

## Continuing Work

I have a few more ideas that I'm working on.  They will reside
in example code until I feel like they're ready to be included
as a module.  For example, I am working on a method of displaying
a list of strings from which the user will select one.

## Example Files

Until I get around to making a **man** page for the library,
sample source files will show how these modules are used.  In many
cases, the source files will include a *compile-command* string
that tells EMACS how to compile that source file, outside of the
project Makefile.  To compile, open the file in EMACS and type
`M-x compile`.

