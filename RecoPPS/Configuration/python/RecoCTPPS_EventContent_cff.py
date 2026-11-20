import FWCore.ParameterSet.Config as cms

RecoCTPPSAOD = cms.PSet(
  outputCommands = cms.untracked.vstring(
    # trigger counters
    'keep TotemTriggerCounters_totemTriggerRawToDigi_*_*',

    # tracking strip
    'keep TotemFEDInfos_totemRPRawToDigi_*_*',
    'keep std::vector<io_v1::TotemRPDigi>_totemRPRawToDigi_*_*',
    'keep TotemVFATStatusedmDetSetVector_totemRPRawToDigi_*_*',
    'keep std::vector<io_v1::TotemRPCluster>_totemRPClusterProducer_*_*',
    'keep std::vector<io_v1::TotemRPRecHit>_totemRPRecHitProducer_*_*',
    'keep std::vector<io_v1::TotemRPUVPattern>_totemRPUVPatternFinder_*_*',
    'keep std::vector<io_v1::TotemRPLocalTrack>_totemRPLocalTrackFitter_*_*',

    # totem T2
    'keep TotemFEDInfos_totemT2Digis_*_*',
    'keep std::vector<io_v1::TotemT2Digi>_totemT2Digis_*_*',
    'keep TotemVFATStatusedmDetSetVector_totemT2Digis_*_*',

    # timing diamonds
    'keep TotemFEDInfos_ctppsDiamondRawToDigi_*_*',
    'keep std::vector<io_v1::CTPPSDiamondDigi>_ctppsDiamondRawToDigi_*_*',
    'keep TotemVFATStatusedmDetSetVector_ctppsDiamondRawToDigi_*_*',
    'keep CTPPSDiamondRecHits_ctppsDiamondRecHits_*_*',
    'keep CTPPSDiamondLocalTracks_ctppsDiamondLocalTracks_*_*',

    #diamond sampic
    'keep TotemTimingLocalTracks_diamondSampicLocalTracks_*_*',

    # TOTEM timing
    'keep std::vector<io_v1::TotemTimingDigi>_totemTimingRawToDigi_*_*',
    'keep TotemTimingRecHits_totemTimingRecHits_*_*',
    'keep TotemTimingLocalTracks_totemTimingLocalTracks_*_*',

    # tracking pixels
    'keep std::vector<io_v1::CTPPSPixelDigi>_ctppsPixelDigis_*_*',
    'keep std::vector<io_v1::CTPPSPixelDataError>_ctppsPixelDigis_*_*',
    'keep std::vector<io_v1::CTPPSPixelCluster>_ctppsPixelClusters_*_*',
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
