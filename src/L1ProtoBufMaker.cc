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
#include <iostream>
#include <fstream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

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

#include "UserCode/L1TriggerDPG/interface/L1AnalysisL1Extra.h"

//
// class declaration
//

class L1ProtoBufMaker : public edm::EDAnalyzer {
  public:
    explicit L1ProtoBufMaker(const edm::ParameterSet&);
    ~L1ProtoBufMaker();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
  private:
    virtual void beginJob() ;
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;

    virtual void beginRun(edm::Run const&, edm::EventSetup const&);
    virtual void endRun(edm::Run const&, edm::EventSetup const&);
    virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
    virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

    // ----------member data ---------------------------
    L1Analysis::L1AnalysisL1Extra* l1Extra_;
    L1Analysis::L1AnalysisL1ExtraDataFormat * l1ExtraData_;

    unsigned maxL1Extra_;

    // EDM input tags
    edm::InputTag nonIsoEmLabel_;
    edm::InputTag isoEmLabel_;
    edm::InputTag tauJetLabel_;
    edm::InputTag cenJetLabel_;
    edm::InputTag fwdJetLabel_;
    edm::InputTag muonLabel_;
    edm::InputTag metLabel_;
    edm::InputTag mhtLabel_;
    edm::InputTag hfRingsLabel_;

    bool doUpgrade_;
    edm::InputTag tauLabel_;
    edm::InputTag isoTauLabel_;
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
L1ProtoBufMaker::L1ProtoBufMaker(const edm::ParameterSet& iConfig):
  nonIsoEmLabel_(iConfig.getUntrackedParameter("nonIsoEmLabel",edm::InputTag("l1extraParticles:NonIsolated"))), 
  isoEmLabel_(iConfig.getUntrackedParameter("isoEmLabel",edm::InputTag("l1extraParticles:Isolated"))),
  tauJetLabel_(iConfig.getUntrackedParameter("tauJetLabel",edm::InputTag("l1extraParticles:Tau"))),
  cenJetLabel_(iConfig.getUntrackedParameter("cenJetLabel",edm::InputTag("l1extraParticles:Central"))),
  fwdJetLabel_(iConfig.getUntrackedParameter("fwdJetLabel",edm::InputTag("l1extraParticles:Forward"))),
  muonLabel_(iConfig.getUntrackedParameter("muonLabel",edm::InputTag("l1extraParticles"))),
  metLabel_(iConfig.getUntrackedParameter("metLabel",edm::InputTag("l1extraParticles:MET"))),
  mhtLabel_(iConfig.getUntrackedParameter("mhtLabel",edm::InputTag("l1extraParticles:MHT"))),
  hfRingsLabel_(iConfig.getUntrackedParameter("hfRingsLabel",edm::InputTag("l1extraParticles")))
{
  //now do what ever initialization is needed
  std::ofstream myfile;
  myfile.open ("example.txt");
  myfile << "Writing this to a file.\n";
  myfile.close();

  maxL1Extra_ = iConfig.getParameter<unsigned int>("maxL1Extra");

  l1Extra_     = new L1Analysis::L1AnalysisL1Extra();
  l1ExtraData_ = l1Extra->getData();
  
  // set up output

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
void
L1ProtoBufMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  ///////////////////////////////////////////
  // get L1Extra collectionsfrom the event //
  ///////////////////////////////////////////

  l1Extra->Reset();

  edm::Handle<l1extra::L1EmParticleCollection> isoEm;
  edm::Handle<l1extra::L1EmParticleCollection> nonIsoEm;
  edm::Handle<l1extra::L1JetParticleCollection> cenJet;
  edm::Handle<l1extra::L1JetParticleCollection> fwdJet;
  edm::Handle<l1extra::L1JetParticleCollection> tauJet;
  edm::Handle<l1extra::L1MuonParticleCollection> muon; ;
  edm::Handle<l1extra::L1EtMissParticleCollection> mets;
  edm::Handle<l1extra::L1EtMissParticleCollection> mhts;
  edm::Handle<l1extra::L1HFRingsCollection> hfRings ;

  iEvent.getByLabel(nonIsoEmLabel_, nonIsoEm);
  iEvent.getByLabel(isoEmLabel_, isoEm);
  iEvent.getByLabel(tauJetLabel_, tauJet);
  iEvent.getByLabel(cenJetLabel_, cenJet);
  iEvent.getByLabel(fwdJetLabel_, fwdJet);
  iEvent.getByLabel(muonLabel_, muon);
  iEvent.getByLabel(metLabel_, mets);
  iEvent.getByLabel(mhtLabel_, mhts);
  iEvent.getByLabel(hfRingsLabel_, hfRings);

  if (isoEm.isValid()){ 
    l1Extra_->SetIsoEm(isoEm, maxL1Extra_);
  } else {
    edm::LogWarning("MissingProduct") << "L1Extra Iso Em not found. Branch will not be filled" << std::endl;
  }

  if (nonIsoEm.isValid()){ 
    l1Extra_->SetNonIsoEm(nonIsoEm, maxL1Extra_);
  } else {
    edm::LogWarning("MissingProduct") << "L1Extra Non Iso Em not found. Branch will not be filled" << std::endl;
  }

  if (cenJet.isValid()){ 
    l1Extra_->SetCenJet(cenJet, maxL1Extra_);
  } else {
    edm::LogWarning("MissingProduct") << "L1Extra Central Jets not found. Branch will not be filled" << std::endl;
  }

  if (tauJet.isValid()){ 
    l1Extra_->SetTauJet(tauJet, maxL1Extra_);
  } else {
    edm::LogWarning("MissingProduct") << "L1Extra Tau Jets not found. Branch will not be filled" << std::endl;
  }

  if (fwdJet.isValid()){ 
    l1Extra_->SetFwdJet(fwdJet, maxL1Extra_);
  } else {
    edm::LogWarning("MissingProduct") << "L1Extra Forward Jets not found. Branch will not be filled" << std::endl;
  }

  if (muon.isValid()){ 
    l1Extra_->SetMuon(muon, maxL1Extra_);
  } else {
    edm::LogWarning("MissingProduct") << "L1Extra Muons not found. Branch will not be filled" << std::endl;
  }

  if (mets.isValid()){ 
    l1Extra_->SetMet(mets);
  } else {
    edm::LogWarning("MissingProduct") << "L1Extra MET not found. Branch will not be filled" << std::endl;
  }

  if (mhts.isValid()){ 
    l1Extra_->SetMht(mhts);  
  } else {
    edm::LogWarning("MissingProduct") << "L1Extra MHT not found. Branch will not be filled" << std::endl;
  }

  if (hfRings.isValid()){ 
    l1Extra_->SetHFring(hfRings);  
  } else {
    edm::LogWarning("MissingProduct") << "L1Extra HF Rings not found. Branch will not be filled" << std::endl;
  }

  //////////////////////////////////////////////////
  // Now write this to the protocol buffer output //
  //////////////////////////////////////////////////



// ------------ method called once each job just before starting event loop  ------------
void 
L1ProtoBufMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
L1ProtoBufMaker::endJob() 
{
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
void
L1ProtoBufMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(L1ProtoBufMaker);
