#include <stdio.h>
#include <assert.h>

#include "rob.h"


extern int32_t NUM_ROB_ENTRIES;

/////////////////////////////////////////////////////////////
// Init function initializes the ROB
/////////////////////////////////////////////////////////////

ROB* ROB_init(void){
  int ii;
  ROB *t = (ROB *) calloc (1, sizeof (ROB));
  for(ii=0; ii<MAX_ROB_ENTRIES; ii++){
    t->ROB_Entries[ii].valid=false;
    t->ROB_Entries[ii].ready=false;
    t->ROB_Entries[ii].exec=false;
  }
  t->head_ptr=0;
  t->tail_ptr=0;
  return t;
}

/////////////////////////////////////////////////////////////
// Print State
/////////////////////////////////////////////////////////////
void ROB_print_state(ROB *t){
 int ii = 0;
  printf("Printing ROB \n");
  printf("Entry  Inst   Valid   ready  exec\n");
  for(ii = 0; ii < 32; ii++) {
    printf("%5d ::  %d\t", ii, (int)t->ROB_Entries[ii].inst.inst_num);
    printf(" %5d\t", t->ROB_Entries[ii].valid);
    printf(" %5d\t", t->ROB_Entries[ii].ready);
    printf(" %5d\n", t->ROB_Entries[ii].exec);
  }
  printf("\n");
}

/////////////////////////////////////////////////////////////
// If there is space in ROB return true, else false
/////////////////////////////////////////////////////////////

bool ROB_check_space(ROB *t){
    int ii;
    for (ii=0;ii<NUM_ROB_ENTRIES;ii++){
        if (!t->ROB_Entries[ii].valid)
            return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////
// insert entry at tail, increment tail (do check_space first)
/////////////////////////////////////////////////////////////

int ROB_insert(ROB *t, Inst_Info inst){
    int prf_id,ii;
    if (ROB_check_space(t)){

        for(ii=0;ii<NUM_ROB_ENTRIES;ii++){
         if(!t->ROB_Entries[ii].valid){
            t->tail_ptr=ii;
            break;
          }
	}
        t->ROB_Entries[t->tail_ptr].valid=true;
        t->ROB_Entries[t->tail_ptr].ready=false;
        t->ROB_Entries[t->tail_ptr].exec=false;
        t->ROB_Entries[t->tail_ptr].inst=inst;
        prf_id=t->tail_ptr;
	
        for(ii=0;ii<NUM_ROB_ENTRIES;ii++){
	 if(!t->ROB_Entries[ii].valid){
	    t->tail_ptr=ii;
            break;
	  }
	}

/*
        t->tail_ptr++;
        if (t->tail_ptr == NUM_ROB_ENTRIES)
            t->tail_ptr=0;
*/
        return prf_id;
    }
    return -1;

}

/////////////////////////////////////////////////////////////
// When an inst gets scheduled for execution, mark exec
/////////////////////////////////////////////////////////////

void ROB_mark_exec(ROB *t, Inst_Info inst){
    int ii;
    for (ii=0;ii<NUM_ROB_ENTRIES;ii++){
        if (t->ROB_Entries[ii].inst.inst_num==inst.inst_num && t->ROB_Entries[ii].valid)
            break;
    }
    t->ROB_Entries[ii].exec=true;

}


/////////////////////////////////////////////////////////////
// Once an instruction finishes execution, mark rob entry as done
/////////////////////////////////////////////////////////////

void ROB_mark_ready(ROB *t, Inst_Info inst){
    int ii;
    for (ii=0;ii<NUM_ROB_ENTRIES;ii++){
        if (t->ROB_Entries[ii].inst.inst_num==inst.inst_num && t->ROB_Entries[ii].valid )
            break;
    }
    t->ROB_Entries[ii].ready=true;
   // t->ROB_Entries[ii].valid=true;

}

/////////////////////////////////////////////////////////////
// Find whether the prf (rob entry) is ready
/////////////////////////////////////////////////////////////

bool ROB_check_ready(ROB *t, int tag){
    if (t->ROB_Entries[tag].ready && t->ROB_Entries[tag].valid)
        return true;
    else
        return false;

}


/////////////////////////////////////////////////////////////
// Check if the oldest ROB entry is ready for commit
/////////////////////////////////////////////////////////////

bool ROB_check_head(ROB *t){
    if (t->ROB_Entries[t->head_ptr].ready && t->ROB_Entries[t->head_ptr].valid)
        return true;
    else
        return false;

}

/////////////////////////////////////////////////////////////
// For writeback of freshly ready tags, wakeup waiting inst
/////////////////////////////////////////////////////////////

void  ROB_wakeup(ROB *t, int tag){
    int ii;
    for (ii=0;ii< NUM_ROB_ENTRIES; ii++){
        if (!t->ROB_Entries[ii].inst.src1_ready && (tag==t->ROB_Entries[ii].inst.src1_tag)){
            t->ROB_Entries[ii].inst.src1_tag=-1;
            t->ROB_Entries[ii].inst.src1_ready=true;
        }
        if (!t->ROB_Entries[ii].inst.src2_ready && (tag==t->ROB_Entries[ii].inst.src2_tag)){
            t->ROB_Entries[ii].inst.src2_tag=-1;
            t->ROB_Entries[ii].inst.src2_ready=true;
        }
    }
}


/////////////////////////////////////////////////////////////
// Remove oldest entry from ROB (after ROB_check_head)
/////////////////////////////////////////////////////////////

Inst_Info ROB_remove_head(ROB *t){
    int ii,flag=1;
    uint64_t z;
    t->ROB_Entries[t->head_ptr].valid=0;
    Inst_Info commited = t->ROB_Entries[t->head_ptr].inst;
    for(ii=0;ii<NUM_ROB_ENTRIES-1;ii++){
	if(t->ROB_Entries[ii].valid){
	  z=t->ROB_Entries[ii].inst.inst_num;
          t->head_ptr=ii;
	  flag=0;
	  break;
	}

    }
  /*  if(flag){
       t->head_ptr++;
    }
    else{
  */  
     for(ii=0;ii<NUM_ROB_ENTRIES;ii++){
         if(t->ROB_Entries[ii].inst.inst_num < z && t->ROB_Entries[ii].valid){
            z=t->ROB_Entries[ii].inst.inst_num;
	    t->head_ptr=ii;
         }
     
     }

if(!t->ROB_Entries[t->head_ptr].valid){
     for(ii=0;ii<NUM_ROB_ENTRIES;ii++){
         if(t->ROB_Entries[ii].valid){
            t->head_ptr=ii;
         }

     }

}

//    }

/*
    t->head_ptr++;
    if (t->head_ptr == NUM_ROB_ENTRIES)
        t->head_ptr=0;
*/
    return commited;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
