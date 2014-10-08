import FWCore.ParameterSet.Config as cms

XMLIdealGeometryESSource = cms.ESSource("XMLIdealGeometryESSource",
                                        geomXMLFiles = cms.vstring('Geometry/CMSCommonData/data/materials.xml', 
                                                                   'Geometry/CMSCommonData/data/rotations.xml', 
                                                                   'Geometry/CMSCommonData/data/normal/cmsextent.xml', 
                                                                   'Geometry/CMSCommonData/data/cms.xml', 

      'Geometry/CMSCommonData/data/cmsMother.xml', 
	'Geometry/TotemRPData/data/TotemRPGlobal.xml',
                                                                 
        'Geometry/TotemRPData/data/PPS_Prova_Assembly.xml',
#'Geometry/TrackerCommonData/data/trackermaterial.xml',
'Geometry/PPSTrackerData/data/ppstrackerMaterials.xml',

                                                                   'Geometry/PPSTrackerData/data/PPSTrackerModule.xml',


#                                                                   'Geometry/CMSCommonData/data/cmsMotherPilot.xml'






 #       'Geometry/CMSCommonData/data/FieldParameters.xml'
),
    rootNodeName = cms.string('cms:OCMS')
)

