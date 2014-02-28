// -*- C++ -*-
//
// Package:    L1ProtoBufMaker
// Class:      L1ProtoBufMaker
// 
/**\class L1ProtoBufMaker L1ProtoBufMaker.cc L1Trigger/L1ProtoBufMaker/src/L1ProtoBufMaker.cc

 Description: [EDAnalyzer module to convert collections in files to protocol buffers. 
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
#include <algorithm>

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
// #include "UserCode/L1TriggerUpgrade/interface/L1AnalysisDataFormat.h"
// #include "UserCode/L1TriggerUpgrade/interface/L1AnalysisL1ExtraUpgradeDataFormat.h"

// #include "UserCode/L1TriggerDPG/interface/L1AnalysisGT.h"
// #include "UserCode/L1TriggerDPG/interface/L1AnalysisGTDataFormat.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerEvmReadoutRecord.h"

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
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"

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


	private:
		l1menu::ReducedSample * pReducedSample; // reduced sample for getting trigger thresholds, etc

		// For PU weights:
		bool doPUWeights_;
		std::string puMCFile_;
		std::string puDataFile_;
		std::string puMCHist_;
		std::string puDataHist_;
		edm::LumiReWeighting lumiWeights_;

		// For L1Trigger bits
		edm::InputTag gtSource_;

		// To read in collections
		edm::InputTag egLabel_;
		edm::InputTag isoEGLabel_;
		edm::InputTag cenJetLabel_;
		edm::InputTag fwdJetLabel_;
		edm::InputTag tauLabel_;
		edm::InputTag isoTauLabel_;
		bool doReEmulMuons_; // whether to use re emulated muons or l1extra muons
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
// - GT digis (for L1Trigger bits)
// - collection names (some defaults are provided, overridden in the cfi & cfg files), 
// - trigger menu and protobuf output filenames
L1ProtoBufMaker::L1ProtoBufMaker(const edm::ParameterSet& iConfig):
	doPUWeights_(iConfig.getUntrackedParameter("doPUWeights",true)),
	puMCFile_(iConfig.getUntrackedParameter<std::string>("puMCFile","")),
	puDataFile_(iConfig.getUntrackedParameter<std::string>("puDataFile","")),
	puMCHist_(iConfig.getUntrackedParameter<std::string>("puMCHist","pileup")),
	puDataHist_(iConfig.getUntrackedParameter<std::string>("puDataHist","pileup")),
	gtSource_(iConfig.getUntrackedParameter("gtSource",edm::InputTag("gtDigis"))),
	egLabel_(iConfig.getUntrackedParameter("egLabel",edm::InputTag("l1extraParticles:NonIsolated"))), 
	isoEGLabel_(iConfig.getUntrackedParameter("isoEGLabel",edm::InputTag("l1extraParticles:Isolated"))),
	cenJetLabel_(iConfig.getUntrackedParameter("cenJetLabel",edm::InputTag("l1extraParticles:Central"))),
	fwdJetLabel_(iConfig.getUntrackedParameter("fwdJetLabel",edm::InputTag("l1extraParticles:Forward"))),
	tauLabel_(iConfig.getUntrackedParameter("tauLabel",edm::InputTag("l1extraParticles:Tau"))),
	isoTauLabel_(iConfig.getUntrackedParameter("isoTauLabel",edm::InputTag("none"))),
	doReEmulMuons_(iConfig.getUntrackedParameter("doReEmulMuons",true)),
	muonLabel_(iConfig.getUntrackedParameter("muonLabel",edm::InputTag("gmtDigis"))),
	metLabel_(iConfig.getUntrackedParameter("metLabel",edm::InputTag("l1extraParticles:MET"))),
	mhtLabel_(iConfig.getUntrackedParameter("mhtLabel",edm::InputTag("l1extraParticles:MHT"))),
	hfRingsLabel_(iConfig.getUntrackedParameter("hfRingsLabel",edm::InputTag("l1extraParticles")))
{

	// Check PU files exist, set weights if they do
	LogDebug("MethodStart") << "Doing ctor: PU";
	if (doPUWeights_)
	{
		struct stat buf;
		if ((stat(puMCFile_.c_str(), &buf) != -1) && (stat(puDataFile_.c_str(), &buf) != -1)) 
		{
			lumiWeights_ = edm::LumiReWeighting(puMCFile_, puDataFile_, puMCHist_, puDataHist_);
			edm::LogInfo("UserOption") << "Setup PU reweighting";
		}
		else 
		{	
			doPUWeights_ = false; // set doPUWeights_ to false, even if the user wanted them done - there's no files!
			edm::LogWarning("MissingFile") << "No PU reweighting inputs - not going to calculate weights"<<std::endl;
		}
	}
	else
		edm::LogWarning("OptionOff") << "No PU reweighting inputs - not going to calculate weights"<<std::endl;

	// Get trigger menu file
	//? why init here and not in list above?
	// TODO(Robin): Deal with this exception better - shouldn't be using std::exception!
	LogDebug("MethodStart") << "Doing ctor: trigger menu";
	try
	{
		std::string menuFilename = iConfig.getParameter<std::string>("menuFilename");
		edm::LogInfo("UserOption") << "Loading menu from file " << menuFilename;
		std::unique_ptr<l1menu::TriggerMenu> pMyMenu=l1menu::tools::loadMenu( menuFilename );
		pReducedSample = new l1menu::ReducedSample( *pMyMenu );
	}
	catch( std::exception& error )
	{
		std::cerr << "Exception caught: " << error.what() << std::endl;
		exit(2);
	}

	// Get output protobuf file name
	protobufFilename_ = iConfig.getUntrackedParameter<std::string>("protobufFilename","default.pb");
	edm::LogInfo("UserOption") << "Output filename: " << protobufFilename_ ;
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

	LogDebug("MethodStart") << "Begin analyze()";

	/////////////////////////////////////////////////
	// Create & fill L1TriggerDPGEvent             //
	//                                             //
	// Nice package of info that can be passed     //
	// to the ReducedSample for further processing //
	/////////////////////////////////////////////////
	
	// TODO(Robin): Put these in easy to comprehend methods, not in one long one!
	l1menu::L1TriggerDPGEvent event;
	LogDebug("EventSetup") << "Begin setting up event";

	////////////////////////////////////////////////////////////////////
	//                                                                //
	// Set event PU weight, run number, lumi section and event number //
	// Do PU re-weighting for MC only                                 //
	// Used code from from L1AnalysisEvent.cc,                        //
	//                                                                //
	////////////////////////////////////////////////////////////////////
	
	double weight = 1.;

	if (doPUWeights_ && (! iEvent.eventAuxiliary().isRealData())) 
	{
		edm::Handle<std::vector< PileupSummaryInfo > >  puInfo;
		iEvent.getByLabel(edm::InputTag("addPileupInfo"), puInfo);

		if (puInfo.isValid()) {
			LogDebug("PU") << "Setting PU weights";
			std::vector<PileupSummaryInfo>::const_iterator pvi;
			float tnpv = -1;
			for(pvi = puInfo->begin(); pvi != puInfo->end(); ++pvi) 
			{

				int bx = pvi->getBunchCrossing();
				if(bx == 0) 
				{ 
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

	//////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                          //
	// Set L1 trigger bits                                                                      //
	// Used code from L1NtupleProducer, FullSample.cpp, L1AnalysisGT.cc                         //
	// Basically amalgamating the setting and getting of L1 trigger bits from the GlobalTrigger //
	//                                                                                          //
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	bool PhysicsBits[128] = {false}; // set default off, incase below fails
	edm::Handle<L1GlobalTriggerReadoutRecord> gtrr_handle;
	iEvent.getByLabel(gtSource_, gtrr_handle);

	if (gtrr_handle.isValid())
	{
		// I've removed the reliance upon L1AnalysisGT:
		// a) to save time converting from one obj to another 
		// (L1AnalysisGT converts DecisionWord to 2 64-bit arrays, FullSample converts them back to a 128-bit array)
		// b) because it doesn't want to link to the UserCode library. I have no idea why.
		
		L1GlobalTriggerReadoutRecord const* gtrr = gtrr_handle.product();
		// typedef std::vector<bool> DecisionWord;
		// See DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h 
		DecisionWord gtDecisionWord = gtrr->decisionWord(0); // Only need 0th bunch crossing - it also stores ±2 either side of 0th bx
		std::copy(gtDecisionWord.begin(), gtDecisionWord.end(), PhysicsBits); // convert from vector<bool> to bool array
	}
	else
	{
		edm::LogWarning("MissingProduct") << "GT source invalid. L1 Trigger Bits all set false!" << std::endl;
	}

	event.setL1Bits(PhysicsBits);

	LogDebug("EventSetup") << "Finished setting up PU, event#, trigger bits";

	/////////////////////////////////////////
	//                                     //
	// Fill event with obj collection info //
	//                                     //
	/////////////////////////////////////////

	//
	// Get L1Extra collections from the event 
	// Pass collections to setters in L1TriggerDPGEvent obj
	// 
	
	LogDebug("EventFilling") << "Start filling event with collection info";

	LogDebug("EventFilling") << "Doing EG";
	edm::Handle<l1extra::L1EmParticleCollection> isoEG;
	edm::Handle<l1extra::L1EmParticleCollection> nonIsoEG;
	iEvent.getByLabel(isoEGLabel_, isoEG);
	iEvent.getByLabel(egLabel_, nonIsoEG);
	
	if (nonIsoEG.isValid())
	{  
		if (isoEG.isValid())
		{
			event.setEG( nonIsoEG, isoEG);   
		}
		else
		{
			edm::LogWarning("MissingProduct") << "L1Extra Iso Em not found. Branch will not be filled" << std::endl;
		}
	} 
	else 
	{
		edm::LogWarning("MissingProduct") << "L1Extra Non Iso EM not found. Branch will not be filled" << std::endl;
	}
	

	LogDebug("EventFilling") << "Doing Jets";
	edm::Handle<l1extra::L1JetParticleCollection> cenJet;
	edm::Handle<l1extra::L1JetParticleCollection> fwdJet;
	iEvent.getByLabel(cenJetLabel_, cenJet);
	iEvent.getByLabel(fwdJetLabel_, fwdJet);
	if (cenJet.isValid() )
	{
		if (fwdJet.isValid())
		{
			event.setJets( cenJet, fwdJet );
		}
		else
		{
			edm::LogWarning("MissingProduct") << "L1Extra fwd Jet not found. Branch will not be filled" << std::endl;
		}
	} 
	else 
	{
		edm::LogWarning("MissingProduct") << "L1Extra cen Jet not found. Branch will not be filled" << std::endl;
	}


	LogDebug("EventFilling") << "Doing Taus";
	edm::Handle<l1extra::L1JetParticleCollection> tau;
	edm::Handle<l1extra::L1JetParticleCollection> isoTau;
	iEvent.getByLabel(tauLabel_, tau);
	if (tau.isValid())
	{
		if ( isoTauLabel_.label() != "none" )
		{
			iEvent.getByLabel(isoTauLabel_, isoTau);
			if (!isoTau.isValid())
				edm::LogWarning("MissingProduct") << "L1Extra iso tau jets not found. Branch will not be filled" << std::endl;
		}
		event.setTaus( tau, isoTau );   
	} 
	else 
	{
		edm::LogWarning("MissingProduct") << "L1Extra non-iso tau jets not found. Branch will not be filled" << std::endl;
	}	

	
	LogDebug("EventFilling") << "Doing MET";
	edm::Handle<l1extra::L1EtMissParticleCollection> mets;
	iEvent.getByLabel(metLabel_, mets);
	if (mets.isValid())
	{
		event.setETSums( mets ); //ETT, ETM, PhiETM, HTT, HTM, PhiHTM
	} 
	else 
	{
		edm::LogWarning("MissingProduct") << "L1Extra mets not found. Branch will not be filled" << std::endl;
	}


	LogDebug("EventFilling") << "Doing MHT";
	edm::Handle<l1extra::L1EtMissParticleCollection> mhts;
	iEvent.getByLabel(mhtLabel_, mhts);
	if (mhts.isValid())
	{
		event.setHTSums( mhts ); //ETT, ETM, PhiETM, HTT, HTM, PhiHTM
	} 
	else 
	{
		edm::LogWarning("MissingProduct") << "L1Extra mhts not found. Branch will not be filled" << std::endl;
	}


	// LogDebug("EventFilling") << "Doing HFRings";
	// edm::Handle<l1extra::L1HFRingsCollection> hfRings;
	// iEvent.getByLabel(hfRingsLabel_, hfRings);
	// if (hfRings.isValid())
	// {
	//   event.setHFring( hfRings ); //ETT, ETM, PhiETM, HTT, HTM, PhiHTM
	// } 
	// else 
	// {
	//   edm::LogWarning("MissingProduct") << "L1Extra hfRings not found. Branch will not be filled" << std::endl;
	// } 


	LogDebug("EventFilling") << "Doing Muons";
	if (doReEmulMuons_) 
	{	
		LogDebug("UserOption") << "Using GMT re-emulated muons, not L1Extra";
		edm::Handle<L1MuGMTReadoutCollection> reEmulMuon;
		iEvent.getByLabel(muonLabel_, reEmulMuon); // use L1MuGMTReadoutCollection handle
		if (reEmulMuon.isValid())
		{
			event.setMuons( reEmulMuon );
		}
		else
		{
			edm::LogWarning("MissingProduct") << "GMT Muons not found. Branch will not be filled" << std::endl;
		}
		
	}
	else  
	{
		LogDebug("UserOption") << "Using L1Extra muons, not GMT re-emulated";
		edm::Handle<l1extra::L1MuonParticleCollection> muon;
		iEvent.getByLabel(muonLabel_, muon); // use l1extra collection handle
		if (muon.isValid())
		{
			event.setMuons( muon );
		}
		else
		{
			edm::LogWarning("MissingProduct") << "L1 Extra Muons not found. Branch will not be filled" << std::endl;
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////
	// Pass L1TriggerDPGEvent object to ReducedSample to set trigger thresholds //
	//////////////////////////////////////////////////////////////////////////////

	// pReducedSample->addEvent( event ); 

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
		// pReducedSample->saveToFile( protobufFilename_ );
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
