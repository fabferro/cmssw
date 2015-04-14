import FWCore.ParameterSet.Config as cms

from RecoBTag.SecondaryVertex.combinedSecondaryVertexCommon_cfi import *

combinedSecondaryVertexSoftLepton = cms.ESProducer("CombinedSecondaryVertexSoftLeptonESProducer",
	combinedSecondaryVertexCommon,
	useCategories = cms.bool(True),
	calibrationRecords = cms.vstring(
		'CombinedSVRecoVertexNoSoftLepton', 
		'CombinedSVPseudoVertexNoSoftLepton', 
		'CombinedSVNoVertexNoSoftLepton',
		'CombinedSVRecoVertexSoftMuon', 
		'CombinedSVPseudoVertexSoftMuon', 
		'CombinedSVNoVertexSoftMuon',
		'CombinedSVRecoVertexSoftElectron', 
		'CombinedSVPseudoVertexSoftElectron', 
		'CombinedSVNoVertexSoftElectron'),
	categoryVariableName = cms.string('vertexLeptonCategory')
)
combinedSecondaryVertexSoftLepton.trackSelection.qualityClass = cms.string('any')
combinedSecondaryVertexSoftLepton.trackPseudoSelection.qualityClass = cms.string('any')
combinedSecondaryVertexSoftLepton.trackMultiplicityMin = cms.uint32(2)

