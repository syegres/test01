# test01
This is a simple calculator with a given precision that has either double or int64 underlying type.
Usage:
```bash
./test01 <expressions>|--integer|--floating|--precision=<precision>
```

Sample:
```bash
./test01 "1+1" 2-2 "3--3" "23+avc" 3.145678/4.556778 --integer "1.1+2.2" --precision=4 22/33 --floating "-(+1)" 223/445 --precision=3 1/3 --integer 1/3 "1+1)))*(2+2)*(3+3)" "(3)*2)" "((1" "1))"
1+1 -> 2
2-2 -> 0
!!! 3--3 -> Incorrect input at 1: --3
!!! 23+avc -> Incorrect input at 2: +avc
3.145678/4.556778 -> 0.69
*** Switched to integer calculator
1.1+2.2 -> 3.3
*** Changed precision to 4
22/33 -> 0.6667
*** Switched to floating calculator
-(+1) -> -1
223/445 -> 0.5011
*** Changed precision to 3
1/3 -> 0.333
*** Switched to integer calculator
1/3 -> 0.333
!!! 1+1)))*(2+2)*(3+3) -> Incorrect input at 14: 3+3)
!!! (3)*2) -> Incorrect input at 0: (3)*2)
!!! ((1 -> Incorrect input at 0: ((1
!!! 1)) -> Incorrect input at 1: ))
```

To run unit tests:
```bash
./test01_tests
```
