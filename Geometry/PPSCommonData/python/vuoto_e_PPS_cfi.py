import FWCore.ParameterSet.Config as cms

XMLIdealGeometryESSource = cms.ESSource("XMLIdealGeometryESSource",
                          geomXMLFiles = cms.vstring('Geometry/CMSCommonData/data/materials.xml', 
                                                     'Geometry/CMSCommonData/data/rotations.xml', 
                                                     'Geometry/CMSCommonData/data/normal/cmsextent.xml', 
                                                     'Geometry/CMSCommonData/data/cms.xml', 
 
                                                     
                                                     'Geometry/PPSCommonData/data/cmsMother.xml', 
                                                     'Geometry/PPSCommonData/data/TotemRPGlobal.xml',
                                                     
                                                     'Geometry/PPSCommonData/data/PPS_Prova_Assembly.xml',

                                                     'Geometry/PPSCommonData/data/ppstrackerMaterials.xml',
                                                     
                                                     'Geometry/PPSCommonData/data/PPSTrackerModule.xml',
                                                     
 
                                                     ),
                                        rootNodeName = cms.string('cms:OCMS')
                                        )

