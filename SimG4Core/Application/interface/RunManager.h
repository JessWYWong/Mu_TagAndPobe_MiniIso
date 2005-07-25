#ifndef SimG4Core_RunManager_H
#define SimG4Core_RunManager_H

#include "FWCore/EDProduct/interface/EDProduct.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimG4Core/SensitiveDetector/interface/AttachSD.h"
#include "SimG4Core/SensitiveDetector/interface/SensitiveDetector.h"
#include "SimG4Core/SensitiveDetector/interface/SensitiveTkDetector.h"
#include "SimG4Core/SensitiveDetector/interface/SensitiveCaloDetector.h"

#include "SealKernel/Context.h"

#include <memory>



class PrimaryTransformer;
class Generator;
class DummyPhysics;
class G4SimEvent;

class DDDWorld;

class G4RunManagerKernel;
class G4Run;
class G4Event;
class G4UserRunAction;

class RunManager
{
public:
    static RunManager * instance();
    static RunManager * init(edm::ParameterSet const & p); 
    virtual ~RunManager();
    void initG4(const edm::EventSetup & es);
    void initializeUserActions();
    void initializeRun();
    void terminateRun();
    void abortRun(bool softAbort=false);
    const G4Run * currentRun() const { return m_currentRun; }
    void produce(const edm::EventSetup & es);
    void abortEvent();
    const Generator * generator() const { return m_generator; }
    const G4Event * currentEvent() const { return m_currentEvent; }
    G4SimEvent * simEvent() { return m_simEvent; }
    void dispatch(DDDWorld * world);
    seal::Handle<seal::Context> runContext() { return m_context; }

    std::vector<SensitiveTkDetector*>& sensTkDetectors(){return sensTkDets;}
    std::vector<SensitiveCaloDetector*>& sensCaloDetectors(){return sensCaloDets;}

protected:
    G4Event * generateEvent(int evt);
private:
    static RunManager * me;
    explicit RunManager(edm::ParameterSet const & p);
    seal::Handle<seal::Context> m_context;
    G4RunManagerKernel * m_kernel;
    Generator * m_generator;
    DummyPhysics * m_physics;
    PrimaryTransformer * m_primaryTransformer;
    bool m_managerInitialized;
    bool m_geometryInitialized;
    bool m_physicsInitialized;
    bool m_runInitialized;
    bool m_runTerminated;
    bool m_runAborted;
    G4Run * m_currentRun;
    G4Event * m_currentEvent;
    G4SimEvent * m_simEvent;
    G4UserRunAction * m_userRunAction;
    int m_EvtMgrVerbosity;
    bool m_Override;
    int m_RunNumber;
    edm::ParameterSet m_pGeometry;
    edm::ParameterSet m_pGenerator;   
    edm::ParameterSet m_pPhysics; 
    edm::ParameterSet m_pRunAction;      
    edm::ParameterSet m_pEventAction;
    edm::ParameterSet m_pTrackingAction;
    edm::ParameterSet m_pSteppingAction;

    AttachSD* attach_;
    std::vector<SensitiveTkDetector*> sensTkDets;
    std::vector<SensitiveCaloDetector*> sensCaloDets;

};

#endif
