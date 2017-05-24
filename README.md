# medici
medici - mdd for combinatorial testing

### Compatibility
Tested on Windows with Cygwin and Linux (Ubuntu)

### installation
* place the source code of [MEDDLY](http://meddly.sourceforge.net/obtain.html) in folder medici/meddly_svn and perform the required steps to compile and install it. Make sure to have, at the end, the file meddly_svn/lib/libmeddly.a.
* You need gmp (for meddly) and boost (program_options)
* run `runcmake.txt`, the executable file is produced under build directory.

### Running MEDICI
Supposing on Windows, from Cygwin command line:
Example: `medici.exe --m ../examples/apache.medici`
Help description: `medici.exe --help`, which produces the following quick manual as output:
```Allowed options:
  --help                produce help message
  --casa                set input in casa format
  --m arg               set model file name
  --o arg               set output file name
  --c arg               set constraints file name, only for casa mode
  --actsconv            enable acts model conversion mode
  --mname arg           set model name for acts mode
  --validate            validating model mode
  --autovalidate        autovalidate model
  --silent              be silent
  --donotgenerate       build mdds but do not generate test suite
  --mintries arg        set mintries [1]
  --tries arg           set max tries [1]
  --bettertries arg     set number of better tries [1]
  --noqueuecheck        disable check the queue for early termination
  --nit1 arg            set iterations over threshold [1]
  --nit2 arg            set iterations under threshold [2]
  --threshold arg       set threshold [2000]
  --wexp arg            set weight exponent over threshold [1]
  --wexp2 arg           set weight exponent under threshold [1.5]
```

### File input format
It accepts CASA input format to describe a combinatorial model. It also accepts constraints in free form (not necessarily CNF) expressed in Polish Inverse Notation.

* Example input file in CASA format:
```
2
4
6 4 3 3
```
The input of the model without constraints accepts at the first line the parameter t for the t-wise coverage criterium. In the second line it is defined the number of parameters P of the model, that are then singularly defined in the following line, with the number of values they can assume (their cardinality).
The tool then generates internally the list T of the tuples to cover.
To define the values of the single parameters in the constraints and in the output file, it is used the same syntax of CASA, so 0 is assigned to the value of the first parameter, and so on following the order in which the parameters are inserted in the model.

* Example input file in medici format:
```
2
4
6 4 3 3
4
0 6 * −
2 3 4 5 + + + 6 * −
2 4 + 9 * −
3 5 + 13 14 + * −
```
The first three lines of the input file are the same of the CASA format. The following lines expresses the constraints on the model. The first line of them contains the number C of constraints. The following C lines contain the constraints, in Polish Reverse Notation using the operators: 

Operator Symbol | Meaning
--------------- | -------
\* | AND
\+ | OR
\- | NOT

In the example above, there are 4 constraints. The first one, `0 6 * −` is equivalent to `NOT (0 AND 6)`, which means `NOT (Parameter1.Value1 AND Parameter2.Value1)`.

### Output
Example of output:
```
CARD: 1 6 tuples weight: 1103.61 [ 1 0 2 2 --> T]
CARD: 1 6 tuples weight: 857.797 [ 0 0 1 0 --> T]
CARD: 1 4 tuples weight: 515.296 [ 0 0 2 1 --> T]
CARD: 1 6 tuples weight: 579.534 [ 0 2 0 2 --> T]
CARD: 1 5 tuples weight: 369.288 [ 1 2 1 0 --> T]
CARD: 1 6 tuples weight: 288.588 [ 1 1 0 1 --> T]
CARD: 1 4 tuples weight: 113.824 [ 2 0 2 0 --> T]
CARD: 1 5 tuples weight: 102.471 [ 2 2 1 1 --> T]
CARD: 1 5 tuples weight: 40.3765 [ 2 1 1 2 --> T]
CARD: 1 3 tuples weight: 9.02458 [ 0 1 0 0 --> T]
CARD: 1 2 tuples weight: 2 [ 2 0 0 0 --> T]
Tuple coperte: 52 Tuple da coprire: 0 Tuple non copribili: 2
Ottenuti: 11 mdd
Tempo per ordinamenti: 0ms
time execution script 15ms
```

### Examples
In folder examples are present several examples.

## Paper
If you use this tool for academic research, please cite:
[*Efficient Combinatorial Test Generation based on Multivalued Decision Diagrams*](http://cs.unibg.it/gargantini/research/abstracts/hvc14.html)
(Gargantini, Angelo, and Vavassori, Paolo) in Hardware and Software: Verification and Testing, Haifa Verification Conference HVC 2014 (Eds. Yahav, Eran) Springer International Publishing, Lecture Notes in Computer Science, vol. 8855 (2014): 220-235 ISBN 978-3-319-13337-9
```
@Inbook{Gargantini2014,
  author="Gargantini, Angelo and Vavassori, Paolo",
  editor="Yahav, Eran",
  title="Efficient Combinatorial Test Generation Based on Multivalued Decision Diagrams",
  bookTitle="Hardware and Software: Verification and Testing: 10th International Haifa Verification Conference, HVC 2014, Haifa, Israel, November 18-20, 2014. Proceedings",
  year="2014",
  publisher="Springer International Publishing",
  address="Cham",
  pages="220--235",
  isbn="978-3-319-13338-6",
  doi="10.1007/978-3-319-13338-6_17",
  url="http://dx.doi.org/10.1007/978-3-319-13338-6_17"
}
```
