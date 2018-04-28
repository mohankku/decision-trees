Readme:
======

To build:
   - cd src/HiCuts/project-cs7260
   - make

To clean:
   - make clean

To run the code:
   - Command line options
     ./hicut -h
     hicut [-b binth][-s spfac][-o opt][-r ruleset][-t trace]

   Example runs:
           ./hicut -o 0 -r rules_50 -s 1
           ./hicut -o 0 -r rules_50 -s 1 -b 8
           ./hicut -o 2 -r rules_50 -s 1
           ./hicut -o 3 -r rules_50 -s 1
