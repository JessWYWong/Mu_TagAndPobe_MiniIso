#include "TrackingTools/GsfTools/interface/MultiGaussianStateAssembler.h"

#include "TrackingTools/GsfTools/src/GaussianStateLessWeight.h"
#include "FWCore/Utilities/interface/Exception.h"

MultiGaussianStateAssembler::MultiGaussianStateAssembler
		(const RCMultiGaussianState & state) :
  theInitialState(state), minFractionalWeight(1.e-16),
  combinationDone(false), theValidWeightSum(0.)
{
  //
  // parameters (could be configurable)
  //
//   static SimpleConfigurable<bool> sortStatesConf(false,"MultiGaussianStateAssembler:sortStates");
//   sortStates = sortStatesConf.value();
//   static SimpleConfigurable<double> minWeightConf(1.e-16,"MultiGaussianStateAssembler:minFractionalWeight");
//   minFractionalWeight = minWeightConf.value();
//   //
//   // Timers
//   //
//   if ( theTimerAdd==0 ) {
//     theTimerAdd =
//       &(*TimingReport::current())[string("MultiGaussianStateAssembler::addState")];
//     theTimerAdd->switchCPU(false);
//     theTimerComb =
//       &(*TimingReport::current())[string("MultiGaussianStateAssembler::combinedState")];
//     theTimerComb->switchCPU(false);
//   }
}

void MultiGaussianStateAssembler::addState (const RCSingleGaussianState& sgs)
{
  //
  // refuse to add states after combination has been done
  //
  if ( combinationDone )
    throw cms::Exception("LogicError")
      << "MultiGaussianStateAssembler: trying to add states after combination";

  theValidWeightSum += sgs->weight();

  theStates.push_back(sgs);
}


void MultiGaussianStateAssembler::addState (const RCMultiGaussianState& mgs) {
//   // Timer
//   TimeMe t(*theTimerAdd,false);
  //
  // refuse to add states after combination has been done
  //
  if ( combinationDone )
    throw cms::Exception("LogicError") 
      << "MultiGaussianStateAssembler: trying to add states after combination";
  //
  // Verify validity of state to be added
  //ThS: Is not possible for gaussian states.
//   if ( !tsos.isValid() )
//     throw cms::Exception("LogicError") 
//       << "MultiGaussianStateAssembler: trying to add invalid state";
  //
  // Add components (i.e. state to be added can be single or multi state)
  //
  SGSVector components(mgs->components());
  addStateVector(components);
}

void MultiGaussianStateAssembler::addStateVector (const SGSVector& states)
{
  //
  // refuse to add states after combination has been done
  //
  if ( combinationDone )
    throw cms::Exception("LogicError") 
      << "MultiGaussianStateAssembler: trying to add states after combination";
  //
  // sum up weights (all components are supposed to be valid!!!)
  //
  double sum(0.);
  for ( SGSVector::const_iterator i=states.begin();
	i!=states.end(); i++ ) {
  //ThS: Is not possible for gaussian states:
//     if ( !(i->isValid()) )
//       throw cms::Exception("LogicError") 
//         << "MultiGaussianStateAssembler: trying to add invalid state";
    sum += (**i).weight();
  }
  theValidWeightSum += sum;
  //
  // add to vector of states
  //
  theStates.insert(theStates.end(),states.begin(),states.end());
}


RCMultiGaussianState MultiGaussianStateAssembler::combinedState () {
//   // Timer
//   TimeMe t(*theTimerComb,false);
  //
  // Prepare resulting state vector
  prepareCombinedState();
  // ThS: What to return here? I choose a RCMP with empty state vector...
//  if ( !prepareCombinedState() )  return TSOS();
  //
  // Return new multi state without reweighting
  //
  return theInitialState->createNewState(theStates);
}

RCMultiGaussianState
MultiGaussianStateAssembler::combinedState (const float newWeight) {
//   // Timer
//   TimeMe t(*theTimerComb,false);
  //
  // Prepare resulting state vector
  //
  prepareCombinedState();
  // ThS: What to return here? I choose a RCMP with empty state vector...
//  if ( !prepareCombinedState() )  return TSOS();
  //
  // return reweighted state
  //
  return reweightedCombinedState(newWeight);
}

bool
MultiGaussianStateAssembler::prepareCombinedState () {
  //
  // remaining part to be done only once
  //
  if ( combinationDone )  return true;
  else  combinationDone = true;
  //
  // Remove states with negligible weights
  //
  removeSmallWeights();
  if ( theStates.empty() )  return false;
  //
  // Sort output by weights?
  //
  if ( sortStates )
    sort(theStates.begin(),theStates.end(),GaussianStateLessWeight());

  return true;
}

RCMultiGaussianState
MultiGaussianStateAssembler::reweightedCombinedState (const double newWeight) const {
  //
  // scaling factor
  //
  double factor = theValidWeightSum>0. ? newWeight/theValidWeightSum : 1;
  //
  // create new vector of states & combined state
  //
  SGSVector reweightedStates;
  reweightedStates.reserve(theStates.size());
  for ( SGSVector::const_iterator i=theStates.begin();
	i!=theStates.end(); i++ ) {
    double oldWeight = (**i).weight();
    reweightedStates.push_back( (**i).createNewState ((**i).mean(),
    	(**i).covariance(), factor*oldWeight) );
  }

  return theInitialState->createNewState(reweightedStates);
}

void
MultiGaussianStateAssembler::removeSmallWeights()
{
  //
  // check total weight
  //
  if ( theValidWeightSum == 0. ) {
    theStates.clear();
    return;
  }
  //
  // Loop until no more states are removed
  //
  bool redo;
  do {
    redo = false;
    for ( SGSVector::iterator i=theStates.begin();
	  i!=theStates.end(); i++ ) {
      if ( (**i).weight()/theValidWeightSum < minFractionalWeight ) {
	theStates.erase(i);
	redo = true;
	break;
      }
    }
  } while (redo);
}

// TimingReport::Item * MultiGaussianStateAssembler::theTimerAdd(0);
// TimingReport::Item * MultiGaussianStateAssembler::theTimerComb(0);
