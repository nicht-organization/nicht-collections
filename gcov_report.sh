#/bin/bash
for f in *.gcov;
    do
    echo "=== Uncovered in $f ===";
    grep -n "#####:" "$f" || echo "  (100% Executed)";
    echo "";
    done