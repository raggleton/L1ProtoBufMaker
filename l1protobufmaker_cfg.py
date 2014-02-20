import FWCore.ParameterSet.Config as cms

#
# Robin Aggleton 18/2/14, robin.aggleton@cern.ch
# 
# Simple config file to produce protocol buffer from input AOD
#

process = cms.Process("L1ProtoBufMaker")

process.load("FWCore.MessageService.MessageLogger_cfi")
# process.MessageLogger.cerr.FwkReport.reportEvery = 1000


# Comment out this module if you don't want debug messages on
# If you do want debug messages, compile with
# scram b -j8 USER_CXXFLAGS="-DEDM_ML_DEBUG"
# may need to do scram b clean first
process.MessageLogger = cms.Service("MessageLogger",
					destinations = cms.untracked.vstring('cerr'),
					categories   = cms.untracked.vstring('*'),
					debugModules = cms.untracked.vstring('*'),
					cerr         = cms.untracked.PSet(
						threshold  = cms.untracked.string('DEBUG') 
			        )
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

process.source = cms.Source("PoolSource",
    # replace with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:../MinBias_TuneZ2star_8TeV-pythia6_PU_S10_START53_V7A-v1_AODSIM.root'
    )
)

# Load L1ProtoBufMaker module, and override some default settings
process.load("L1Trigger.L1ProtoBufMaker.l1protobufmaker_cfi")
process.protoBufMaker.doPUWeights      = cms.untracked.bool(False)
process.protoBufMaker.puMCFile         = cms.untracked.string("PUHistS10.root")
process.protoBufMaker.puDataFile       = cms.untracked.string("") # need filename here!
process.protoBufMaker.puMCHist         = cms.untracked.string("pileup")
process.protoBufMaker.puDataHist       = cms.untracked.string("pileup")
process.protoBufMaker.protobufFilename = cms.untracked.string("myTest.pb")
process.protoBufMaker.doReEmulMuons    = cms.untracked.bool(False)
process.protoBufMaker.muonLabel        = cms.untracked.InputTag("l1extraParticles") #gtDigis

process.p = cms.Path(process.protoBufMaker)
