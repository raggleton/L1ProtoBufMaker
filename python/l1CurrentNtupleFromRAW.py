import FWCore.ParameterSet.Config as cms

print "Adding current L1 information"

# make L1 ntuples from RAW+RECO
# Stripped down version of L1TriggerUpgrade/python/l1CurrentNtupleFromRAW.py
# The L1ProtoBufMaker module is added in in the individual data/MC config files

process = cms.Process("L1PROTOBUF")

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
process.load('Configuration/StandardSequences/L1HwVal_cff')
process.load("Configuration.StandardSequences.RawToDigi_Data_cff")
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration/StandardSequences/EndOfProcess_cff')
process.load('Configuration.Geometry.GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_AutoFromDBCurrent_cff')
# process.load('Configuration.EventContent.EventContent_cff')

# process.load('Configuration.StandardSequences.MagneticField_38T_cff')
# process.load('IOMC.EventVertexGenerators.VtxSmearedGauss_cfi')

# process.load('Configuration.Geometry.GeometryExtendedPhase2TkBE5DReco_cff')
# process.load('Configuration.Geometry.GeometryExtendedPhase2TkBE5D_cff')
# process.load('Geometry.TrackerGeometryBuilder.StackedTrackerGeometry_cfi')
# process.load("JetMETCorrections.Configuration.DefaultJEC_cff")

# L1 raw to digi options
process.gctDigis.numberOfGctSamplesToUnpack = cms.uint32(5)
process.l1extraParticles.centralBxOnly = cms.bool(False)

# L1Extra particle producer
import L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi

# L1 emulator 
from L1Trigger.Configuration.ValL1Emulator_cff import *

# remove RPC TT because it seg faults
process.ValL1Emulator.remove(process.valRpcTechTrigDigis)

# re-wire emulators to include upstream modules in L1Extra
#process.valGctDigis.inputLabel = 'valRctDigis'
process.valGmtDigis.DTCandidates = cms.InputTag('valDttfDigis')
process.valGmtDigis.RPCbCandidates = cms.InputTag('valRpcTriggerDigis', 'RPCb')
process.valGmtDigis.RPCfCandidates = cms.InputTag('valRpcTriggerDigis', 'RPCf')

# global tag etc
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'POSTLS161_V12::All'
# process.GlobalTag.globaltag = 'POSTLS261_V3::All'

# GCT re-emulation
process.GlobalTag.toGet = cms.VPSet(
    cms.PSet(record = cms.string("L1GctJetFinderParamsRcd"),
             tag = cms.string("L1GctJetFinderParams_GCTPhysics_2012_04_27_JetSeedThresh5GeV_mc"),
             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_L1T")
             )
)

process.GlobalTag.toGet = cms.VPSet(
    cms.PSet(record = cms.string("L1GctChannelMaskRcd"),
             tag = cms.string("L1GctChannelMask_AllEnergySumsMaskedFromHF_jetCentresToEta3Allowed_mc"),
             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_L1T")
             )
)

# DTTF re-emulation
process.GlobalTag.toGet = cms.VPSet(
    cms.PSet(record = cms.string("L1MuDTTFParametersRcd"),
             tag = cms.string("L1MuDTTFParameters_dttf12_TSC_03_csc_col_mc"),
             connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_L1T")
             )
)

# RPC re-emulation
process.load("L1TriggerConfig.RPCTriggerConfig.RPCBxOrConfig_cff")
process.load("L1Trigger.RPCTrigger.RPCConeConfig_cff")
process.load("L1Trigger.RPCTrigger.l1RpcEmulDigis_cfi")
process.l1RPCBxOrConfig.lastBX = cms.int32(0)
process.l1RPCBxOrConfig.firstBX = cms.int32(0)

# Add in l1ProtoBufMaker module
process.load("L1Trigger.L1ProtoBufMaker.l1ProtoBufMaker_cfi")
process.l1ProtoBufMaker.doPUWeights      = cms.untracked.bool(False)
process.l1ProtoBufMaker.puMCFile         = cms.untracked.string("data/PUHistS10.root")
process.l1ProtoBufMaker.puDataFile       = cms.untracked.string("") # need filename here!
process.l1ProtoBufMaker.isoTauJetLabel   = cms.untracked.InputTag("none")
process.l1ProtoBufMaker.doReEmulMuons    = cms.untracked.bool(True)
process.l1ProtoBufMaker.muonLabel        = cms.untracked.InputTag("valGmtDigis") # valGmtDigis | l1extraParticles
# process.l1ProtoBufMaker.menuFilename     = cms.string("myTest.txt")
process.l1ProtoBufMaker.protobufFilename = cms.untracked.string("myTest.pb")

# path
process.p = cms.Path(
    process.RawToDigi
    + process.ValL1Emulator
    + process.l1extraParticles
    + process.l1ProtoBufMaker
)

# job options
process.options = cms.untracked.PSet( Rethrow = cms.untracked.vstring('ProductNotFound') )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
    )

readFiles = cms.untracked.vstring('/store/mc/Summer12/Neutrino_Pt2to20_gun/GEN-SIM-DIGI-RAW/UpgradeL1TDR-PU35_POSTLS161_V12-v2/00000/168A4B24-C53C-E211-A370-003048C68AA6.root')
secFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource",
                             fileNames = readFiles,
                             secondaryFileNames = secFiles
                             )

# Debug stuff
process.output = cms.OutputModule(
   "PoolOutputModule",
   outputCommands = cms.untracked.vstring('keep *'),
   fileName = cms.untracked.string('output.root')
   )
process.e = cms.EndPath(process.output)

file = open("l1Stage1NtupleFromGENRAW_cfg.py",'w')
file.write(str(process.dumpPython()))
file.close()

print "Done"
