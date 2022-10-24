#!/bin/zsh

EXEC="build/dxdt"
OUT="output/dxdt"

for i in {10..100}; {
    echo -e "$i\n" | "$EXEC" 2>> "$OUT"
}
