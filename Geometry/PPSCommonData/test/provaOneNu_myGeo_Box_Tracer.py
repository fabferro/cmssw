# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: SingleMuPt1_cfi.py -s GEN,SIM,DIGI --conditions=MC_72_V1::All --eventcontent RAWSIM -n 10 --no_exec
import FWCore.ParameterSet.Config as cms

process = cms.Process('Prova')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
#process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
#process.load('SimGeneral.MixingModule.mixNoPU_cfi')
#process.load('Configuration.StandardSequences.GeometryRecoDB_cff')

#process.load('Configuration.Geometry.GeometrySimDB_cff')
#process.load('Configuration.Geometry.GeometrySimDB_cff_my')

process.load("Geometry.PPSCommonData.vuoto_e_PPS_Box_cfi")
#process.load("Geometry.CMSCommonData.ecalOnlyGeometryXML_cfi")

process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('Configuration.StandardSequences.VtxSmearedNoSmear_cff')
#process.load('GeneratorInterface.Core.genFilterSummary_cff')
#process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load('Configuration.StandardSequences.Sim_cff')
#process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

# Input source
process.source = cms.Source("EmptySource")

process.options = cms.untracked.PSet(

)

# Production Info
#process.configurationMetadata = cms.untracked.PSet(
#    version = cms.untracked.string('$Revision: 1.19 $'),
#    annotation = cms.untracked.string('provaOneNu_.py nevts:10'),
#    name = cms.untracked.string('Applications')
#)

# Output definition

process.RAWSIMoutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    outputCommands = process.RAWSIMEventContent.outputCommands,
    fileName = cms.untracked.string('provaOneNu_.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('')
    ),
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('generation_step')
    )
)

# Additional output definition

# Other statements


process.MessageLogger.destinations=cms.untracked.vstring('cout'
                                                             ,'cerr'
                                                              ,'G4msg'
                                                              )

process.MessageLogger.categories=cms.untracked.vstring('FwkJob'
                                                            ,'FwkReport'
                                                           ,'FwkSummary'
                                                            ,'Root_NoDictionary'
                                                           ,'TimeReport'
                                                           ,'TimeModule'
                                                            ,'TimeEvent'
                                                           ,'MemoryCheck'
                                                           ,'PhysicsList'
                                                           ,'G4cout'
                                                            ,'G4cerr'                                                            )
     #Configuring the G4msg.log output
process.MessageLogger.G4msg =  cms.untracked.PSet(
        noTimeStamps = cms.untracked.bool(True)
        #First eliminate unneeded output
         ,threshold = cms.untracked.string('INFO')
        ,INFO = cms.untracked.PSet(limit = cms.untracked.int32(0))
         ,FwkReport = cms.untracked.PSet(limit = cms.untracked.int32(0))
         ,FwkSummary = cms.untracked.PSet(limit = cms.untracked.int32(0))
        ,Root_NoDictionary = cms.untracked.PSet(limit = cms.untracked.int32(0))
        ,FwkJob = cms.untracked.PSet(limit = cms.untracked.int32(0))
        ,TimeReport = cms.untracked.PSet(limit = cms.untracked.int32(0))
        ,TimeModule = cms.untracked.PSet(limit = cms.untracked.int32(0))
         ,TimeEvent = cms.untracked.PSet(limit = cms.untracked.int32(0))
         ,MemoryCheck = cms.untracked.PSet(limit = cms.untracked.int32(0))

         ,PhysicsList = cms.untracked.PSet(limit = cms.untracked.int32(-1))
         ,G4cout = cms.untracked.PSet(limit = cms.untracked.int32(-1))
         ,G4cerr = cms.untracked.PSet(limit = cms.untracked.int32(-1))
         )
 
     #Add these 3 lines to put back the summary for timing information at the end of the logfile
     #(needed for TimeReport report)
process.options = cms.untracked.PSet(
         wantSummary = cms.untracked.bool(True)
        )

process.genstepfilter.triggerConditions=cms.vstring("generation_step")
#from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'MC_72_V1::All', '')

process.generator = cms.EDProducer("FlatRandomEGunProducer",
    PGunParameters = cms.PSet(
        MaxE = cms.double(10.1),
        MinE = cms.double(9.99),
        PartID = cms.vint32(12),
        MaxEta = cms.double(3.3),
        MaxPhi = cms.double(1.6),
        MinEta = cms.double(2.8),
        MinPhi = cms.double(1.5)
    ),
    Verbosity = cms.untracked.int32(0),
    psethack = cms.string('single nu E 10'),
    AddAntiParticle = cms.bool(False),
    firstRun = cms.untracked.uint32(1)
)


# Path and EndPath definitions
process.generation_step = cms.Path(process.pgen)
process.simulation_step = cms.Path(process.psim)

process.g4SimHits.NonBeamEvent = cms.bool(False)
process.g4SimHits.Generator.ApplyEtaCuts = cms.bool(False)
#process.g4SimHits.G4EventManagerVerbosity = cms.untracked.int32(0)
#process.g4SimHits.G4StackManagerVerbosity = cms.untracked.int32(0)
#process.g4SimHits.G4TrackingManagerVerbosity = cms.untracked.int32(0)


#process.g4SimHits.Watchers = cms.VPSet(cms.PSet(
#        type = cms.string('G4StepStatistics'),
#         verbose = cms.untracked.bool(True)
#         ))
#process.TFileService = cms.Service("TFileService", 
#       fileName = cms.string("G4StepStatistics.root"),
#       closeFileFast = cms.untracked.bool(True)
#    )

process.g4SimHits.Watchers = cms.VPSet(cms.PSet(
        type = cms.string('SimTracer'),
         verbose = cms.untracked.bool(True)
         ))

#process.digitisation_step = cms.Path(process.pdigi)
#process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RAWSIMoutput_step = cms.EndPath(process.RAWSIMoutput)

# Schedule definition
#process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step,process.simulation_step,process.digitisation_step,process.endjob_step,process.RAWSIMoutput_step)
process.schedule = cms.Schedule(process.generation_step,process.simulation_step,process.endjob_step,process.RAWSIMoutput_step)


# filter all path with the production filter sequence
for path in process.paths:
	getattr(process,path)._seq = process.generator * getattr(process,path)._seq 


