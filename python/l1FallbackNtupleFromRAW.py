import FWCore.ParameterSet.Config as cms

# import master L1 Ntuple config
from L1Trigger.L1ProtoBufMaker.l1CurrentNtupleFromRAW import *

# add Stage 1 EG, tau, muon isolation
print "Adding Stage 1 EG, tau, muon iso algorithms"
process.load('UserCode.L1TriggerUpgrade.L1UCT2015_cff')
process.UCT2015Producer.puCorrectSums = cms.bool(False) # hack. no idea if this should be true or false
process.UCT2015Producer.useHI = cms.bool(False) # hack. no idea if this should be true or false
process.UCT2015Producer.switchOffTauIso = cms.double(60.0) # hack. no idea waht this shoudl be. nicked from emulation_cfi

process.uct2015L1ExtraParticles.muonSource = cms.InputTag("valGmtDigis")
process.uct2015L1ExtraParticles.produceMuonParticles = cms.bool(True)

process.p += process.L1UCT2015

# add Stage 2 jets
print "Adding Stage 2 jet, sum algorithms"
process.load('SLHCUpgradeSimulations.L1CaloTrigger.SLHCCaloTrigger_cff')
process.L1CaloTowerProducer.ECALDigis = cms.InputTag("ecalDigis:EcalTriggerPrimitives")
process.L1CaloTowerProducer.HCALDigis =  cms.InputTag("hcalDigis")

process.p += process.L1CaloTowerProducer
process.p += process.L1TowerJetProducer
process.p += process.L1TowerJetFilter1D
process.p += process.L1TowerJetFilter2D
process.p += process.L1TowerFwdJetProducer
process.p += process.L1TowerFwdJetFilter1D
process.p += process.L1TowerFwdJetFilter2D
process.p += process.L1TowerJetPUEstimator
process.p += process.L1TowerJetPUSubtractedProducer # for cen jets
process.p += process.L1CalibFilterTowerJetProducer # for mht
# process.p += process.rawSLHCL1ExtraParticles # for met?

# change the input collections for l1ProtoBufMaker using new UCT + SLHC collections
process.l1ProtoBufMaker.egLabel       = cms.untracked.InputTag("uct2015L1ExtraParticles", "Relaxed")
process.l1ProtoBufMaker.isoEGLabel    = cms.untracked.InputTag("uct2015L1ExtraParticles", "Isolated")
# process.l1ProtoBufMaker.cenJetLabel   = cms.untracked.InputTag("L1CalibFilterTowerJetProducer:Cen8x8")
process.l1ProtoBufMaker.cenJetLabel   = cms.untracked.InputTag("L1TowerJetPUSubtractedProducer:PUSubCen8x8")
process.l1ProtoBufMaker.fwdJetLabel   = cms.untracked.InputTag("l1extraParticles","Forward") 
# process.l1ProtoBufMaker.fwdJetLabel = cms.untracked.InputTag("L1CalibFilterTowerJetProducer:Fwd8x8") # the L1CalibFilterTowerJetProducer has this output commented out - why?
process.l1ProtoBufMaker.tauLabel      = cms.untracked.InputTag("uct2015L1ExtraParticles", "RelaxedTauLeadTrk")
process.l1ProtoBufMaker.isoTauLabel   = cms.untracked.InputTag("uct2015L1ExtraParticles", "IsolatedTau")
process.l1ProtoBufMaker.doReEmulMuons = cms.untracked.bool(True)
process.l1ProtoBufMaker.muonLabel     = cms.untracked.InputTag("valGmtDigis")
# process.l1ProtoBufMaker.muonLabel   = cms.untracked.InputTag("uct2015L1ExtraParticles")
# process.l1ProtoBufMaker.metLabel      = cms.untracked.InputTag("rawSLHCL1ExtraParticles:MET") # can't get this to work
process.l1ProtoBufMaker.metLabel      = cms.untracked.InputTag("uct2015L1ExtraParticles", "MET")
process.l1ProtoBufMaker.mhtLabel      = cms.untracked.InputTag("L1CalibFilterTowerJetProducer:TowerMHT")

# move l1ProtoBufMaker to the end of the path
process.p.remove(process.l1ProtoBufMaker)
process.p += process.l1ProtoBufMaker

# Debug
file = open("l1FallBackNtupleFromGENRAW_cfg.py",'w')
file.write(str(process.dumpPython()))
file.close()

print "Done"

