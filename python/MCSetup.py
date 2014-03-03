import FWCore.ParameterSet.Config as cms

# Taken from L1TriggerUpgrade
# Shortened version since we don't need all the l1NtupleProducer, etc
 
def mcSetup(process, stage1=False, stage2=False):

    print "Going to run on MC"

    # adopt to MC
    # don't know why this is here, it's already first... keep just in case
    process.p.remove(process.RawToDigi)
    process.load('Configuration.StandardSequences.RawToDigi_cff')
    process.p.insert(0, process.RawToDigi)

    # process.l1NtupleProducer.simulationSource = cms.InputTag("simulation")

    # bug fix for missing HCAL TPs in MC RAW
    from SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff import HcalTPGCoderULUT
    HcalTPGCoderULUT.LUTGenerationMode = cms.bool(True)
    # process.p.insert(1, process.valHcalTriggerPrimitiveDigis) # Don't think this is needed - already included in ValL1Emulator bit
    process.valRctDigis.hcalDigis = cms.VInputTag(cms.InputTag('valHcalTriggerPrimitiveDigis'))

    if (stage1):    
        print "Adding extra Stage 1 info for MC"
        process.hackHCALMIPs.src = cms.InputTag("valHcalTriggerPrimitiveDigis")
        from L1Trigger.UCT2015.Lut import eg_calib_v1, eg_calib_v4
        process.RCTConfigProducers.eGammaECalScaleFactors = eg_calib_v1
        process.RCTConfigProducers.jetMETECalScaleFactors = eg_calib_v1

    if (stage2):
        print "Adding extra Stage 2 info for MC"
        process.L1CaloTowerProducer.HCALDigis =  cms.InputTag("valHcalTriggerPrimitiveDigis")

