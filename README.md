<h1 align="center">
    Multipipe_tutor
</h1>

<h3 align="center">
    <a href="#summary">Summary</a>
    <span> · </span>
    <a href="#what-is-a-pipe">What is a pipe</a>
    <span> · </span>
    <a href="#now-multi-pipe">Now multi pipe</a>
    <span> · </span>
    <a href="#example">Example</a>
    <span> · </span>
    <a href="#sum-up">Sum Up</a>
    <span> · </span>
    <a href="#sources">Sources</a>
</h3>

# Summary

This project aims to guarantee me a quick and clear reunderstanding of the
implementation of the **multi-pipe** when the notion will be as rusty as a
distant memory the day I need it.

For this purpose we will **implement a multi-pipe in C** and dissect the **`echo
five | wc -c | grep a`** command execution.

[**→ GitHub Page ←**](https://clemedon.github.io/Multipipe_tutor/)<br>
[**→ GitHub Repo ←**](https://github.com/clemedon/Multipipe_tutor/)

# What is a pipe

- A pipe is a **unidirectional** data channel that can be used for **interprocess**
  communication.

- It is made of two descriptors: `pipefd[0]` that refers to the **read end** of
  the pipe and `pipefd[1]` that refers to the **write end** of the pipe.

- **Data written to the write end of the pipe is buffered** by the kernel until it
  is read from the read end of the pipe.

# Now multi-pipe

- The following example creates a pipe and fork to create a child process; the
  child inherits a **duplicate set of file descriptors that refer to the same
  pipe**.

- After the fork each process closes the file descriptors that it doesn't need
  for the pipe to have only **two open ends**, like a normal pipe but with its
  ends in **two differents processes**.

```
Fig.0 Interprocess communication with a single pipe.
                   ₍₂₎
    IN→ 1=====xxxx0
          ⁽³⁾↓
    ⁽¹⁾ 1xxxx=====0 →OUT

(0) The main process (futur parent) creates a pipe and forks itself which
duplicates its `pipefd[1]` and `pipefd[0]` into the newly created (child)
process.

(1) The parent process closes its pipefd[1] to prevent its process from writing
in the pipe.

(2) Simultaneously the child process closes its pipefd[0] to prevent its process
from reading in the pipe.

(3) In the end we have a parent that can read and a child that can write, both
sharing the same pipe.  If the child write in the pipe, the data stream will
find its way out in the read end of the parent process ⇒ interprocess
communication.
```

- Alternating several pipes and processes we can thus create an interprocess
  communication chain, **passing the output of one program as the input of
  another program** and so on.


```
Fig.1 Overall idea of following multi-pipe example.

   Stdin → PRG1  PRG2  PRG3 → Stdout  <three programs
              \  /  \  /              <interprocess communication
               P1    P2               <two pipes
```

# Example

[**PROGRAM 1**](#program-1) see `multipipe.c: ft_pipe()`<br>
[**PROGRAM 2**](#program-2) see `multipipe.c: ft_pipe()`<br>
[**PROGRAM 3**](#program-3) see `multipipe.c: ft_last()`<br>

[**→ Example C Source Code**](https://github.com/clemedon/Multipipe_tutor/tree/main/src)<br>
*For reasons of readability the code does not include any protection.*

## MAIN

```
 - initialize prevpipe to any valid file descriptor.
```

`prevpipe` is the variable used to **pass the previous pipe read end to the next
program Stdin**.

The **first child** doesn't need `prevpipe` because there is **no previous
pipe** to connect to yet. Thus we must initialize `prevpipe` to any valid fd so
as not to get an error from the `close` and `dup2` calls on on this fd.

### PROGRAM 1

```
 - create a pipe     P1[2]      Size 2 array that contains P1[0] and P1[1]
 - create a child               Which duplicate P1
```

**[Child]**

```
 - close              P1[0]     Unused.
 - redirect Stdin  to prevpipe  Here Stdin (cf. prevpipe init).
 - close              prevpipe  Not needed anymore.
 - redirect Stdout to P1[1]     Fill the pipe with PRG1 output.
 - close              P1[1]     Not needed anymore.
 - exec


Fig.2 Pipe1 in the child process.


                      P1[1]              P1[0]
                      ――――――――――――――――――――――――
  (A) Stdin → PRG1 →  OPEN → (B)        CLOSED
                      ――――――――――――――――――――――――

The (A) to (J) symbols indicate the path taken by the stream of data.
```

No need to mention that **`Stdin` and `Stdout`** are the file descriptors where
`PRG` reads its input and writes its output.

The child pipe `Fig.2` is a duplicate of the parent pipe `Fig.3`.  To repeat
myself, since two processes share the same pipe they can communicate through it
by writing to one end of the pipe in one process and listening to the other end
of this same pipe in the other process.

**[Parent]**

```
 - close              P1[1]     Unused
 - prevpipe         = P1[0]     Save prevpipe for PRG2 Stdin.


Fig.3 Pipe1 in the parent process.


                      P1[1]              P1[0]
                      ――――――――――――――――――――――――
                      CLOSED        (C) → OPEN → prevpipe (D)
                      ――――――――――――――――――――――――
```

### PROGRAM 2

```
 - create a pipe     P2[2]      Size 2 array that contains P2[0] and P2[1]
 - create a child               Which duplicate P2
```

**[Child]**

```
 - close              P2[0]     Unused.
 - redirect Stdin  to prevpipe  Here P1[0] (the previous P[0]).
 - close              prevpipe  Not needed anymore.
 - redirect Stdout to P2[1]     Fill the pipe with PRG2 output.
 - close              P2[1]     Not needed anymore.
 - exec


Fig.4 Pipe2 in the child process.


                      P2[1]              P2[0]
                      ――――――――――――――――――――――――
(E) prevpipe → PRG2 → OPEN → (F)        CLOSED
                      ――――――――――――――――――――――――
```

**[Parent]**

```
 - close              P2[1]     Unused
 - prevpipe         = P2[0]     Save prevpipe for PRG3 Stdin.


Fig.5 Pipe2 in the parent process.


                      P2[1]              P2[0]
                      ――――――――――――――――――――――――
                      CLOSED        (G) → OPEN → prevpipe (H)
                      ――――――――――――――――――――――――
```

### PROGRAM 3

```
 - create a child
```

**[Child]**

```
 - redirect Stdin  to prevpipe  Here P2[0] (the previous P[0]).
 - close              prevpipe  Not needed anymore
 - exec


Fig.6 Last program execution.


(I) prevpipe → PRG3 → Stdout (J)
```

**[Parent]**

```
 - close              prevpipe  Unused
 - wait for children
```

## Sum Up

The **`(A)` to `(J)` symbols** indicate the path taken by the stream of data
throughout the execution:

```

Program 1 Child


                      P1[1]              P1[0]
                      ――――――――――――――――――――――――
  (A) Stdin → PRG1 →  OPEN → (B)        CLOSED
                      ――――――――――――――――――――――――


Program 1 Parent


                      P1[1]              P1[0]
                      ――――――――――――――――――――――――
                      CLOSED        (C) → OPEN → prevpipe (D)
                      ――――――――――――――――――――――――


Program 2 Child


                      P2[1]              P2[0]
                      ――――――――――――――――――――――――
(E) prevpipe → PRG2 → OPEN → (F)        CLOSED
                      ――――――――――――――――――――――――


Program 2 Parent


                      P2[1]              P2[0]
                      ――――――――――――――――――――――――
                      CLOSED        (G) → OPEN → prevpipe (H)
                      ――――――――――――――――――――――――


Program 3 Child


(I) prevpipe → PRG3 → Stdout (J)
```

# Sources

- **`$ man 2 pipe`**
- [**GPT Chat**](https://chat.openai.com/chat)
