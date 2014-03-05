import FWCore.ParameterSet.Config as cms

from L1Trigger.L1ProtoBufMaker.l1CurrentNtupleFromRAW import *

from L1Trigger.L1ProtoBufMaker.MCSetup import *
mcSetup(process, False, False)

# job options
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
    )

process.source.fileNames = cms.untracked.vstring('/store/mc/Summer12/Neutrino_Pt2to20_gun/GEN-SIM-DIGI-RAW/UpgradeL1TDR-PU35_POSTLS161_V12-v2/00000/168A4B24-C53C-E211-A370-003048C68AA6.root')

# Debug stuff
process.output = cms.OutputModule(
   "PoolOutputModule",
   outputCommands = cms.untracked.vstring('keep *'),
   fileName = cms.untracked.string('output.root')
   )
process.e = cms.EndPath(process.output)

file = open("l1CurrentNtupleFromGENRAW_cfg.py",'w')
file.write(str(process.dumpPython()))
file.close()

#process.load("L1TriggerConfig.GctConfigProducers.l1GctConfigDump_cfi")
#process.MessageLogger.cout.placeholder = cms.untracked.bool(False)
#process.MessageLogger.cout.threshold = cms.untracked.string('DEBUG')
#process.MessageLogger.debugModules = cms.untracked.vstring('l1GctConfigDump')
#process.p += ( process.l1GctConfigDump )
