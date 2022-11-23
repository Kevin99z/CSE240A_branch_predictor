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
  char* history;
} gsharePredictor;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// functions for static predictor
uint8_t static_pred() {
  return TAKEN;
}
void static_train(uint32_t pc, uint8_t outcome) {
  return;
}

void init_static_predictor() {
  pred_func = static_pred;
  train_func = static_train;
}

// functions for gshare predictor
uint8_t gshare_pred() {
  return NOTTAKEN;
}

void init_gsharePredictor(){
    gsharePredictor.bht = malloc(2 << ghistoryBits);
    memset(gsharePredictor.bht, 0, 2 << ghistoryBits);

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
