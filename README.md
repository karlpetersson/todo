## TODO

### INSTALLATION

Download the repo, and then `make` followed by `make install` in the root folder. This will install todo in `/usr/local/bin`, and copy `todoStyles.json` to `/usr/local/etc`.

### USAGE

Whenever you run `todo`, the program will locate the closest file named `todo.txt` starting in the current directory and going up. `todo init` will create this file for you in the directory you're currently in. The commands available are listed below (each command has a shortcut which is the first letter, i.e. `todo finish` is equivalent to `todo f`)

Create todo.txt in current directory
```
todo init
```

List todos
```
todo
```

Add a todo
```
todo add <text>
```

Finish a todo
```
todo finish <num>
```

Toggle priority on a todo
```
todo prio <num>
```

Load from sourcefiles (searches files for "TODO:")
```
todo load <files>
```

Interactive mode
```
todo i
```

#### During interactive mode

Use `up` and `down` arrow keys to select todo  
press `a` to add a new todo, press `ENTER` when done  
press `f` on a selected todo to finish it  
press `p` to toggle priority on selected todo  
press `q` or `ESC` to exit  

### Custom text-styles
Styles define how the todos are rendered to the console, both in normal and interactive mode, and are defined in `todoStyles.json`. The active style is defined by the attribute "activeStyle", and can be set by using the keys in the "styles"-array.

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

##### Example
`$(C7)$(N)  $(R)$(C5)$(P)$(R)$(T)` will render todos with the line number $(N) in white (C7), render two spaces, reset the colors with $(R), render the priority character $(R) in magenta (C5), reset again, and then render the todo-text with $(T).

##### Available text colors and background colors
0 = Black  
1 = Red  
2 = Green  
3 = Yellow  
4 = Blue  
5 = Magenta  
6 = Cyan  
7 = White  

##### Available extras
0 = Bold  
1 = Underline  
2 = Invert color  

