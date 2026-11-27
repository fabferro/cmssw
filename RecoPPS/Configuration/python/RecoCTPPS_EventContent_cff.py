import FWCore.ParameterSet.Config as cms

RecoCTPPSAOD = cms.PSet(
  outputCommands = cms.untracked.vstring(
    # trigger counters
    'keep TotemTriggerCounters_totemTriggerRawToDigi_*_*',

    # tracking strip
    'keep TotemFEDInfos_totemRPRawToDigi_*_*',
    'keep TotemRPDigi_totemRPRawToDigi_*_*',
    'keep TotemVFATStatus_totemRPRawToDigi_*_*',
    'keep TotemRPCluster_totemRPClusterProducer_*_*',
    'keep TotemRPRecHit_totemRPRecHitProducer_*_*',
    'keep TotemRPUVPattern_totemRPUVPatternFinder_*_*',
    'keep TotemRPLocalTrack_totemRPLocalTrackFitter_*_*',

    # totem T2
    'keep TotemFEDInfos_totemT2Digis_*_*',
    'keep TotemT2Digi_totemT2Digis_*_*',
    'keep TotemVFATStatus_totemT2Digis_*_*',

    # timing diamonds
    'keep TotemFEDInfos_ctppsDiamondRawToDigi_*_*',
    'keep CTPPSDiamondDigi_ctppsDiamondRawToDigi_*_*',
    'keep TotemVFATStatus_ctppsDiamondRawToDigi_*_*',
    'keep CTPPSDiamondRecHits_ctppsDiamondRecHits_*_*',
    'keep CTPPSDiamondLocalTracks_ctppsDiamondLocalTracks_*_*',

    #diamond sampic
    'keep TotemTimingLocalTracks_diamondSampicLocalTracks_*_*',

    # TOTEM timing
    'keep TotemTimingDigi_totemTimingRawToDigi_*_*',
    'keep TotemTimingRecHits_totemTimingRecHits_*_*',
    'keep TotemTimingLocalTracks_totemTimingLocalTracks_*_*',

    # tracking pixels
    'keep CTPPSPixelDigi_ctppsPixelDigis_*_*',
    'keep CTPPSPixelDataError_ctppsPixelDigis_*_*',
    'keep CTPPSPixelCluster_ctppsPixelClusters_*_*',
    'keep CTPPSPixelRecHits_ctppsPixelRecHits_*_*',
    'keep CTPPSPixelLocalTracks_ctppsPixelLocalTracks_*_*',

    # CTPPS common
    'keep CTPPSLocalTrackLites_ctppsLocalTrackLiteProducer_*_*',
    'keep recoForwardProtons_ctppsProtons_*_*',
  )
)


RecoCTPPSRECO = cms.PSet(
  outputCommands = cms.untracked.vstring()
)


RecoCTPPSFEVT = cms.PSet(
  outputCommands = cms.untracked.vstring()
)

RecoCTPPSRECO.outputCommands.extend(RecoCTPPSAOD.outputCommands)
RecoCTPPSFEVT.outputCommands.extend(RecoCTPPSRECO.outputCommands)
