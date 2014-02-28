import FWCore.ParameterSet.Config as cms

# import master L1 Ntuple config
from L1Trigger.L1ProtoBufMaker.l1CurrentNtupleFromRAW import *

print "Adding Stage 1 algorithms"

# add Stage 1 algorithms
# produces necessary uct2015L1ExtraParticles collections
process.load('UserCode.L1TriggerUpgrade.L1UCT2015_cff')
process.UCT2015Producer.puCorrectSums = cms.bool(False) # hack. no idea if this should be true or false
process.UCT2015Producer.useHI = cms.bool(False) # hack. no idea if this should be true or false
process.UCT2015Producer.switchOffTauIso = cms.double(60.0) # hack. no idea waht this shoudl be. nicked from emulation_cfi

process.uct2015L1ExtraParticles.muonSource = cms.InputTag("valGmtDigis")
process.uct2015L1ExtraParticles.produceMuonParticles = cms.bool(True)

# change the input collections for l1ProtoBufMaker using new UCT collections
process.l1ProtoBufMaker.egLabel       = cms.untracked.InputTag("uct2015L1ExtraParticles", "Relaxed")
process.l1ProtoBufMaker.isoEGLabel    = cms.untracked.InputTag("uct2015L1ExtraParticles", "Isolated")
process.l1ProtoBufMaker.cenJetLabel   = cms.untracked.InputTag("uct2015L1ExtraParticles", "Jets")
process.l1ProtoBufMaker.fwdJetLabel   = cms.untracked.InputTag("uct2015L1ExtraParticles", "FwdJets")
process.l1ProtoBufMaker.tauLabel      = cms.untracked.InputTag("uct2015L1ExtraParticles", "RelaxedTau")
process.l1ProtoBufMaker.isoTauLabel   = cms.untracked.InputTag("uct2015L1ExtraParticles", "IsolatedTau")
process.l1ProtoBufMaker.doReEmulMuons = cms.untracked.bool(True)
process.l1ProtoBufMaker.muonLabel     = cms.untracked.InputTag("valGmtDigis")
# process.l1ProtoBufMaker.muonLabel     = cms.untracked.InputTag("uct2015L1ExtraParticles")
process.l1ProtoBufMaker.metLabel      = cms.untracked.InputTag("uct2015L1ExtraParticles", "MET")
process.l1ProtoBufMaker.mhtLabel      = cms.untracked.InputTag("uct2015L1ExtraParticles", "MHT")

# add upgrade algorithms to the path, just before the l1ProtoBufMaker module
ind = process.p.index(process.l1ProtoBufMaker)
process.p.insert(ind,process.L1UCT2015)

# Debug
file = open("l1Stage1NtupleFromGENRAW_cfg.py",'w')
file.write(str(process.dumpPython()))
file.close()

print "Done"
