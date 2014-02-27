import FWCore.ParameterSet.Config as cms

# Robin Aggleton 18/2/14, robin.aggleton@cern.ch
#
# Default configuration for L1ProtoBufMaker EDAnalyzer
# Include this in your _cfg.py file by adding:
# 
# process.load("L1Trigger.L1ProtoBufMaker.l1ProtoBufMaker_cfi")
#
# Override default options with statements like:
# 
# processs.l1ProtoBufMaker.protobufFilename = cms.string("myFile.pb")
#

l1ProtoBufMaker = cms.EDAnalyzer('L1ProtoBufMaker',
    doPUWeights      = cms.untracked.bool(False), # currently missing the PU files...
    puMCFile         = cms.untracked.string("PUHistS10.root"),
    puDataFile       = cms.untracked.string(""),
    puMCHist         = cms.untracked.string("pileup"), # Hist name in files above
    puDataHist       = cms.untracked.string("pileup"),
    gtSource         = cms.untracked.InputTag("gtDigis"), # For L1 Global Trigger bits
    nonIsoEmLabel    = cms.untracked.InputTag("l1extraParticles:NonIsolated"),
    isoEmLabel       = cms.untracked.InputTag("l1extraParticles:Isolated"),
    tauJetLabel      = cms.untracked.InputTag("l1extraParticles:Tau"),
    isoTauJetLabel   = cms.untracked.InputTag("none"), # set to "none" if you don't want any isolation testing
    cenJetLabel      = cms.untracked.InputTag("l1extraParticles:Central"),
    fwdJetLabel      = cms.untracked.InputTag("l1extraParticles:Forward"),
    doReEmulMuons    = cms.untracked.bool(False), # whether to use re emulated muons from GMT, or use l1extra muons. Use former until simulation in place.
    muonLabel        = cms.untracked.InputTag("l1extraParticles"), # "gtDigis" for re emul muons, or "l1extraParticles" for l1extra collection
    metLabel         = cms.untracked.InputTag("l1extraParticles:MET"),
    mhtLabel         = cms.untracked.InputTag("l1extraParticles:MHT"),
    hfRingsLabel     = cms.untracked.InputTag("l1extraParticles"), # Not used at the moment
    menuFilename     = cms.string("/storage/phrca/L1Menu/CMSSW_6_1_2_SLHC2/src/L1Trigger/MenuGeneration/test/L1Menu_input_std.txt"), # Trigger menu input file
    protobufFilename = cms.untracked.string("test.pb") # Output protobuf file
)

print "Loading L1ProtoBufMaker defaults from l1ProtoBufMaker_cfi.py"