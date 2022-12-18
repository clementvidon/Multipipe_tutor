<h1 align="center">
    Multipipe_tutor
</h1>

<h3 align="center">
    <a href="#summary">Summary</a>
    <span> · </span>
    <a href="#pipe">Pipe</a>
    <span> · </span>
    <a href="#multipipe">Multipipe</a>
    <span> · </span>
    <a href="#sources">Sources</a>
    <span> · </span>
    <a href="#contact">Contact</a>
</h3>

# Summary

This project aims to guarantee me a quick and clear reunderstanding of the
implementation of the multi-pipe when the notion will be as rusty as a
distant memory the day I need it.

For this purpose we will **dissect** the `prg1 | prg2 | prg3` **command execution**
using a [**C multi-pipe implementation**](https://github.com/clemedon/Multipipe_tutor/tree/main/src).

*For reasons of readability the code does not include the protections.*

[**→ GitHub Page ←**](https://clemedon.github.io/Multipipe_tutor/)<br>
[**→ GitHub Repo ←**](https://github.com/clemedon/Multipipe_tutor/)

# Pipe

- A pipe is a **unidirectional** data channel that can be used for **interprocess
  communication**.

- It is made of **two descriptors**: `pipefd[0]` that refers to the **read end** of
  the pipe and `pipefd[1]` that refers to the **write end** of the pipe.

- **Data written to the write end of the pipe is buffered** by the kernel until it
  is read from the read end of the pipe.

```
Fig.0 Interprocess communication with a single pipe.

               ₍₂₎
    IN→ 1=====xxxx0
          ⁽³⁾↓
    ⁽¹⁾ 1xxxx=====0 →OUT
```


- (0) The main process (futur parent) creates a pipe and forks itself which
  duplicates its pipe's set of file descriptors `pipefd[1]` and `pipefd[0]` into
  the newly created (child) process.

- (1) The parent process closes its `pipefd[1]` to prevent its process from
  writing in the pipe.

- (2) Simultaneously, the child process closes its `pipefd[0]` to prevent its
  process from reading in the pipe.

- (3) In the end we have a parent that can read and a child that can write, both
  sharing the same pipe.  If the child write in the pipe, the data stream will
  find its way out in the read end of the parent process ⇒ interprocess
  communication.

# Multipipe

- Alternating several pipes and processes we can create an interprocess
  communication chain, **passing the output of one program as the input of
  another program** and so on.

```
Fig.1 Overall idea of following multi-pipe example.

    Stdin → PRG1  PRG2  PRG3 → Stdout  <three programs execution
               \  /  \  /              <interprocess communication
                P1    P2               <two pipes

PRG = Program (one per command)
P   = Pipe    (one pipe for two communicating programs)
```

##  Example

The following example dissects a `prg1 | prg2 | prg3` command line execution.

```bash
$ git clone git@github.com:clemedon/Multipipe_tutor.git
$ cd Multipipe_tutor/src
$ make
$ ./multipipe /bin/echo five "|" /bin/wc -c "|" /bin/cat -e
```

Don't forget to frame `|` symbol with quotes or it will be interpreted by the
shell.

Note that the **first child** doesn't need `prevpipe` because there is **no
previous pipe** to connect. Thus we must initialize `prevpipe` to any valid fd
so as not to get an error from the `close` and `dup2` calls on an invalid fd.
`prevpipe` is the variable that we use to pass the previous pipe read end to the
next program Stdin.

**Index**
* [**Instructions**](#instructions)<br>
* [**Illustrations**](#illustrations)

### Instructions

```
main()

    - initialize prevpipe to any valid file descriptor.
    - start the while loop that iterate over each of the 3 commands (PRG).


PRG1 in ft_pipe()

    - create a pipe     P1[2]       Size 2 array that contains P1[0] and P1[1]
    - fork itself                   Which clones P1

  Child

    - close              P1[0]      Unused
    - redirect Stdout to P1[1]      Fill the pipe with PRG1 output
    - close              P1[1]      Not needed anymore
    - redirect Stdin  to prevpipe   Here Stdin (cf. prevpipe init)
    - close              prevpipe   Not needed anymore
    - exec

  Parent

    - close              P1[1]      Unused
    - prevpipe         = P1[0]      Save prevpipe for PRG2 Stdin


PRG2 in ft_pipe()

    - create a pipe     P2[2]       Size 2 array that contains P2[0] and P2[1]
    - fork itself                   Which clones P2

  Child

    - close              P2[0]      Unused
    - redirect Stdout to P2[1]      Fill the pipe with PRG2 output
    - close              P2[1]      Not needed anymore
    - redirect Stdin  to prevpipe   Here P1[0] (the previous P[0])
    - close              prevpipe   Not needed anymore
    - exec

  Parent

    - close              P2[1]      Unused
    - prevpipe         = P2[0]      Save prevpipe for PRG3 Stdin


PRG3 in ft_last()

    - fork itself

  Child

    - redirect Stdin  to prevpipe   Here P2[0] (the previous P[0])
    - close              prevpipe   Not needed anymore
    - exec

  Parent

    - close              prevpipe   Unused
    - wait for children
```
[**Return to Index ↑**](#example)

### Illustrations

The exact path taken by the **stream of data** during the execution is indicated by
the **`(A)`** to **`(J)`** and  **`→`** symbols.

```
PRG1 in ft_pipe()

P1 in the child process.

                      P1[1]           P1[0]
                      ―――――――――――――――――――――
   (A) Stdin → PRG1 → OPEN → (B)     CLOSED
                      ―――――――――――――――――――――

P1 in the parent process.

                      P1[1]           P1[0]
                      ―――――――――――――――――――――
                      CLOSED     (C) → OPEN → prevpipe (D)
                      ―――――――――――――――――――――

PRG2 in ft_pipe()

P2 in the child process.

                      P2[1]           P2[0]
                      ―――――――――――――――――――――
(E) prevpipe → PRG2 → OPEN → (F)     CLOSED
                      ―――――――――――――――――――――

P2 in the parent process.

                      P2[1]           P2[0]
                      ―――――――――――――――――――――
                      CLOSED     (G) → OPEN → prevpipe (H)
                      ―――――――――――――――――――――
PRG3 in ft_last()

Last program execution.

(I) prevpipe → PRG3 → Stdout (J)
```

[**Return to Index ↑**](#example)

# Sources

- **`$ man 2 pipe`**
- [**GPT Chat**](https://chat.openai.com/chat)

# Contact

```
cvidon   42
clemedon icloud
```

<sub><i>Copyright 2022 Clément Vidon.  All Rights Reserved.</i></sub>
