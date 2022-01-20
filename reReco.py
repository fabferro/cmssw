
# Auto generated configuration file
# using:
# Revision: 1.19
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras
process = cms.Process('fab',eras.Run3)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/eos/cms/store/relval/CMSSW_12_2_0_pre2/RelValGluGluTo2Jets_14TeV/GEN-SIM-RECO/122X_mcRun3_2021_realistic_Candidate_2021_11_19_14_14_21_PPS_DD4HEPLHCTransportTrue-v1/2580000/b1482de9-f19d-4cb7-9a33-e76349f430e6.root'),
    secondaryFileNames = cms.untracked.vstring()
)

# Track memory leaks
process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",ignoreTotal = cms.untracked.int32(1) )

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

# Output definition

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('out.root'),
    outputCommands = cms.untracked.vstring("drop *","keep SimVertexs_g4SimHits_*_*","keep CTPPS*_*_*_*","keep *_*RP*_*_*")
)


# Additional output definition
# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2021_realistic', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '113X_mcRun3_2021_realistic_Candidate_2021_04_06_19_59_53', '')

# Path and EndPath definitions
#process.raw2digi_step = cms.Path(process.RawToDigi)
#process.reco_step = cms.Path(process.reconstruction)
#process.raw2digi_step = cms.Path(process.ctppsRawToDigi) # to avoid full CMS digitization
process.reco_step = cms.Path(process.ctppsPixelLocalReconstruction) # to avoid full CMS reconstruction
process.endjob_step = cms.EndPath(process.endOfProcess)
process.output_step = cms.EndPath(process.output)

# Schedule definition
process.schedule = cms.Schedule(process.reco_step,process.endjob_step,process.output_step)

# filter all path with the production filter sequence
for path in process.paths:
    #  getattr(process,path)._seq = process.ProductionFilterSequence * getattr(process,path)._seq
    getattr(process,path)._seq = getattr(process,path)._seq
