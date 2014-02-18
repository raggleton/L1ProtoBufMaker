import FWCore.ParameterSet.Config as cms

#
# Robin Aggleton 18/2/14, robin.aggleton@cern.ch
# 
# Simple config file to produce protocol buffer from input AOD
#

process = cms.Process("L1ProtoBufMaker")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:../MinBias_TuneZ2star_8TeV-pythia6_PU_S10_START53_V7A-v1_AODSIM.root'
    )
)

process.load("L1Trigger.L1ProtoBufMaker.l1protobufmaker_cfi")
process.protoBufMaker.doPUWeights      = cms.untracked.bool(True)
process.protoBufMaker.puMCFile         = cms.untracked.string("PUHistS10.root")
process.protoBufMaker.puDataFile       = cms.untracked.string("") # need filename here!
process.protoBufMaker.puMCHist         = cms.untracked.string("pileup")
process.protoBufMaker.puDataHist       = cms.untracked.string("pileup")
process.protoBufMaker.protobufFilename = cms.untracked.string("myTest.pb")

process.p = cms.Path(process.protoBufMaker)
