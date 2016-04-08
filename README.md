## TODO

### INSTALLATION

Download the repo, and then `make` followed by `make install` in the root folder. This will install todo in `/usr/local/bin`, and copy `todoStyles.json` to `~/`.

### USAGE

List todos:
```
todo
```

Add a todo:
```
todo add <text>
```

Finish a todo:
```
todo do <num>
```

Set priority on a todo (0 or 1):
```
todo prio <num> <prio>
```

Load from sourcefiles (searches files for "TODO:")
```
todo l <files>
```

Interactive mode:
```
todo i
```

#### During interactive mode

Use `up` and `down` arrow keys to select todo  
press `a` to add a new todo, press `ENTER` when done  
press `d` on a selected todo to finish it  
press `p` to toggle priority on selected todo  
press `ESC` to exit  

### Custom text-styles
Styles define how the todos are rendered to the console, both in normal and interactive mode.Custom styles are defined in `todoStyles.json`. The active style is defined by the attribute "activeStyle", and can be changed to the key of any of the styles defined in the "styles"-array.

#### Syntax (work in progress)
A style is defined by three strings (in the json-file); the "line" and "selected" attributes, which describes how a line is styled when its not selected or when its selected(interactive mode), as well as the attribute "prioString" which determines how the priority of a todo should be shown. 

The "line" and "selected" strings are constructed using a syntax of variables enclosed in $().

`$(N)` determines where the line number should be rendered
`$(P)` is the position of the priority identifier  
`$(T)` is the actual todo-text  

The above variables need to be present and in the following order: `$(N)$(P)$(T)`

Any additional characters can be added before, after or between them.

Text color can be added with the `$(cX)` variable, where `X` is a number between 0 and 7.
Background color can be added with the `$(bX)` variable, where `X` is a number between 0 and 7.
Extras color can be added with the `$(sX)` variable, where `X` is a number between 0 and 2.

Use the variable `$(R)` to reset styles (color, background etc). Needed if for instance you want the line number to be in a different color.

Below is a table of available ANSI text colors and background colors:
0 = Black
1 = Red
2 = Green
3 = Yellow
4 = Blue
5 = Magenta
6 = Cyan
7 = White

Below is the available extras:
0 = BOLD
1 = Underline
2 = Invert color

