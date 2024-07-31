# IZP 2. project
The goal of this project is to create a program that would find a way out of the maze with the right-hand and left-hand rule and show the result step by step.
### Evaluation 12/14

# Usage and arguments
First make the project:
```console
$ make
gcc -std=c11 -Wall -Wextra -Werror maze.c -o maze
```
Now you can start by using:
```console
$ ./keyfilter --help
```

```console
$ ./keyfilter --test example.txt
Valid
```

```console
$ ./keyfilter --rpath ROW COLUMN example.txt
...path
```

```console
$ ./keyfilter --lpath ROW COLUMN example.txt
...path
```
Where _ROW_ and _COLUMN_ is the point of entry.



**Arguments**
- `--help` Will give a little help explanation of usage
- `--test` Will validate the maze from input file
- `--rpath` Will find a way out of the maze with the right-hand rule
- `--lpath` Will find a way out of the maze with the left-hand rule
- `Some file` That contains the maze

# Maze

### Maze file structure
![Maze file structure](https://github.com/prosperritty/uni-izp2/blob/main/img/maze.png)
*This maze is in example.txt*

First two digits in file define the size of maze by:
```
ROW COLUMN
value value value
...
```

### Value of cell
![Value of cell](https://github.com/prosperritty/uni-izp2/blob/main/img/cell.png)

Value of cell is 3 bit and if value of bit is 1, that means there is border:
- First bit defines left border
- Second bit defines rigth border
- Third bit defines upper/bottom border<br>

# Implementation details

### The conditions of the task were
- Implement structure
  ```c
  typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
  } Map;
  ```
  then allocate `*cells` and the structure itself.<br>
  - `rows` is number of rows.
  - `cols` is number of columns.
  - `*cells` is array that contains values of cells and have size of `rows * cols`
- A function that decides whether a certain cell has a border or not. *(see [value of cell](#value-of-cell))*
  ```c
  bool isborder(Map *map, int r, int c, int border);
  ```
- A function that decides where to go first from point of entry.
  ```c
  int start_border(Map *map, int r, int c, int leftright);
  ```
### Other
- Main loop of finding a way out is in function:
  ```c
  void solve_maze(Map *map, int r, int c, int border, int leftright);
  ```
- A function that decides whether a certain cell has an upper border or bottom border.
  ```c
  bool upper_border(int r, int c);
  ```
  Returns `TRUE` if cell has an upper border.
- A function that fills an already allocated array `*cells` in `Map`
  ```c
  void cells_filling(Map *map, FILE* file);
  ```

# Try it!
There is `example.txt`, which you can see in [maze section](#maze) and try it like this:
```console
$ ./maze --rpath 6 1 example.txt
6,1
6,2
5,2
5,3
5,4
6,4
6,3
6,4
6,5
6,6
5,6
5,7
4,7
4,6
4,5
4,4
3,4
3,5
3,6
3,5
3,4
3,3
3,2
3,1
2,1
2,2
2,3
2,4
2,5
2,6
2,7
3,7
```

```console
$ ./maze --lpath 6 1 example.txt
6,1
6,2
5,2
5,3
5,4
6,4
6,5
6,6
5,6
5,7
4,7
4,6
4,5
5,5
4,5
4,4
3,4
3,3
3,2
4,2
4,1
5,1
4,1
4,2
3,2
3,1
2,1
2,2
2,3
2,4
1,4
1,3
1,2
1,1
```
  
