L1ProtoBufMaker
================

Code for EDAnalyzer module to produce lightweight L1 trigger upgrade protocol buffer files from AOD format. No need to go to ntuples and then convert to protocol buffers.

The idea behind using protocol buffers is that we only store the relevant info: for a given event, for a given trigger; *what is the tightest threshold at which this event would pass this trigger*. So when we come to look at rates, we can quickly and easily iterate over different bandwidths for each trigger by applying a threshold and counting the events which pass.

More on protocol buffers: 

(https://developers.google.com/protocol-buffers/docs/overview)

##Installation instructions

```
scram project CMSSW CMSSW_6_0_1 # isn't dependant on this specific CMSSW version.
cd CMSSW_6_0_1/src
cmsenv
git cms-addpkg Documentation/ReferenceManualScripts
git clone git@github.com:mark-grimes/MenuGeneration.git L1Trigger/MenuGeneration
# add Robin's fork & branch to MenuGeneration. Will put in master branch at some point
cd L1Trigger/MenuGeneration
git remote add RobinProtoBufMaker git@github.com:raggleton/MenuGeneration.git
git fetch RobinProtoBufMaker
git checkout -b ProtoBufMaker RobinProtoBufMaker/ProtoBufMaker
cd ../..
git clone git@github.com:raggleton/L1ProtoBufMaker.git L1Trigger/L1ProtoBufMaker
export CVSROOT=:ext:<your-cern-username>@lxplus.cern.ch:/afs/cern.ch/user/c/cvscmssw/public/CMSSW
cvs co UserCode/L1TriggerDPG
cvs co UserCode/L1TriggerUpgrade
cd L1Trigger  # doesn't like to checkout to 2 folders deep 
cvs co -d UCT2015 UserCode/dasu/L1Trigger/UCT2015
scram b -j 6
scram b doc
```
(https://github.com/mark-grimes/MenuGeneration)

(https://github.com/kknb1056/MenuGeneration)

## Running instructions

For a simple example see `l1protobufmaker_cfg.py`. This takes an AOD file, loads the protoBufMaker module & default args from `python/l1protobufmaker_cfi.py`. The source code for the module is in `src/L1ProtoBufMaker.cc`. 

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
	-  Get the `l1extra` collections from the AOD. (Except for muons - use re-emulated muons by default form GMT.)
	-  Fill the event object with EG/jet/muon/… information from the AOD collections. [**TODO - muons, taus, MHT properly, jet co-ords**]
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

*Yes, I'm well aware that I've used proto**B**uf  and proto**b**uf in my code. I should probably be more consistent. But at least I've never used buf**f** *