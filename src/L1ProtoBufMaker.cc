// -*- C++ -*-
//
// Package:    L1ProtoBufMaker
// Class:      L1ProtoBufMaker
// 
/**\class L1ProtoBufMaker L1ProtoBufMaker.cc L1Trigger/L1ProtoBufMaker/src/L1ProtoBufMaker.cc

 Description: [EDAnalyzer module to convert AOD to protocol buffers. 
 				Amalgamation of UserCode/L1TriggerDPG/src/L1ExtraTreeProducer.cc, L1AnalysisEvent.cc, etc. 
 				Resulting files can be used with Mark Grimes' menu studies tools.]

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
#include <sys/stat.h>

// framework
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

// data formats
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

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

// Headers from MenuGeneration
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

		// Following are commented out as I might need them in future, but not now
		// virtual void beginRun(edm::Run const&, edm::EventSetup const&);
		// virtual void endRun(edm::Run const&, edm::EventSetup const&);
		// virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
		// virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

	public:
		
		l1menu::ReducedSample * pReducedSample; // reduced sample for getting trigger thresholds, etc

	private:
		// For PU weights:
		bool doPUWeights_;
		std::string puMCFile_;
		std::string puDataFile_;
		std::string puMCHist_;
		std::string puDataHist_;
	    edm::LumiReWeighting lumiWeights_;

		// To read in AOD file:
		// EDM input tags
		edm::InputTag isoEmLabel_;
		edm::InputTag nonIsoEmLabel_;
		edm::InputTag cenJetLabel_;
		edm::InputTag fwdJetLabel_;
		edm::InputTag tauJetLabel_;
		edm::InputTag isoTauJetLabel_;
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
// Here we get 
// - PU data/MC hists names
// - collection names from AOD (some defaults are provided, overridden in the cfi & cfg files), 
// - trigger menu and protobuf output filenames
L1ProtoBufMaker::L1ProtoBufMaker(const edm::ParameterSet& iConfig):
	doPUWeights_(iConfig.getUntrackedParameter("doPUWeights",true)),
	puMCFile_(iConfig.getUntrackedParameter<std::string>("puMCFile","")),
	puDataFile_(iConfig.getUntrackedParameter<std::string>("puDataFile","")),
	puMCHist_(iConfig.getUntrackedParameter<std::string>("puMCHist","pileup")),
	puDataHist_(iConfig.getUntrackedParameter<std::string>("puDataHist","pileup")),
	isoEmLabel_(iConfig.getUntrackedParameter("isoEmLabel",edm::InputTag("l1extraParticles:Isolated"))),
	nonIsoEmLabel_(iConfig.getUntrackedParameter("nonIsoEmLabel",edm::InputTag("l1extraParticles:NonIsolated"))), 
	cenJetLabel_(iConfig.getUntrackedParameter("cenJetLabel",edm::InputTag("l1extraParticles:Central"))),
	fwdJetLabel_(iConfig.getUntrackedParameter("fwdJetLabel",edm::InputTag("l1extraParticles:Forward"))),
	tauJetLabel_(iConfig.getUntrackedParameter("tauJetLabel",edm::InputTag("l1extraParticles:Tau"))),
	isoTauJetLabel_(iConfig.getUntrackedParameter("isoTauJetLabel",edm::InputTag("l1extraParticles:Tau"))),
	muonLabel_(iConfig.getUntrackedParameter("muonLabel",edm::InputTag("l1extraParticles"))),
	metLabel_(iConfig.getUntrackedParameter("metLabel",edm::InputTag("l1extraParticles:MET"))),
	mhtLabel_(iConfig.getUntrackedParameter("mhtLabel",edm::InputTag("l1extraParticles:MHT"))),
	hfRingsLabel_(iConfig.getUntrackedParameter("hfRingsLabel",edm::InputTag("l1extraParticles")))
{

	// Check PU files exist, set weights if they do
	struct stat buf;
	if ((stat(puMCFile_.c_str(), &buf) != -1) && (stat(puDataFile_.c_str(), &buf) != -1)) 
	{
		doPUWeights_ = true;
		lumiWeights_ = edm::LumiReWeighting(puMCFile_, puDataFile_, puMCHist_, puDataHist_);
	}
	else 
	{	
		doPUWeights_ = false;
		edm::LogWarning("L1Prompt") << "No PU reweighting inputs - not going to calculate weights"<<std::endl;
	}

	// Get trigger menu file
	//? why init here and not in list above?
	// TODO(Robin): Deal with this exception better - shouldn't be using std::exception!
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
	protobufFilename_ = iConfig.getUntrackedParameter<std::string>("protobufFilename","default.pb");

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
	edm::Handle<l1extra::L1JetParticleCollection> isoTauJet;
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
	// Nice package of info that can be passed     //
	// to the ReducedSample for further processing //
	/////////////////////////////////////////////////
	
	l1menu::L1TriggerDPGEvent event;

	// set event PU weight, run number, lumi section and event number
	// do PU re-weighting for MC only
	double weight = 1.;

	if (doPUWeights_ && (! iEvent.eventAuxiliary().isRealData())) {

		edm::Handle<std::vector< PileupSummaryInfo > >  puInfo;
		iEvent.getByLabel(edm::InputTag("addPileupInfo"), puInfo);

		if (puInfo.isValid()) {
			std::vector<PileupSummaryInfo>::const_iterator pvi;

			float tnpv = -1;
			for(pvi = puInfo->begin(); pvi != puInfo->end(); ++pvi) {

				int bx = pvi->getBunchCrossing();

				if(bx == 0) { 
					tnpv = pvi->getTrueNumInteractions();
					continue;
				}

			}

			weight = lumiWeights_.weight( tnpv );

		}

	}

	event.setWeight(weight); 
	event.setRun(iEvent.id().run());
	event.setLS(iEvent.id().luminosityBlock());
	event.setEventNum(iEvent.id().event());

	// set trigger bits
	// TODO(Robin): Do setL1Bits correctly! Need to fix in L1TriggerDPGEvent.cc as well
	// event.setL1Bits(SOMETHING);

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
	if (tauJet.isValid())
	{
		event.setTaus( tauJet, isoTauJet );   
	} 
	else 
	{
		edm::LogWarning("MissingProduct") << "L1Extra Non Iso tau jets not found. Branch will not be filled" << std::endl;
	}
	
	if (mets.isValid())
	{
		event.setETSums( mets ); //ETT, ETM, PhiETM, HTT, HTM, PhiHTM
	} 
	else 
	{
		edm::LogWarning("MissingProduct") << "L1Extra mets not found. Branch will not be filled" << std::endl;
	}

	if (mhts.isValid())
	{
		event.setHTSums( mhts ); //ETT, ETM, PhiETM, HTT, HTM, PhiHTM
	} 
	else 
	{
		edm::LogWarning("MissingProduct") << "L1Extra mhts not found. Branch will not be filled" << std::endl;
	}

	// if (hfRings.isValid())
	// {
	//   event.setHFring( hfRings ); //ETT, ETM, PhiETM, HTT, HTM, PhiHTM
	// } 
	// else 
	// {
	//   edm::LogWarning("MissingProduct") << "L1Extra hfRings not found. Branch will not be filled" << std::endl;
	// } 

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
// void 
// L1ProtoBufMaker::beginRun(edm::Run const&, edm::EventSetup const&)
// {
// }

// ------------ method called when ending the processing of a run  ------------
// void 
// L1ProtoBufMaker::endRun(edm::Run const&, edm::EventSetup const&)
// {
// }

// ------------ method called when starting to processes a luminosity block  ------------
// void 
// L1ProtoBufMaker::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
// {
// }

// ------------ method called when ending the processing of a luminosity block  ------------
// void 
// L1ProtoBufMaker::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
// {
// }

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
