L1ProtoBufMaker
================

Code for EDAnalyzer module to produce lightweight L1 trigger upgrade protocol buffer files from RAW format. No need to go to ntuples and then convert to protocol buffers.

The idea behind using protocol buffers is that we only store the relevant info: for a given event, for a given trigger; *what is the tightest threshold at which this event would pass this trigger*. So when we come to look at rates, we can quickly and easily iterate over different bandwidths for each trigger by applying a threshold and counting the events which pass.

More on protocol buffers: 

(https://developers.google.com/protocol-buffers/docs/overview)

Please so [TODO](TODO.md) for things Robin has yet to do...

##Installation instructions

Note this is for the non-track-trigger version.

```
scram project CMSSW_6_1_2_SLHC2
cd CMSSW_6_1_2_SLHC2/src
cmsenv

git cms-addpkg SLHCUpgradeSimulations/L1CaloTrigger
git cms-addpkg SimDataFormats/SLHC
git remote add jbsauvan-cmssw git@github.com:jbsauvan/cmssw.git
git fetch jbsauvan-cmssw new-eg-l1-clustering:new-eg-l1-clustering
git checkout new-eg-l1-clustering

git clone https://github.com/uwcms/UCT2015.git L1Trigger/UCT2015
git cms-addpkg L1Trigger/RegionalCaloTrigger
git cms-addpkg DataFormats/L1CaloTrigger
git cms-addpkg L1TriggerConfig/L1ScalesProducers
patch -N -p0 < L1Trigger/UCT2015/eic9bit.patch

cvs co -r jimb4Jan2013 UserCode/L1TriggerDPG
cvs co -r jimb14May2013 UserCode/L1TriggerUpgrade

git clone git@github.com:mark-grimes/MenuGeneration.git L1Trigger/MenuGeneration
# add Robin's fork & branch to MenuGeneration. Does this work properly?
cd L1Trigger/MenuGeneration
git remote add Robin git@github.com:raggleton/MenuGeneration.git
git fetch Robin
git checkout -b ProtoBufMaker Robin/ProtoBufMaker
cd ../..
git clone git@github.com:raggleton/L1ProtoBufMaker.git L1Trigger/L1ProtoBufMaker

scram b -j 6
```
(https://github.com/mark-grimes/MenuGeneration)

## Running instructions

The `python` folder contains basic scripts for 4 scenarios: current, Stage 1 upgrades, Stage 2 upgrades, and Fallback (Stage 1 EG, tau, muon iso + Stage 2 jets). They all use l1CurrentNtupleFromRAW.py as a base config. Run them with `cmsRun <scriptname>`. 

The `test` folder contains data & MC specific configurations. MC have GEN in the filename.

All configs take a (GEN-SIM-DIGI-)RAW file, loads some modules which add collections of objects (e.g. l1Extra), then finally loads the l1ProtoBufMaker module & default args from `python/l1ProtoBufMaker_cfi.py`. The user can then override which EG/jet/etc collections are used when applying the triggers. Example:

```
# Add in l1ProtoBufMaker module
process.load("L1Trigger.L1ProtoBufMaker.l1ProtoBufMaker_cfi")
process.l1ProtoBufMaker.egLabel       = cms.untracked.InputTag("uct2015L1ExtraParticles", "Relaxed")
process.l1ProtoBufMaker.isoEGLabel    = cms.untracked.InputTag("uct2015L1ExtraParticles", "Isolated")
process.l1ProtoBufMaker.cenJetLabel   = cms.untracked.InputTag("uct2015L1ExtraParticles", "Jets")
```

The source code for the module is in `src/L1ProtoBufMaker.cc`. 

## Detailed description

[This is the boring bit, for future me, or anyone who has to maintain or develop on top of my code]

Basic flow:

1. **Start:** (`L1ProtoBufMaker::L1ProtoBufMaker()`)
	- Initialise PU weights, get trigger menu file, get collection names for jets, EGamma, etc. 
	- We setup a `l1menu::ReducedSample` object, which is a handy container for: 
		- the L1 trigger bits for the sample
		- the event #, run #, lumi section
		- a `l1menu::L1TriggerDPGEvent` object, which hold information about all the jets, EGamma, etc. in an event. What's stored is defined in `UserCode/L1TriggerUpgrade/interface/L1AnalysisDataFormat.h` 
	- This also initialises a protobuf file, and writes the user-defined trigger menu to it.

2. **For each event:** (`L1ProtoBufMaker::analyze()`)
	-  Create a `l1menu::L1TriggerDPGEvent` object to store event information.
	-  Calculate the PU weight (MC only).
	-  Set run #, LS, event #.
	-  Set L1 trigger bits from Global Trigger.
	-  Get the `l1extra` collections from the RAW collections. 
	-  Fill the event object with EG/jet/muon/… information from the input collections.
	-  Add the `L1TriggerDPGEvent` object to `ReducedSample`. This does the following:
		- Check to see if a new `l1protobuf::Run` object is required. ( The protobuf events are stored in groups of 20000 in a  `l1protobuf::Run`, to circumvent a limit set by Google.)
		-  Make a new `l1protobuf::Event` object to store the trigger thresholds in.
		-  Loop over all the triggers in the menu. For each, set the trigger threshold as tight as possible, whilst still allowing the event to pass the trigger. We pass our `L1TriggerDPGEvent` object to `l1menu::tools::setTriggerThresholdsAsTightAsPossible`.
		-  Store the trigger threshold name(s) and value(s) in the `l1protobuf::Event` object.

3. **End:** (`L1ProtoBufMaker::endJob()`)
	- Tell the `ReducedSample` object to save all the events in a protobuf file.

	
##Notes

- This is dependant on both `MenuGeneration` and `L1TriggerUpgrade` packages. Ideally we'd remove the dependance on the latter, since CVS is depreciated and the package isn't maintained.
- The event format is defined in `L1AnalysisDataFormat.h`. This isn't ideal, and it would be good to move away from this. 
- Also there appears to be several different formats floating about amongst `L1TriggerDPG` and `L1TriggerUpgrade` packages (e.g. `L1AnalysisL1ExtraDataFormat`, `L1AnalysisL1ExtraUpgradeDataFormat`) - not sure which is best/correct.
- If we *do* change `L1AnalysisDataFormat.h`, the following must also change:
	- `MenuGeneration/src/L1TriggerDPGEvent.cpp` - the `setX` methods must be updated
	- the `apply()` methods in the triggers `MenuGeneration/src/triggers/X.cpp` 
- The way the code is designed, no changes to `L1ProtoBufmaker.cc` should be necessary - everything happens through the setters in `L1TriggerDPGEvent.cpp`.
