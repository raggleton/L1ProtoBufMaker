import FWCore.ParameterSet.Config as cms

# Robin Aggleton 18/2/14, robin.aggleton@cern.ch
#
# Default configuration for L1ProtoBufMaker EDAnalyzer
# Include this in your _cfg.py file by adding:
# 
# process.load("L1Trigger.L1ProtoBufMaker.l1protobufmaker_cfi")
#
# Override default options with statements like
# 
# processs.protoBufMaker.protobufFilename = cms.string("myFile.pb")
#

protoBufMaker = cms.EDAnalyzer('L1ProtoBufMaker',
    doPUWeights = cms.untracked.bool(False), # currently missing the PU files...
    puMCFile = cms.untracked.string(""),
    puDataFile = cms.untracked.string(""),
    puMCHist = cms.untracked.string("pileup"),
    puDataHist = cms.untracked.string("pileup"),
    gtSource = cms.untracked.InputTag("gtDigis"),
    nonIsoEmLabel = cms.untracked.InputTag("l1extraParticles:NonIsolated"),
    isoEmLabel = cms.untracked.InputTag("l1extraParticles:Isolated"),
    tauJetLabel = cms.untracked.InputTag("l1extraParticles:Tau"),
    cenJetLabel = cms.untracked.InputTag("l1extraParticles:Central"),
    fwdJetLabel = cms.untracked.InputTag("l1extraParticles:Forward"),
    muonLabel = cms.untracked.InputTag("l1extraParticles"),
    metLabel = cms.untracked.InputTag("l1extraParticles:MET"),
    mhtLabel = cms.untracked.InputTag("l1extraParticles:MHT"),
    hfRingsLabel = cms.untracked.InputTag("l1extraParticles"),
    menuFilename = cms.string("../MenuGeneration/test/L1Menu_input_std.txt"),
    protobufFilename = cms.untracked.string("test.pb")
)

print "Loading l1protobufmaker_cfi.py"