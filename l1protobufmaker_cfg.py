import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:../MinBias_TuneZ2star_8TeV-pythia6_PU_S10_START53_V7A-v1_AODSIM.root'
    )
)

from L1Trigger.L1ProtoBufMaker.l1protobufmaker_cfi import *

process.protoBufMaker = cms.EDAnalyzer('L1ProtoBufMaker',
	nonIsoEmLabel = cms.untracked.InputTag("l1extraParticles:NonIsolated"),
	isoEmLabel = cms.untracked.InputTag("l1extraParticles:Isolated"),
	tauJetLabel = cms.untracked.InputTag("l1extraParticles:Tau"),
	cenJetLabel = cms.untracked.InputTag("l1extraParticles:Central"),
	fwdJetLabel = cms.untracked.InputTag("l1extraParticles:Forward"),
	muonLabel = cms.untracked.InputTag("l1extraParticles"),
	metLabel = cms.untracked.InputTag("l1extraParticles:MET"),
	mhtLabel = cms.untracked.InputTag("l1extraParticles:MHT"),
	hfRingsLabel = cms.untracked.InputTag("l1extraParticles"),
	menuFilename = cms.string("/storage/phrca/L1Menu/CMSSW_6_0_1/src/L1Trigger/MenuGeneration/test/L1Menu_input_std.txt"),
	protobufFilename = cms.string("test.pb")
)

process.p = cms.Path(process.protoBufMaker)
