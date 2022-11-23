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

void init_static_predictor() {
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
  gsharePredictor.history <<= 1;
  gsharePredictor.history |= 1;
  // update_counter(gsharePredictor.bht, trim(gsharePredictor.history^pc, ghistoryBits), outcome);
  char* pred = gsharePredictor.bht + trim(gsharePredictor.history^pc, ghistoryBits);
  if (outcome==TAKEN && *pred < ST) {
    *pred += 1;
  } else if (outcome==NOTTAKEN && *pred > SN){
    *pred -= 1;
  }
}

void init_gsharePredictor(){
  gsharePredictor.bht = malloc(2 << ghistoryBits);
  gsharePredictor.history = 0;
  pred_func = gshare_pred;
  train_func = gshare_train;
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
      init_static_predictor();
      break;
    case GSHARE:
      init_gsharePredictor();
      break;
    case TOURNAMENT:
      break;
    case CUSTOM:
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
