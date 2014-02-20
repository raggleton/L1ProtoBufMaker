import FWCore.ParameterSet.Config as cms

# Robin Aggleton 18/2/14, robin.aggleton@cern.ch
#
# Default configuration for L1ProtoBufMaker EDAnalyzer
# Include this in your _cfg.py file by adding:
# 
# process.load("L1Trigger.L1ProtoBufMaker.l1protobufmaker_cfi")
#
# Override default options with statements like:
# 
# processs.protoBufMaker.protobufFilename = cms.string("myFile.pb")
#

protoBufMaker = cms.EDAnalyzer('L1ProtoBufMaker',
    doPUWeights      = cms.untracked.bool(False), # currently missing the PU files...
    puMCFile         = cms.untracked.string(""),
    puDataFile       = cms.untracked.string(""),
    puMCHist         = cms.untracked.string("pileup"), # Hist name in files above
    puDataHist       = cms.untracked.string("pileup"),
    gtSource         = cms.untracked.InputTag("gtDigis"), # For L1 Global Trigger bits
    nonIsoEmLabel    = cms.untracked.InputTag("l1extraParticles:NonIsolated"),
    isoEmLabel       = cms.untracked.InputTag("l1extraParticles:Isolated"),
    tauJetLabel      = cms.untracked.InputTag("l1extraParticles:Tau"),
    cenJetLabel      = cms.untracked.InputTag("l1extraParticles:Central"),
    fwdJetLabel      = cms.untracked.InputTag("l1extraParticles:Forward"),
    doReEmulMuons    = cms.untracked.bool(False), # whether to use re emulated muons from GMT, or use l1extra muons. Use former until simulation in place.
    muonLabel        = cms.untracked.InputTag("l1extraParticles"), # "gtDigis" for re emul muons, or "l1extraParticles" for l1extra collection
    metLabel         = cms.untracked.InputTag("l1extraParticles:MET"),
    mhtLabel         = cms.untracked.InputTag("l1extraParticles:MHT"),
    hfRingsLabel     = cms.untracked.InputTag("l1extraParticles"), # Not used at the moment
    menuFilename     = cms.string("../MenuGeneration/test/L1Menu_input_std.txt"), # Trigger menu input file
    protobufFilename = cms.untracked.string("test.pb") # Output protobuf file
)

print "Loading L1ProtoBufMaker defaults from l1protobufmaker_cfi.py"