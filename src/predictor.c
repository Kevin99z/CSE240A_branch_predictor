//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "predictor.h"
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define trim(x,c) ((((uint64_t)1<<(c))-1) & (x))
#define getBit(x,i)    (((x)>>(i)) & 1)
#define clearBit(x,i)  (x) &= ~(1 << (i))
#define setBit(x,i)    (x) |= (1 << (i))

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int pcIndexBase;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
uint8_t (*pred_func)(uint32_t pc);
void (*train_func)(uint32_t pc, uint8_t outcome);
struct {
  char* bht;
  uint64_t history;
} gsharePredictor;

struct {
  char* bht;
} bimodalPredictor;

struct {
  char* predCounter;
} tournamentPredictor;

struct {
  uint64_t* bht;
  char* pht;
} paPredictor;

struct {
  char* predCounter;
} customPredictor;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// functions for static predictor
uint8_t static_pred(uint32_t pc) {
  return TAKEN;
}
void static_train(uint32_t pc, uint8_t outcome) {
  return;
}

void static_init() {
  pred_func = static_pred;
  train_func = static_train;
}

void updateBit(char* x, size_t index, uint8_t val) {
    // update the indexed bit of char x to val (0 or 1)
    if(val) setBit(*x, index);
    else clearBit(*x, index);
}


// update 2-bit counter
void update_counter(char* counter, size_t index, uint8_t outcome) {
  counter += index >> 2;
  index &= 3;
  uint8_t i1=index<<1, i2=1+(index<<1);
  uint8_t lo = getBit(*counter, i1)+(getBit(*counter, i2)<<1);
  if (outcome==TAKEN && lo < ST) {
    lo += 1;
    updateBit(counter, i1, lo & 1);
    updateBit(counter, i2, lo >> 1);
  } else if (outcome==NOTTAKEN && lo > SN){
    lo -= 1;
    updateBit(counter, i1, lo & 1);
    updateBit(counter, i2, lo >> 1);
  }
}

// functions for gshare predictor
uint8_t gshare_pred(uint32_t pc) {
  // int index = trim(gsharePredictor.history^pc, ghistoryBits);
  // char* counter = gsharePredictor.bht;
  // uint8_t i1=index<<1, i2=1+(index<<1);
  // uint8_t lo = getBit(*counter, i1)+(getBit(*counter, i2)<<1);
  return gsharePredictor.bht[trim(gsharePredictor.history^pc, ghistoryBits)]>=WT;
}

void gshare_train(uint32_t pc, uint8_t outcome) {
  // update_counter(gsharePredictor.bht, trim(gsharePredictor.history^pc, ghistoryBits), outcome);
  char* pred = gsharePredictor.bht + trim(gsharePredictor.history^pc, ghistoryBits);
  gsharePredictor.history <<= 1;
  if (outcome==TAKEN) {
    gsharePredictor.history |= 1;
    if(*pred<ST)
    {
      *pred += 1;
    }
  } else if (outcome==NOTTAKEN && *pred > SN){
    *pred -= 1;
  }
}

void gshare_init(){
  size_t bht_size = 1 << ghistoryBits;
  gsharePredictor.bht = malloc(bht_size);
  memset(gsharePredictor.bht, 0, bht_size);
  gsharePredictor.history = 0;
  pred_func = gshare_pred;
  train_func = gshare_train;
}

// functions for bimodal predictor
uint8_t bimodal_pred(uint32_t pc) {
  return bimodalPredictor.bht[trim(pc, lhistoryBits)] >= WT;
}

void bimodal_train(uint32_t pc, uint8_t outcome) {
  char* pred = bimodalPredictor.bht + trim(pc, lhistoryBits);
  if (outcome==TAKEN && *pred < ST) {
    *pred += 1;
  } else if (outcome==NOTTAKEN && *pred > SN){
    *pred -= 1;
  }
}

void bimodal_init() {
  size_t bht_size = 1 << lhistoryBits;
  bimodalPredictor.bht = malloc(bht_size);
  memset(bimodalPredictor.bht, 0, bht_size);
  pred_func = bimodal_pred;
  train_func = bimodal_train;
}

// functions for tournament predictor
uint8_t tournament_pred(uint32_t pc) { 
  if (tournamentPredictor.predCounter[trim(pc, pcIndexBits)] >= 2) {
    return bimodal_pred(pc);
  } else {
    return gshare_pred(pc);
  }
}

void tournament_train(uint32_t pc, uint8_t outcome) {
    uint8_t gresult = gshare_pred(pc);
    uint8_t lresult = bimodal_pred(pc);
    gshare_train(pc, outcome);
    bimodal_train(pc, outcome);
    char* counter = tournamentPredictor.predCounter + trim(pc, pcIndexBits);
    if (gresult==outcome && lresult!=outcome && *counter > 0) {
      *counter -= 1;
    } else if (gresult!=outcome && lresult==outcome && *counter < 3) {
      *counter += 1;
    }
}

void tournament_init() {
  gshare_init();
  bimodal_init();
  tournamentPredictor.predCounter = malloc(1 << pcIndexBits);
  memset(tournamentPredictor.predCounter, 1, 1 << pcIndexBits); //initialized to Weakly select the Global Predictor.
  pred_func = tournament_pred;
  train_func = tournament_train;
}


// functions for pa predictor
uint8_t pa_pred(uint32_t pc) {
    int bht_index=trim(pc,pcIndexBits);
    int pht_index=trim(paPredictor.bht[bht_index],ghistoryBits);
    return paPredictor.pht[pht_index]>=WT;
}

void pa_train(uint32_t pc, uint8_t outcome) {
    // update_counter(gsharePredictor.bht, trim(gsharePredictor.history^pc, ghistoryBits), outcome);
  int bht_index=trim(pc,pcIndexBits);
  int pht_index=trim(paPredictor.bht[bht_index],ghistoryBits);
  char* pred = paPredictor.pht + pht_index;
  paPredictor.bht[bht_index] <<= 1;
  if (outcome==TAKEN) {
    paPredictor.bht[bht_index] |= 1;
    if(*pred<ST)
    {
      *pred += 1;
    }
  } else if (outcome==NOTTAKEN && *pred > SN){
    *pred -= 1;
  }
}

void pa_init() {
    size_t bht_size=1<<pcIndexBits;
    size_t pht_size=1<<ghistoryBits;
    paPredictor.bht = malloc(bht_size*sizeof(uint64_t));
    paPredictor.pht = malloc(pht_size);
    memset(paPredictor.bht, 0, bht_size*sizeof(uint64_t));
    memset(paPredictor.pht, 0, pht_size);
    pred_func = pa_pred;
    train_func = pa_train;
}


// functions for tournament predictor
uint8_t custom_pred(uint32_t pc) { 
  if (customPredictor.predCounter[trim(pc, pcIndexBits)] >= 2) {
    return pa_pred(pc);
  } else {
    return gshare_pred(pc);
  }
}

void custom_train(uint32_t pc, uint8_t outcome) {
    uint8_t gresult = gshare_pred(pc);
    uint8_t paresult = pa_pred(pc);
    gshare_train(pc, outcome);
    pa_train(pc, outcome);
    char* counter = customPredictor.predCounter + trim(pc, pcIndexBits);
    if (gresult==outcome && paresult!=outcome && *counter > 0) {
      *counter -= 1;
    } else if (gresult!=outcome && paresult==outcome && *counter < 3) {
      *counter += 1;
    }
}

void custom_init() {
  gshare_init();
  pa_init();
  customPredictor.predCounter = malloc(1 << pcIndexBits);
  memset(customPredictor.predCounter, 1, 1 << pcIndexBits); //initialized to Weakly select the Global Predictor.
  pred_func = custom_pred;
  train_func = custom_train;
}
// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
    switch (bpType) {
    case STATIC:
      static_init();
      break;
    case GSHARE:
      gshare_init();
      break;
    case TOURNAMENT:
      tournament_init();
      break;
    case CUSTOM:
      custom_init();
      break;
    default:
      break;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  if (pred_func != NULL)
    return pred_func(pc);

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  if (train_func != NULL)
    return train_func(pc, outcome);
}
