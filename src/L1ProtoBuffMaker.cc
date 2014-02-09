// -*- C++ -*-
//
// Package:    L1ProtoBuffMaker
// Class:      L1ProtoBuffMaker
// 
/**\class L1ProtoBuffMaker L1ProtoBuffMaker.cc L1Trigger/L1ProtoBuffMaker/src/L1ProtoBuffMaker.cc

 Description: [EDAnalyzer module to convert AOD to protocol buffers. Resulting files can be used with Mark Grimes' menu studies tools.]

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
//
// class declaration
//

class L1ProtoBuffMaker : public edm::EDAnalyzer {
   public:
      explicit L1ProtoBuffMaker(const edm::ParameterSet&);
      ~L1ProtoBuffMaker();

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
L1ProtoBuffMaker::L1ProtoBuffMaker(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
  std::ofstream myfile;
  myfile.open ("example.txt");
  myfile << "Writing this to a file.\n";
  myfile.close();


}


L1ProtoBuffMaker::~L1ProtoBuffMaker()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
L1ProtoBuffMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;



#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void 
L1ProtoBuffMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
L1ProtoBuffMaker::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
void 
L1ProtoBuffMaker::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
L1ProtoBuffMaker::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
L1ProtoBuffMaker::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
L1ProtoBuffMaker::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
L1ProtoBuffMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(L1ProtoBuffMaker);
