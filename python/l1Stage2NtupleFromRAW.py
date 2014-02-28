import FWCore.ParameterSet.Config as cms

# import master L1 Ntuple config
from L1Trigger.L1ProtoBufMaker.l1CurrentNtupleFromRAW import *

print "Adding Stage 2 algorithms"

# add full upgrade objects
process.load('UserCode.L1TriggerUpgrade.L1Upgrade_EGTau_cff')
process.load('UserCode.L1TriggerUpgrade.L1Upgrade_Jet_cff')
process.load('UserCode.L1TriggerUpgrade.l1UpgradeMuonIsolator_cfi')

# change the input collections for l1ProtoBufMaker using new SLHC collections
# (matches UserCode.L1TriggerUpgrade.l1ExtraUpgradeTreeProducer_cfi)
process.l1ProtoBufMaker.egLabel       = cms.untracked.InputTag("SLHCL1ExtraParticlesNewClustering","EGamma")
process.l1ProtoBufMaker.isoEGLabel    = cms.untracked.InputTag("SLHCL1ExtraParticlesNewClustering","IsoEGamma")
# process.l1ProtoBufMaker.cenJetLabel   = cms.untracked.InputTag("L1CalibFilterTowerJetProducer:Cen8x8")
process.l1ProtoBufMaker.cenJetLabel   = cms.untracked.InputTag("L1TowerJetPUSubtractedProducer:PUSubCen8x8")
# process.l1ProtoBufMaker.fwdJetLabel   = cms.untracked.InputTag("L1CalibFilterTowerJetProducer:Fwd8x8") # the L1CalibFilterTowerJetProducer has this output commented out - why?
process.l1ProtoBufMaker.fwdJetLabel   = cms.untracked.InputTag("l1extraParticles","Forward") 
process.l1ProtoBufMaker.tauLabel      = cms.untracked.InputTag("SLHCL1ExtraParticles:Taus")
process.l1ProtoBufMaker.isoTauLabel   = cms.untracked.InputTag("SLHCL1ExtraParticles:IsoTaus")
process.l1ProtoBufMaker.doReEmulMuons = cms.untracked.bool(True)
process.l1ProtoBufMaker.muonLabel     = cms.untracked.InputTag("valGmtDigis")
# process.l1ProtoBufMaker.muonLabel   = cms.untracked.InputTag("l1UpgradeMuonIsolator") # don't use this as we use re-emul muons
process.l1ProtoBufMaker.metLabel      = cms.untracked.InputTag("rawSLHCL1ExtraParticles:MET")
process.l1ProtoBufMaker.mhtLabel      = cms.untracked.InputTag("L1CalibFilterTowerJetProducer:TowerMHT")

# add upgrade algorithms to the path before l1ProtoBufMaker
process.p += process.SLHCCaloTrigger
process.p += process.l1UpgradeMuonIsolator

# move l1ProtoBufMaker to the end of the path
process.p.remove(process.l1ProtoBufMaker)
process.p += process.l1ProtoBufMaker

# Debug
file = open("l1Stage2NtupleFromGENRAW_cfg.py",'w')
file.write(str(process.dumpPython()))
file.close()

print "Done"
