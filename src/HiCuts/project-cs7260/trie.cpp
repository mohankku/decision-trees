#include "stdinc.h"
#include "hicut.h"
#include "trie.h"
#include <queue>
#include "dheap.h"


trie::trie(int N1, int numrules1, int binth1, float spfac1, struct pc_rule* rule1, int opt1) {
// Initialize trie that can have up to N1 nodes.
  int i;
  N = N1;
  numrules = numrules1;
  binth = binth1;
  spfac = spfac1;
  rule = rule1;
  opt = opt1;
  //nodeSet = new nodeItem[N+1]();
  nodeSet.push_back(nodeItem());
  root = 1; 
  n = 1;
  n2 = 0;
  n3 = 0;
  n4 = 0;

  //for(i=1; i<=N; i++) nodeSet[i].child = new(nodeItem *)();

  printf( "size of tree node %d\n", sizeof(nodeItem));
  
  nodeSet.push_back(nodeItem());
  nodeSet[root].isleaf = 0;
  nodeSet[root].nrules = numrules;
  for (i=0; i<MAXDIMENSIONS; i++){
    nodeSet[root].field[i].low = 0;
    if(i<2) nodeSet[root].field[i].high = 0xffffffff;
    else if(i==4) nodeSet[root].field[i].high = 255; //This code is never run...maybe bad?  We only look at src, dest, input port, output port anyway so...
    else nodeSet[root].field[i].high = 65535; 
  }
  nodeSet[root].ruleid = new int[numrules]();
  for(i=0; i< numrules; i++) nodeSet[root].ruleid[i] = i;
  nodeSet[root].dim = 0;
  nodeSet[root].ncuts = 0;
	
  freelist = 2;	// create list of unallocated nodes
  //nodeSet[root].children = NULL;
  //for (i = 2; i < N; i++) nodeSet[i].child[0] = i+1;
  //nodeSet[N].child[0] = Null;
  
  createtrie();
	
}

trie::~trie() { }//delete [] nodeSet; }


//opt 0: min max numrules in child (can be very bad in worst case)
//opt 1: max entropy (can be very bad in worst case: wildcard)
//opt 2: min sm
//opt 3: dimension with largest distinct components
void trie::choose_np_dim(nodeItem *v, int *d, int *np){

  int nump[MAXDIMENSIONS];
  int done;
  int sm[MAXDIMENSIONS];               //space measurement
  float entropy[MAXDIMENSIONS];
  int ncomponent[MAXDIMENSIONS];
  int i,j,k;
  int lo,hi,r;
  int temp, tmpkey, minv, maxv;
  int max[MAXDIMENSIONS];
  dheap H1(MAXRULES,2);
  dheap H2(MAXRULES,2);
  int *nr[MAXDIMENSIONS];              //number of rules in each child
  int enter_loop=0;

  for(k=0; k<MAXDIMENSIONS; k++) nr[k] = NULL;

  for(k=0; k<MAXDIMENSIONS; k++){
    
    //nump[k] = max(4, (int)sqrt(v.nrules));
    if(v->field[k].high == v->field[k].low){
    	nump[k] = 1;
    }else{
      nump[k] = 2;
    }
    done = 0;
    
    while(!done){
      sm[k] = 0;
      nr[k] = (int *)realloc(nr[k], nump[k]*sizeof(int));
      for(i=0; i<nump[k]; i++) {nr[k][i]=0;}
      for(j=0; j<v->nrules; j++){
        
        r = (v->field[k].high - v->field[k].low)/nump[k];
        lo = v->field[k].low;
        hi = lo + r;
        for(i=0; i<nump[k]; i++){
          if(rule[v->ruleid[j]].field[k].low >=lo && rule[v->ruleid[j]].field[k].low <=hi ||
             rule[v->ruleid[j]].field[k].high>=lo && rule[v->ruleid[j]].field[k].high<=hi ||
             rule[v->ruleid[j]].field[k].low <=lo && rule[v->ruleid[j]].field[k].high>=hi){
            sm[k]++;
            nr[k][i]++;
          }	 
          lo = hi + 1;
          hi = lo + r;
        }
      }
      sm[k] += nump[k];	
      if(sm[k] < (int)(spfac*v->nrules) && nump[k]<= MAXCUTS/2 && nump[k] < (v->field[k].high - v->field[k].low)){
        nump[k] = nump[k]*2;			
      }else{
        done=1;
      }
    }
  }
    
  if(opt == 0){
  	
    for(i=0; i<MAXDIMENSIONS; i++) max[i]=0;
    for(k=0; k<MAXDIMENSIONS; k++){
      for(i=0;i<nump[k];i++){
        if(max[k]<nr[k][i]) max[k]=nr[k][i];
      }
    }
    minv=BIGINT;
    for(k=0; k<MAXDIMENSIONS; k++){
      if(minv > max[k] && nump[k] != 1) minv = max[k];
    }
    for(k=0; k<MAXDIMENSIONS; k++){
      if(minv == max[k] && nump[k] != 1){
    	*d = k;
    	*np = nump[k];
    	break;
      }
    }	
    
  }else if(opt == 1){
  	
    for(k=0; k<MAXDIMENSIONS; k++){
      entropy[k]=0;
      for(i=0; i<nump[k]; i++){
        entropy[k] -= (float)nr[k][i]/(float)sm[k]*log10((float)nr[k][i]/(float)sm[k]);
      }
    }
    float maxfv = 0.0;
    for(k=0;k<MAXDIMENSIONS; k++){
      if(maxfv < entropy[k] && nump[k] != 1) maxfv = entropy[k];
    }
    for(k=0; k<MAXDIMENSIONS; k++){
      if(maxfv == entropy[k] && nump[k] != 1){
        *d = k;
        *np = nump[k];
        break;
      }
    }
     	
  }else if(opt == 2){
  	
    int minv=BIGINT;
    for(k=0; k<MAXDIMENSIONS; k++){
      if(minv > sm[k] && nump[k] != 1) minv = sm[k];
    }
    for(k=0; k<MAXDIMENSIONS; k++){
      if(minv == sm[k] && nump[k] != 1){
    	*d = k;
    	*np = nump[k];
    	break;
      }
    }	
    
  }else if(opt == 3){
    	
    for(k=0; k<MAXDIMENSIONS; k++){
      ncomponent[k]=0;
      
      for(i = 0; i<v->nrules; i++){
        if(rule[v->ruleid[i]].field[k].low < v->field[k].low){
          H1.insert(v->ruleid[i], v->field[k].low); 	
        }else{
          H1.insert(v->ruleid[i], rule[v->ruleid[i]].field[k].low);
        }
      }
      
      while(H1.findmin()!= Null){
          temp = H1.findmin();
    	  tmpkey = H1.key(temp);
          enter_loop = 0;
    	  while(tmpkey == H1.key(H1.findmin())){
            enter_loop=1;
    	    j = H1.deletemin();
    	    if(rule[j].field[k].high > v->field[k].high){
    	      H2.insert(j, v->field[k].high);	
    	    }else{
    	      H2.insert(j, rule[j].field[k].high);
    	    }
    	  }
          /* Some bug, just delete the key */
          if (!enter_loop)
            j = H1.deletemin();
    	  while(H2.findmin() != Null){
    	    ncomponent[k]++;
    	    j = H2.findmin();
    	    tmpkey = H2.key(j);
          while(H2.findmin() != Null && H2.key(j) == tmpkey){
    	      H2.deletemin();
    	      j = H2.findmin();
    	    }
    	  }  		
      }
       
    }
    
    if(ncomponent[0]==1 && ncomponent[1]==1 && ncomponent[2]==1 &&
       ncomponent[3]==1 && ncomponent[4]==1 && ncomponent[5]==1){
        *d = 0;
        *np = 1;
        return;
    }
             
    int maxv = 0;
    for(k=0;k<MAXDIMENSIONS; k++){
      if(maxv < ncomponent[k] && nump[k] != 1) maxv = ncomponent[k];
    }
    
    for(k=0; k<MAXDIMENSIONS; k++){
      if(maxv == ncomponent[k] && nump[k] != 1){
        *d = k;
        *np = nump[k];
        break;
      }
    } 
    	
  }		
}

//remove redundancy by covering
void trie::remove_redundancy(nodeItem *v){
  	
  int cover;
  int tmp, tmp2;
  
  if(v->nrules == 1) return;
  
  tmp = v->nrules -1;
  tmp2 = v->nrules -2;
  while(tmp >= 1){
    for(int i = tmp2; i >= 0; i--){
      for(int k= 0; k < MAXDIMENSIONS; k++){
      	cover = 1;
        if(max(rule[v->ruleid[i]].field[k].low, v->field[k].low) > max(rule[v->ruleid[tmp]].field[k].low, v->field[k].low) ||
           min(rule[v->ruleid[i]].field[k].high,v->field[k].high)< min(rule[v->ruleid[tmp]].field[k].high,v->field[k].high)){
          cover = 0;
          break;
        }
      }
      if(cover == 1){
      	
      	for(int j = tmp; j < v->nrules-1; j++){
      	  v->ruleid[j] = v->ruleid[j+1]; 	
      	}
      	
      	v->nrules --;
      	
           	n2++;
      	break;
      }
    }
    tmp --;
    tmp2 --;
  } 	                       
   		
}

void trie::createtrie(){
	
  std::queue<int> Q;
  //list Q(MAXNODES);
  int last;
  int *d = new int;
  int *np = new int;
  int nr;
  int empty;
  int u,v;
  int r, lo, hi;
  float worstcase = 0.0;
  
  Q.push(root); last = root; pass = 0;
  
  while(!Q.empty())
  {
    v = Q.front();
	Q.pop();

	//Make sure we don't try and access something that isn't there
	while(v >= nodeSet.size())
	{
		nodeSet.push_back(nodeItem());
	}

    //printf("dequeue %d\n", v);
    //remove_redundancy(&nodeSet[v]);
    choose_np_dim(&(nodeSet[v]), d, np);
    if(nodeSet[v].nrules <= binth){
      nodeSet[v].isleaf = 1;
      n3 += nodeSet[v].nrules;
      if(worstcase < pass*NODESIZE + nodeSet[v].nrules*(RULESIZE+RULEPTSIZE)) worstcase = pass*NODESIZE + nodeSet[v].nrules*(RULESIZE+RULEPTSIZE);
    }else if(*np == 1){
      nodeSet[v].isleaf = 1;
      n3 += nodeSet[v].nrules;
      if(worstcase < pass*NODESIZE + nodeSet[v].nrules*(RULESIZE+RULEPTSIZE)) worstcase = pass*NODESIZE + nodeSet[v].nrules*(RULESIZE+RULEPTSIZE);
    }else{
      nodeSet[v].dim = *d;
      int k = *d;
      //printf("***cutting %d in dimension %d with %3d cuts in layer %2d, %4d rules (%8x:%8x)\n", v, *d, *np, pass, nodeSet[v].nrules, nodeSet[v].field[k].low, nodeSet[v].field[k].high);
      nodeSet[v].ncuts = *np;
      nodeSet[v].children.resize(nodeSet[v].ncuts); // = (nodeItem **)realloc(nodeSet[v].child, nodeSet[v].ncuts * sizeof(nodeItem *));
      r = (nodeSet[v].field[k].high - nodeSet[v].field[k].low)/nodeSet[v].ncuts;
      lo = nodeSet[v].field[k].low;
      hi = lo + r;
      for(int i = 0; i < nodeSet[v].ncuts; i++){ //This for loop iterates over the hi and low value and sets i to count the number of cuts...can be confusing, and maybe error prone 
	  empty = 1;
	  nr = 0;
        for(int j=0; j<nodeSet[v].nrules; j++){
          if(rule[nodeSet[v].ruleid[j]].field[k].low >=lo && rule[nodeSet[v].ruleid[j]].field[k].low <=hi ||
             rule[nodeSet[v].ruleid[j]].field[k].high>=lo && rule[nodeSet[v].ruleid[j]].field[k].high<=hi ||
             rule[nodeSet[v].ruleid[j]].field[k].low <=lo && rule[nodeSet[v].ruleid[j]].field[k].high>=hi){ //Not sure that this line is correct...
               empty = 0;
               nr++;
          }
        }
        if(!empty){
          n++;
		  nodeSet.push_back(nodeItem());
          nodeSet[v].children[i] = nodeSet.size() - 1;
		  u = nodeSet.size() - 1; //freelist++;
          // freelist = nodeSet[freelist].child[0];
          nodeSet[u].nrules = nr;
          //printf("creat node %d (%d child of %d) with %d rules\n", u, i, v, nr);
          if(nr <= binth){
            nodeSet[u].isleaf = 1;
            if(worstcase < pass*NODESIZE + nr*(RULESIZE+RULEPTSIZE)) worstcase = pass*NODESIZE + nr*(RULESIZE+RULEPTSIZE);
            n3 += nr;
          }else{
            nodeSet[u].isleaf = 0;
			Q.push(u);
            //Q &= u;
            //printf("enque %d\n", u);
          }
          for (int t=0; t<MAXDIMENSIONS; t++){
            if(t != k){ 
	        nodeSet[u].field[t].low = nodeSet[v].field[t].low;
	        nodeSet[u].field[t].high= nodeSet[v].field[t].high;
	      }else{
	        nodeSet[u].field[t].low = lo;
	        nodeSet[u].field[t].high= hi;
	      }
          }
          int s = 0;
          nodeSet[u].ruleid = new int[nodeSet[v].nrules]();
          for(int j=0; j<nodeSet[v].nrules; j++){
            if(rule[nodeSet[v].ruleid[j]].field[k].low >=lo && rule[nodeSet[v].ruleid[j]].field[k].low <=hi ||
               rule[nodeSet[v].ruleid[j]].field[k].high>=lo && rule[nodeSet[v].ruleid[j]].field[k].high<=hi ||
               rule[nodeSet[v].ruleid[j]].field[k].low <=lo && rule[nodeSet[v].ruleid[j]].field[k].high>=hi){
                 nodeSet[u].ruleid[s] = nodeSet[v].ruleid[j];
                 s++;
            }
          }
        }else{
          nodeSet[v].children[i] = 0;
        }  
        lo = hi + 1;
        hi = lo + r;
      }
      
      if(v == last){
		pass ++;
		last = Q.back();
      }
    }
    
  }	
  printf("worst case %f bytes/lookup\n", worstcase*4); 
}

int trie::trieLookup(long long* header){
  
  int index[MAXDIMENSIONS];
  int cdim, cchild, cover, cuts, temp_index;
  nodeItem cnode = nodeSet[root];
  //int cnode = root;
  int match = 0;
  int nbits;
  int i,k;
  
  for(i = 0; i < MAXDIMENSIONS; i++){
    if(i == 4) index[i] = 8;
    else if(i >= 2) index[i] = 16;
    else index[i] = 32;
  }
  
  while(cnode.isleaf != 1){
    n4+=NODESIZE; //NOOOOOOOO!
    nbits = 0;
    cuts = cnode.ncuts;
    while(cuts != 1){
      nbits ++;
      cuts /= 2;
    }
    cdim = cnode.dim;
	cchild = 0;

	//Does this work?
    for(i = index[cdim]; i > index[cdim] - nbits; i--){
      if((header[cdim] & 1<<(i-1)) != 0){
      	cchild += (int)pow(2, i-index[cdim]+nbits-1);
      }
    }
    
	temp_index = cnode.children[cchild]; 
    
    if(temp_index == 0) { break; } 	

    cnode = nodeSet[temp_index];
    index[cdim] -= nbits;
    
  }
  
  if(temp_index != 0){
    for(i = 0; i < cnode.nrules; i++){
      n4+=RULEPTSIZE+RULESIZE;
      cover = 1;
      for(k = 0; k < MAXDIMENSIONS; k++){
        if(rule[cnode.ruleid[i]].field[k].low > header[k] ||
           rule[cnode.ruleid[i]].field[k].high< header[k]){
          cover = 0;
          break;
        }
      }
      if(cover == 1){
        match = 1;
        break;
      }
    }
  }
  else
  {
  	//fprintf(stderr, "Error: could not find a node with the rule by lookup!\n");
  }
  
  if(match == 1){
    return cnode.ruleid[i];
  }else{
    return -1;
  }
		
}


int trie::trieSize() {
	//printf("TrieSize called!\n");
	return nodeSet.size();
}
