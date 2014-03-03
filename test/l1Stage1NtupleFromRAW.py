import FWCore.ParameterSet.Config as cms

from L1Trigger.L1ProtoBufMaker.l1Stage1NtupleFromRAW import *

print "Going to run on data"

# job options
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
                )

readFiles.extend( [
    'file:/gpfs_phys/storm/cms/data/Run2012C/ZeroBias/RAW/v1/000/198/588/1ADBB979-1ACA-E111-AA51-003048D3C932.root'
    ] )

#process.MessageLogger.cerr.INFO.limit = cms.untracked.int32(10000)

# Debug stuff
file = open("l1Stage1NtupleFromRAW_cfg.py",'w')
file.write(str(process.dumpPython()))
file.close()
