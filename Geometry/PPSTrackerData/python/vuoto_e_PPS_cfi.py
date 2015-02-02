import FWCore.ParameterSet.Config as cms

XMLIdealGeometryESSource = cms.ESSource("XMLIdealGeometryESSource",
                          geomXMLFiles = cms.vstring('Geometry/CMSCommonData/data/materials.xml', 
                                                     'Geometry/CMSCommonData/data/rotations.xml', 
                                                     'Geometry/CMSCommonData/data/normal/cmsextent.xml', 
                                                     'Geometry/CMSCommonData/data/cms.xml', 
 
                                                     
                                                     'Geometry/PPSTrackerData/data/cmsMother.xml', 
                                                     'Geometry/PPSTrackerData/data/TotemRPGlobal.xml',
                                                     
                                                     'Geometry/PPSTrackerData/data/PPS_Prova_Assembly.xml',

                                                     'Geometry/PPSTrackerData/data/ppstrackerMaterials.xml',
                                                     
                                                     'Geometry/PPSTrackerData/data/PPSTrackerModule.xml',
                                                     
 
                                                     ),
                                        rootNodeName = cms.string('cms:OCMS')
                                        )

