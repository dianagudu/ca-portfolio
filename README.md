# ca-portfolio

A portfolio of heuristic algorithms for Combinatorial Auctions

## Prerequisites

* [Boost](https://www.boost.org/)
* [CPLEX](https://www.ibm.com/analytics/cplex-optimizer)
* [yaml-cpp](https://github.com/jbeder/yaml-cpp)

Free academic licenses for IBM's ILOG CPLEX software are [available](https://www.ibm.com/developerworks/community/blogs/jfp/entry/cplex_studio_in_ibm_academic_initiative?lang=en).

Include and library paths for `CPLEX` and ``boost`` can be changed in the ``makefile``.

On Debian stretch:

    $ sudo apt-get install libboost-all-dev libyaml-cpp0.5v5 libyaml-cpp-dev

## Running the code

Compile the source code:

    $ make

Compile the source code with the CPLEX library:

    $ make CPLEX=true

Run unit tests:

    $ make test

Run unit tests, including the CPLEX library:

    $ make test CPLEX=true

Run the program:

    Usage: ./bin/main [-m MODE] [-o OUTFILE] [-i] INFILE(s)
       or: ./bin/main [-a ALGO] [-o OUTFILE] [-i] INFILE(s)
    
    Run algorithm portfolio on auction instance(s) stored in INFILE(s).
    By default, the portfolio is run in HEURISTICS mode, and stats are
    printed to standard out.
    
    Allowed options:
      --help                           show this help message
      -m [ --mode ] MODE (=HEURISTICS) run portfolio in given mode
      -a [ --algo ] ALGO               run only specified algorithm
      -o [ --out ] OUTFILE             output file to store runtime stats
      -i [ --in ] INFILE(s)            input files, one per auction instance
    
    
    Valid MODE values are:
    	ALL       : run all algorithms
    	HEURISTICS: run all heuristic algorithms (exclude CPLEX and RLPS from all)
    	SAMPLES   : run all heuristic algorithms on instance and samples
    	RANDOM    : run all stochastic algorithms multiple times
    
    Valid ALGO values are:
    	GREEDY1   : greedy algorihm
    	GREEDY2   : greedy algorihm with scarcity-based relevance factors
    	GREEDY3   : greedy algorihm with relative scarcity-based relevance factors
    	GREEDY1S  : greedy algorihm with focus on sellers
    	HILL1     : hill climbing algorihm
    	HILL1S    : hill climbing algorihm with focus on sellers
    	HILL2     : hill climbing algorihm
    	HILL2S    : hill climbing algorihm with focus on sellers
    	SA        : simulated annealing algorithm
    	SAS       : simulated annealing algorithm with focus on sellers
    	CASANOVA  : Casanova algorithm (stochastic local search)
    	CASANOVAS : Casanova algorithm (stochastic local search) with focus on sellers
    	CPLEX     : optimal algorithm using CPLEX library to solve MILP
    	RLPS      : heuristic based on relaxed linear program (requires CPLEX library)
