SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

g++ -std=c++11 "$SCRIPT_DIR"/../src/RTest.cpp -o "$SCRIPT_DIR"/../rtest.out
g++ -std=c++11 "$SCRIPT_DIR"/../src/ITest.cpp -o "$SCRIPT_DIR"/../itest.out
g++ -std=c++11 "$SCRIPT_DIR"/../src/bITest.cpp -o "$SCRIPT_DIR"/../bitest.out
