# CS214 - Project 1 - WordWrap
# Iman Din - iid5
# Gabriel Parente - gnp32


Testing for Part 1
- Our test cases for Part 1 included testing inputs from stdin, files, and files within a certain directory, and checking the output from stdout or a "wrap." file.
- We tested files of various length, that contained words of various lengths, while also using different widths as input. 
  - This allows us to determine if our program can properly wrap files while not depending on specific file or word sizes.
- Tests also included the use of various combinations of white spaces e.g: "\n \n\t   \n \t".
  - This allows us to determine if our program can handle any combination of white spaces and provide the correct output " " or "\n" or"\n\n".
- We also tested empty files, files that begin with whitespace, and files that do not contain "\n".
  - These tests allow us to account for any strange scenarios that might occur as input
- We also used command line commands such as "diff" and "cmp" to compare our output to the desired output.



Testing for Part 2
- Our test cases for Part 2 included the use of both relative and absolute paths, and non-existant directories.
- It also included the use of regular files, anon-regular, and subdirectories within the given directory.
  - This allows us to determine whether or not our program is able to ignore certain contents inside the given directory.
- Tests also consisted of providing files within the given directory that had names that should be ignored e.g: "wrap.foo or ..".
  - Again, this allowed us to determine if our program is able to ignore files with specific names.
- We also included files with the "wrap." prefix already inside the directory. 
  - This allowed us to test whether or not the program would overwrite the files properly.
