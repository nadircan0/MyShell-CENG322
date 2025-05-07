# MyShell - CENG322

## Overview

This repository contains a custom-built shell program written in **C**. The shell supports various features like:

- **Built-in commands**: 
  - `cd` (change directory)
  - `pwd` (print working directory)
  - `history` (view command history)
  - `exit` (exit the shell)

- **Piping**: 
  - Pipe (`|`) to connect two commands, e.g., `ls -l | wc -l`.
  
- **Logical AND (`&&`)**: 
  - Chain commands using `&&`, ensuring that the second command only runs if the first one is successful.

- **Background processes**: 
  - Use the `&` operator to run processes in the background.

## Features

1. **Command Parsing**: 
   - Continuously waits for user input, parses the command, and executes it accordingly.
  
2. **Piping Between Commands**: 
   - Supports piping between two commands, redirecting the output of one command as input to another.
  
3. **Handling Background Processes**: 
   - Allows commands to run in the background, freeing up the shell for other tasks.

4. **Command History**: 
   - Keeps track of the most recently entered commands in the shell, with support for viewing the history.

## How to Use

1. Clone the repository:
    ```bash
    git clone https://github.com/yourusername/MyShell-CENG322.git
    ```

2. Navigate to the directory:
    ```bash
    cd MyShell-CENG322
    ```

3. Compile the shell program:
    ```bash
    gcc -o shell shell.c
    ```

4. Run the shell:
    ```bash
    ./shell
    ```

5. Start using the shell by typing commands like:
    - `pwd`
    - `cd /path/to/directory`
    - `ls -l | wc -l`
    - `gcc main.c && ./a.out`
    - `some_command &`

## License

This project is open-source and available under the MIT License.

---

Feel free to contribute, report issues, or suggest improvements!
