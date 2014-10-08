import FWCore.ParameterSet.Config as cms

XMLIdealGeometryESSource = cms.ESSource("XMLIdealGeometryESSource",
                                        geomXMLFiles = cms.vstring('Geometry/CMSCommonData/data/materials.xml', 
                                                                   'Geometry/CMSCommonData/data/rotations.xml', 
                                                                   'Geometry/CMSCommonData/data/normal/cmsextent.xml', 
                                                                   'Geometry/CMSCommonData/data/cms.xml', 

      'Geometry/CMSCommonData/data/cmsMother.xml', 
	'Geometry/TotemRPData/data/TotemRPGlobal.xml',
                                                                 
        'Geometry/TotemRPData/data/Pixel_Prova_Assembly.xml',
'Geometry/TrackerCommonData/data/trackermaterial.xml',
'Geometry/TrackerCommonData/data/Pilot/pixfwdMaterials.xml',

                                                                   'Geometry/TrackerCommonData/data/Pilot/pixfwdPilotBlade.xml',


#                                                                   'Geometry/CMSCommonData/data/cmsMotherPilot.xml'






 #       'Geometry/CMSCommonData/data/FieldParameters.xml'
),
    rootNodeName = cms.string('cms:OCMS')
)

