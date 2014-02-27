import FWCore.ParameterSet.Config as cms

#
# Robin Aggleton 18/2/14, robin.aggleton@cern.ch
# 
# Simple config file to produce protocol buffer from input GEN+SIM+DIGI+RAW
#

# process = cms.Process("L1ProtoBufMaker")

# process.MessageLogger.cerr.FwkReport.reportEvery = 1000

# If you do want debug messages, compile with
# scram b -j8 USER_CXXFLAGS="-DEDM_ML_DEBUG"
# may need to do scram b clean first
# process.MessageLogger = cms.Service("MessageLogger",
# 					destinations = cms.untracked.vstring('cout'),
# 					categories   = cms.untracked.vstring('*'),
# 					debugModules = cms.untracked.vstring('*'),
# 					cout         = cms.untracked.PSet(
# 						threshold = cms.untracked.string('DEBUG') 
# 						)
# )
# 
from UserCode.L1TriggerUpgrade.l1CurrentNtupleFromRAW import *

from L1Trigger.L1ProtoBufMaker.MCSetup import *
mcSetup(process, False, False)

# from L1Trigger.Configuration.ValL1Emulator_cff import *

# Remove everything except the re emulation producer (ValL1Emulator) & l1Extra particle producer (l1extraParticles)
process.p.remove(process.l1NtupleProducer)
process.p.remove(process.l1GtTriggerMenuLite)
process.p.remove(process.l1MenuTreeProducer)
# process.p.remove(process.l1extraParticles)
process.p.remove(process.l1ExtraTreeProducer)
process.p.remove(process.L1EmulatorTree)
process.p.insert(2,process.ValL1Emulator)

# Add in l1ProtoBufMaker module
process.load("L1Trigger.L1ProtoBufMaker.l1ProtoBufMaker_cfi")
process.l1ProtoBufMaker.doPUWeights      = cms.untracked.bool(False)
process.l1ProtoBufMaker.puMCFile         = cms.untracked.string("data/PUHistS10.root")
process.l1ProtoBufMaker.puDataFile       = cms.untracked.string("") # need filename here!
process.l1ProtoBufMaker.puMCHist         = cms.untracked.string("pileup")
process.l1ProtoBufMaker.puDataHist       = cms.untracked.string("pileup")
process.l1ProtoBufMaker.isoTauLabel      = cms.untracked.InputTag("none")
process.l1ProtoBufMaker.doReEmulMuons    = cms.untracked.bool(True)
process.l1ProtoBufMaker.muonLabel        = cms.untracked.InputTag("valGmtDigis") # valGmtDigis | l1extraParticles
# process.l1ProtoBufMaker.menuFilename     = cms.string("myTest.txt")
process.l1ProtoBufMaker.protobufFilename = cms.untracked.string("myTest.pb")
process.p *= process.l1ProtoBufMaker

# job options
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
    )

# process.TFileService.fileName = cms.string('l1Tree_test.root') 
process.p.remove(process.TFileService)

readFiles.extend( [
        '/store/mc/Summer12/Neutrino_Pt2to20_gun/GEN-SIM-DIGI-RAW/UpgradeL1TDR-PU35_POSTLS161_V12-v2/00000/168A4B24-C53C-E211-A370-003048C68AA6.root'
        ] )

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

