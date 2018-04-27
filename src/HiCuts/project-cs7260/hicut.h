#define MAXNODES 100000000
#define MAXRULES 10000
#define MAXBUCKETS 40 
#define MAXCUTS 64
#define MAXDIMENSIONS 4
#define RULESIZE 4.5 
#define NODESIZE 4 
#define RULEPTSIZE 0.5 

struct range{
  unsigned low;
  unsigned high;
};

struct pc_rule{
  struct range field[MAXDIMENSIONS];
};

