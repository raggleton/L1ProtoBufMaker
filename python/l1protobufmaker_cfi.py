import FWCore.ParameterSet.Config as cms

protoBufMaker = cms.EDAnalyzer('L1ProtoBufMaker',
	nonIsoEmLabel = cms.untracked.InputTag("l1extraParticles:NonIsolated"),
  isoEmLabel = cms.untracked.InputTag("l1extraParticles:Isolated"),
  tauJetLabel = cms.untracked.InputTag("l1extraParticles:Tau"),
  cenJetLabel = cms.untracked.InputTag("l1extraParticles:Central"),
  fwdJetLabel = cms.untracked.InputTag("l1extraParticles:Forward"),
  muonLabel = cms.untracked.InputTag("l1extraParticles"),
  metLabel = cms.untracked.InputTag("l1extraParticles:MET"),
  mhtLabel = cms.untracked.InputTag("l1extraParticles:MHT"),
  hfRingsLabel = cms.untracked.InputTag("l1extraParticles"),
  menuFilename = cms.string("/storage/phrca/L1Menu/CMSSW_6_0_1/src/L1Trigger/MenuGeneration/test/L1Menu_input_std.txt"),
  protobufFilename = cms.string("test.pb")
)

print "Loading l1protobufmaker_cfi.py"