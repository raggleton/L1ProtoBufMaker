// -*- C++ -*-
//
// Package:    L1ProtoBufMaker
// Class:      L1ProtoBufMaker
// 
/**\class L1ProtoBufMaker L1ProtoBufMaker.cc L1Trigger/L1ProtoBufMaker/src/L1ProtoBufMaker.cc

 Description: [EDAnalyzer module to convert AOD to protocol buffers. Adaptation of UserCode/L1TriggerDPG/src/L1ExtraTreeProducer.cc. Resulting files can be used with Mark Grimes' menu studies tools.]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Robin Aggleton
//         Created:  Sat Feb  8 19:32:09 GMT 2014
// $Id$
//
//


// system include files
#include <memory>
#include <vector>
#include <stdexcept>


// framework
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

// data formats
#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1HFRingsFwd.h"
#include "DataFormats/L1Trigger/interface/L1HFRings.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"

// Had to manually copy these into my repo.
// For some reason it doesn't like it when I do
// #include "UserCode/L1TriggerDPG/interface/L1AnalysisL1Extra.h"
// #include "UserCode/L1TriggerDPG/interface/L1AnalysisL1ExtraDataFormat.h"
// scram b doesn't include the relevant libraries, even when I tell it to in the BuildFile.xml grrr
#include "L1AnalysisL1Extra.h"
#include "L1AnalysisL1ExtraDataFormat.h"

// Headers from MenuGeneration. Which one will work?
// #include "L1Trigger/MenuGeneration/interface/l1menu/ReducedSample.h"
#include "l1menu/ReducedSample.h"
#include "l1menu/tools/fileIO.h"
#include "l1menu/TriggerMenu.h"
#include "l1menu/L1TriggerDPGEvent.h"
#include "l1menu/ITrigger.h"
#include "l1menu/ICachedTrigger.h"



//
// class declaration
//

class L1ProtoBufMaker : public edm::EDAnalyzer {
  public:
    explicit L1ProtoBufMaker(const edm::ParameterSet&);
    ~L1ProtoBufMaker();

    // static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
  private:
    virtual void beginJob() ;
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;

    virtual void beginRun(edm::Run const&, edm::EventSetup const&);
    virtual void endRun(edm::Run const&, edm::EventSetup const&);
    virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
    virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

  public:
    
    L1Analysis::L1AnalysisL1Extra * l1Extra;
    L1Analysis::L1AnalysisL1ExtraDataFormat * l1ExtraData;

    // std::unique_ptr<l1menu::TriggerMenu> pMyMenu; // trigger menu, as set by user
    l1menu::ReducedSample * pReducedSample; // reduced sample for getting trigger thresholds, etc

  private:
    /////////////////////////
    // To read in AOD file //
    /////////////////////////
    unsigned maxL1Extra_;

    // EDM input tags
    edm::InputTag isoEmLabel_;
    edm::InputTag nonIsoEmLabel_;
    edm::InputTag cenJetLabel_;
    edm::InputTag fwdJetLabel_;
    edm::InputTag tauJetLabel_;
    edm::InputTag muonLabel_;
    edm::InputTag metLabel_;
    edm::InputTag mhtLabel_;
    edm::InputTag hfRingsLabel_;

    // bool doUpgrade_;
    // edm::InputTag tauLabel_;
    // edm::InputTag isoTauLabel_;

    std::string protobufFilename_;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
// here we get collections from AOD (some defaults are provided, overridden in the cfi file), and get trigger menu filename and protobuf output name
L1ProtoBufMaker::L1ProtoBufMaker(const edm::ParameterSet& iConfig):
  isoEmLabel_(iConfig.getUntrackedParameter("isoEmLabel",edm::InputTag("l1extraParticles:Isolated"))),
  nonIsoEmLabel_(iConfig.getUntrackedParameter("nonIsoEmLabel",edm::InputTag("l1extraParticles:NonIsolated"))), 
  cenJetLabel_(iConfig.getUntrackedParameter("cenJetLabel",edm::InputTag("l1extraParticles:Central"))),
  fwdJetLabel_(iConfig.getUntrackedParameter("fwdJetLabel",edm::InputTag("l1extraParticles:Forward"))),
  tauJetLabel_(iConfig.getUntrackedParameter("tauJetLabel",edm::InputTag("l1extraParticles:Tau"))),
  // tauJetLabel_(iConfig.getUntrackedParameter("isoTauJetLabel",edm::InputTag("l1extraParticles:Tau"))),
  muonLabel_(iConfig.getUntrackedParameter("muonLabel",edm::InputTag("l1extraParticles"))),
  metLabel_(iConfig.getUntrackedParameter("metLabel",edm::InputTag("l1extraParticles:MET"))),
  mhtLabel_(iConfig.getUntrackedParameter("mhtLabel",edm::InputTag("l1extraParticles:MHT"))),
  hfRingsLabel_(iConfig.getUntrackedParameter("hfRingsLabel",edm::InputTag("l1extraParticles")))
{

  // Get trigger menu file
  //? why init here and not in list above?
  try
  {
    std::string menuFilename = iConfig.getParameter<std::string>("menuFilename");
    std::cout << "Loading menu from file " << menuFilename << std::endl;
    std::unique_ptr<l1menu::TriggerMenu> pMyMenu=l1menu::tools::loadMenu( menuFilename );
    pReducedSample = new l1menu::ReducedSample( *pMyMenu );
  }
  catch( std::exception& error )
  {
    std::cerr << "Exception caught: " << error.what() << std::endl;
  }

  // Get output protobuf file name
  protobufFilename_ = iConfig.getParameter<std::string>("protobufFilename");

}


L1ProtoBufMaker::~L1ProtoBufMaker()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void L1ProtoBufMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  ////////////////////////////////////////////
  // Get L1Extra collections from the event //
  ////////////////////////////////////////////

  edm::Handle<l1extra::L1EmParticleCollection> isoEm;
  edm::Handle<l1extra::L1EmParticleCollection> nonIsoEm;
  edm::Handle<l1extra::L1JetParticleCollection> cenJet;
  edm::Handle<l1extra::L1JetParticleCollection> fwdJet;
  edm::Handle<l1extra::L1JetParticleCollection> tauJet;
  edm::Handle<l1extra::L1MuonParticleCollection> muon;
  edm::Handle<l1extra::L1EtMissParticleCollection> mets;
  edm::Handle<l1extra::L1EtMissParticleCollection> mhts;
  edm::Handle<l1extra::L1HFRingsCollection> hfRings ;

  iEvent.getByLabel(isoEmLabel_, isoEm);
  iEvent.getByLabel(nonIsoEmLabel_, nonIsoEm);
  iEvent.getByLabel(cenJetLabel_, cenJet);
  iEvent.getByLabel(fwdJetLabel_, fwdJet);
  iEvent.getByLabel(tauJetLabel_, tauJet);
  // iEvent.getByLabel(isoTauJetLabel_, isoTauJet);
  iEvent.getByLabel(muonLabel_, muon);
  iEvent.getByLabel(metLabel_, mets);
  iEvent.getByLabel(mhtLabel_, mhts);
  iEvent.getByLabel(hfRingsLabel_, hfRings);

  /////////////////////////////////////////////////
  // Create & fill L1TriggerDPGEvent             //
  //                                             //
  // nice package of info that can be passed     //
  // to the ReducedSample for further processing //
  /////////////////////////////////////////////////
  
  l1menu::L1TriggerDPGEvent event;

  // set event PU weight, run number, lumi section and event number
  event.setWeight(1);
  event.setRun(iEvent.id().run());
  event.setLS(iEvent.id().luminosityBlock());
  event.setEventNum(iEvent.id().event());

  // fill it with collection info
  if (nonIsoEm.isValid() && isoEm.isValid())
  {  
    event.setEG( nonIsoEm, isoEm);   
  } 
  else 
  {
    edm::LogWarning("MissingProduct") << "L1Extra Iso Em or Non Iso EM not found. Branch will not be filled" << std::endl;
  }
  
  if (cenJet.isValid() && fwdJet.isValid())
  {
    event.setJets( cenJet, fwdJet );
  } 
  else 
  {
    edm::LogWarning("MissingProduct") << "L1Extra cen Jet or fwd jet not found. Branch will not be filled" << std::endl;
  }
  

  // TESTING - COME BACK TO ISO/NON ISO TAUS LATER
  // if (tauJet.isValid() && isoTauJet.isValid())
  // {
  //   event.setTaus( tauJet,isoTauJet );   
  // } 
  // else {
  //   edm::LogWarning("MissingProduct") << "L1Extra Iso tau jets or Non Iso tau jets not found. Branch will not be filled" << std::endl;
  // }
  
  if (mets.isValid())
  {
    event.setMET( mets ); //ETT, ETM, PhiETM, HTT, HTM, PhiHTM
  } 
  else 
  {
    edm::LogWarning("MissingProduct") << "L1Extra mets not found. Branch will not be filled" << std::endl;
  }

  if (mhts.isValid())
  {
    event.setMHT( mhts ); //ETT, ETM, PhiETM, HTT, HTM, PhiHTM
  } 
  else 
  {
    edm::LogWarning("MissingProduct") << "L1Extra mhts not found. Branch will not be filled" << std::endl;
  }

/*  if (hfRings.isValid())
  {
    event.setHFring( hfRings ); //ETT, ETM, PhiETM, HTT, HTM, PhiHTM
  } 
  else 
  {
    edm::LogWarning("MissingProduct") << "L1Extra hfRings not found. Branch will not be filled" << std::endl;
  }
*/
  if (muon.isValid())
  {
    event.setMuons( muon );
  } 
  else 
  {
    edm::LogWarning("MissingProduct") << "L1Extra muons not found. Branch will not be filled" << std::endl;
  }
   
  //////////////////////////////////////////////////////////////////////////////
  // Pass L1TriggerDPGEvent object to ReducedSample to set trigger thresholds //
  //////////////////////////////////////////////////////////////////////////////
  pReducedSample->addEvent( event ); 

}

// ------------ method called once each job just before starting event loop  ------------
void L1ProtoBufMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void L1ProtoBufMaker::endJob() 
{

  //////////////////////////////////////////////////
  // Tell ReducedSample to output a protobuf file //
  //////////////////////////////////////////////////
  
  try
  {
    pReducedSample->saveToFile( protobufFilename_ );
    std::cout << "Saving protobuf file to " << protobufFilename_ << std::endl;
  }
  catch( std::exception& error )
  {
    std::cerr << "Exception caught: " << error.what() << std::endl;
  }
}

// ------------ method called when starting to processes a run  ------------
void 
L1ProtoBufMaker::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
L1ProtoBufMaker::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
L1ProtoBufMaker::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
L1ProtoBufMaker::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
/*void
L1ProtoBufMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}*/

//define this as a plug-in
DEFINE_FWK_MODULE(L1ProtoBufMaker);
