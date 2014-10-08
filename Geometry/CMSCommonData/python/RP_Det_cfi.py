import FWCore.ParameterSet.Config as cms

XMLIdealGeometryESSource = cms.ESSource("XMLIdealGeometryESSource",
    geomXMLFiles = cms.vstring('Geometry/CMSCommonData/data/materials.xml', 
        'Geometry/CMSCommonData/data/rotations.xml', 
        'Geometry/CMSCommonData/data/normal/cmsextent.xml', 
        'Geometry/CMSCommonData/data/cms.xml', 
        'Geometry/CMSCommonData/data/cmsMother.xml', 
	'Geometry/TotemRPData/data/TotemRPGlobal.xml',
        'Geometry/TotemRPData/data/RP_Stations_Assembly.xml',
#
       'Geometry/TotemRPData/data/RP_Device.xml',
       'Geometry/TotemRPData/data/RP_Horizontal_Device.xml',
       'Geometry/TotemRPData/data/RP_Vertical_Device.xml',
        'Geometry/TotemRPData/data/RP_220_Right_Station.xml',

        'Geometry/TotemRPData/data/RP_Box.xml',
        'Geometry/TotemRPData/data/RP_Transformations.xml',
        'Geometry/TotemRPData/data/RP_Garage/RP_Dist_Beam_Cent.xml',
        'Geometry/TotemRPData/data/RP_Materials.xml',


        'Geometry/TotemRPData/data/RP_Detectors_Assembly.xml',
        'Geometry/TotemRPData/data/RP_Hybrid.xml',



#        'Geometry/CMSCommonData/data/FieldParameters.xml'
),
    rootNodeName = cms.string('cms:OCMS')
)

