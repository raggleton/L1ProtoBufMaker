import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:test/MinBias_TuneZ2star_8TeV-pythia6_PU_S10_START53_V7A-v1_AODSIM.root'
    )
)

process.demo = cms.EDAnalyzer('L1ProtoBuffMaker'
)


process.p = cms.Path(process.demo)
