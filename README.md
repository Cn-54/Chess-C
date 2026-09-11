# Chess-C

A simple chess engine written in C, featuring **alpha-beta pruning** and **quiescence search**.

The engine communicates through the **UCI protocol**, allowing it to be used with compatible chess interfaces.

Developed and tested using **Cute Chess** as the interface of choice.

## Features

* Legal chess move generation *(en passant and castling not implemented yet)*
* Alpha-beta pruning
* Quiescence search
* Piece-square table evaluation
* UCI protocol support
* Move making and undo functionality
* Basic check and checkmate detection

## Project Structure

```text
Chess-C/
├── bin/
├── build/
├── include/
│   ├── engine.h
│   ├── game.h
│   └── uci.h
├── src/
│   ├── engine.c
│   ├── game.c
│   ├── main.c
│   └── uci.c
├── compile_flags.txt
├── .gitignore
└── makefile
```
the makefile can produce executuables for both windows and linux

### Source Files

* `engine.c` — search and board evaluation
* `game.c` — board state, move generation, and chess rules
* `uci.c` — UCI protocol handling
* `main.c` — engine entry point


