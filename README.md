# myFind

## Usage

    myFind path [options] [expressions] [actions]

`myFind` recursively searches the path specified in the first parameter for files that matches given criteria. Relative paths are then passed to the action definitions.

### Options

Following options are supported:

`-follow` option will resolve symlinks and will dive into directory simlinks

`-nofollow` option will not resolve symlinks and will not dive into directory simlinks, criteria will be compared with the symlink itself (this is the default behaviour)

`-ignorehidden` option will ignore hidden files - files beginning with . (this is the default behaviour)

`-noignorehidden` option will not ignore hidden files - files not beginning with .

`-mindepth n` option will set the lowest level in directory tree before the search terms will be tested

`-maxdepth n` option will set the highest level in directory tree after the search terms will no longer be tested

### Expressions

Following expressions are supported:

#### Name matching

`name filename` will match the files with specified filename pattern (case sensitive)

`iname filename` will match the files with specified filename pattern (case insensitive)

#### Time matching

`amin n` will match files accessed n minutes ago

`atime n` will match files accessed n days ago

`anewer filename` will match files accessed more recently than filename

`cmin n` will match files accessed n minutes ago

`ctime n` will match files accessed n days ago

`cnewer filename` will match files accessed more recently than filename

`mmin n` will match files accessed n minutes ago

`mtime n` will match files accessed n days ago

`mnewer filename` will match files accessed more recently than filename

#### Size matching

`empty` will match empty regular files and empty directories

`size n` will match the files that are n bytes large

#### Ownership matching

`gid n` will match file group id with provided number

`group name` will match file group with the id of the provided group name

`uid n` will match file owner with provided number

`user name` will match file owner with the id of the provided user name

#### Operators

Listed in order of decreasing precedence:

`( expr )` operator will force precedence of the expr contained within

`! expr`, `not expr` will negate the truth value of the expr

`expr1 and expr2`, `expr1 expr2` will be true if both expressions are true

`expr1 or expr2` will be true if at least one of the expressions are true

#### Misc

`true` will always be true

`false` will never be true

#### Numeric formats

Numeric formats can be specified as:

`+n` for expressing numbers greater than n

` n` for expressing numbers equal to n

`-n` for expressing numbers smaller than n

### Action

If no action is defined, `print` is used

`print` prints the file's relative paths to standard output

`exec executable [params | {}] ;` runs the executable every time the file is matched with specified parameters and all `{}` parameters are replaced with the relative paths of the matched file

## Code 

This program is implemented using evaluation tree. At the start of the program the arguments of the program are parsed and a evaluation tree is build with operators as inner nodes. The nodes have a pointer to function and two structs that may carry the data depending on function. The actions are parsed into linked list of functions and parameters.

Once arguments are parsed the program goes through the directory and passes all the filenames to the root of the evaluation tree. If the function returns true all functions in the linked list are called with the relative path of the matched file as parameter. If the file is a directory then the program recursively searches this directory.

`main.c` main program, launched parameter parser and file crawler

`parser.c` parses the arguments and builds the evaluation tree

`crawler.c` contains the code for going through the files

`action.c` contains the functions for actions that will be run on matched files

`checker.c` contains the functions for checking the files

`common.c` contains misc functions

