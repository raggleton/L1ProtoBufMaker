import FWCore.ParameterSet.Config as cms

#
# Robin Aggleton 18/2/14, robin.aggleton@cern.ch
# 
# Simple config file to produce protocol buffer from input AOD
#

process = cms.Process("L1ProtoBufMaker")

process.load("FWCore.MessageService.MessageLogger_cfi")
# process.MessageLogger.cerr.FwkReport.reportEvery = 1000

# If you do want debug messages, compile with
# scram b -j8 USER_CXXFLAGS="-DEDM_ML_DEBUG"
# may need to do scram b clean first
process.MessageLogger = cms.Service("MessageLogger",
					destinations = cms.untracked.vstring('cout'),
					categories   = cms.untracked.vstring('*'),
					debugModules = cms.untracked.vstring('*'),
					cout         = cms.untracked.PSet(
						threshold = cms.untracked.string('DEBUG') 
						)
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

process.source = cms.Source("PoolSource",
    # replace with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:../MinBias_TuneZ2star_8TeV-pythia6_PU_S10_START53_V7A-v1_AODSIM.root'
        # 'file:../MinBias_TuneZ2star_8TeV-pythia6_GEN-SIM_START50_V13-v3.root'
        # 'file:../ZeroBias1_Run2012A-v1_RAW.root'
    )
)

# analysis path
# import of standard configurations
# process.load('Configuration/StandardSequences/Services_cff')
# process.load('Configuration/StandardSequences/GeometryIdeal_cff')
# process.load('Configuration/StandardSequences/MagneticField_38T_cff')
# process.load('Configuration/StandardSequences/SimL1Emulator_cff')
# process.load("Configuration.StandardSequences.RawToDigi_cff")
# process.load('Configuration/StandardSequences/EndOfProcess_cff')
# process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
# process.load('Configuration/EventContent/EventContent_cff')
# process.load('Configuration.StandardSequences.Reconstruction_cff')

# process.GlobalTag.globaltag = 'GR_R_52_V7::All'

# This one produces extra l1extra particles from digis ??
# process.load("L1Trigger.Configuration.L1Extra_cff")

# Load L1ProtoBufMaker module, and override some default settings
process.load("L1Trigger.L1ProtoBufMaker.l1protobufmaker_cfi")
process.protoBufMaker.doPUWeights      = cms.untracked.bool(False)
process.protoBufMaker.puMCFile         = cms.untracked.string("data/PUHistS10.root")
process.protoBufMaker.puDataFile       = cms.untracked.string("") # need filename here!
process.protoBufMaker.puMCHist         = cms.untracked.string("pileup")
process.protoBufMaker.puDataHist       = cms.untracked.string("pileup")
process.protoBufMaker.isoTauJetLabel   = cms.untracked.InputTag("none")
process.protoBufMaker.doReEmulMuons    = cms.untracked.bool(False)
process.protoBufMaker.muonLabel        = cms.untracked.InputTag("l1extraParticles") # gtDigis | l1ExtraParticles
process.protoBufMaker.protobufFilename = cms.untracked.string("myTest.pb")

process.p = cms.Path(
	# process.RawToDigi +
	process.protoBufMaker
	)
