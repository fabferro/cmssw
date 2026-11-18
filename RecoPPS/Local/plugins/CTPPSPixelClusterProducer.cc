/**********************************************************************
 *
 * Author: F.Ferro fabrizio.ferro@ge.infn.it - INFN Genova - 2017
 *
 **********************************************************************/

#include <memory>
#include <vector>

#include "CondFormats/DataRecord/interface/CTPPSPixelAnalysisMaskRcd.h"
#include "CondFormats/DataRecord/interface/CTPPSPixelDAQMappingRcd.h"
#include "CondFormats/DataRecord/interface/CTPPSPixelGainCalibrationsRcd.h"
#include "CondFormats/PPSObjects/interface/CTPPSPixelAnalysisMask.h"
#include "CondFormats/PPSObjects/interface/CTPPSPixelDAQMapping.h"
#include "CondFormats/PPSObjects/interface/CTPPSPixelGainCalibrations.h"
#include "DataFormats/CTPPSDetId/interface/CTPPSPixelDetId.h"
#include "DataFormats/CTPPSDigi/interface/CTPPSPixelDigi.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelCluster.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/EDPutToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoPPS/Local/interface/RPixDetClusterizer.h"

class CTPPSPixelClusterProducer : public edm::stream::EDProducer<> {
public:
  explicit CTPPSPixelClusterProducer(const edm::ParameterSet &param);

  ~CTPPSPixelClusterProducer() override = default;

  void produce(edm::Event &, const edm::EventSetup &) override;
  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  const edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelDigi>> tokenCTPPSPixelDigi_;
  const edm::EDPutTokenT<std::vector<CTPPSPixelCluster>> tokenCTPPSPixelCluster_;
  const edm::ESGetToken<CTPPSPixelAnalysisMask, CTPPSPixelAnalysisMaskRcd> tokenCTPPSPixelAnalysisMask_;
  const edm::ESGetToken<CTPPSPixelGainCalibrations, CTPPSPixelGainCalibrationsRcd> tokenGainCalib_;
  const int verbosity_;
  RPixDetClusterizer clusterizer_;

  void run(const edm::DetSetVector<CTPPSPixelDigi> &input,
           std::vector<CTPPSPixelCluster> &output,
           const CTPPSPixelAnalysisMask &mask,
           const CTPPSPixelGainCalibrations &gainCalibration);
};

CTPPSPixelClusterProducer::CTPPSPixelClusterProducer(const edm::ParameterSet &conf)
    : tokenCTPPSPixelDigi_(consumes<edm::DetSetVector<CTPPSPixelDigi>>(conf.getParameter<edm::InputTag>("tag"))),
      tokenCTPPSPixelCluster_(produces<std::vector<CTPPSPixelCluster>>()),
      tokenCTPPSPixelAnalysisMask_(esConsumes()),
      tokenGainCalib_(esConsumes()),
      verbosity_(conf.getUntrackedParameter<int>("RPixVerbosity")),
      clusterizer_(conf) {}

void CTPPSPixelClusterProducer::fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.addUntracked<int>("RPixVerbosity", 0);
  desc.add<edm::InputTag>("tag", edm::InputTag("ctppsPixelDigis"));
  desc.add<int>("SeedADCThreshold", 2);
  desc.add<int>("ADCThreshold", 2);
  desc.add<double>("ElectronADCGain", 135.0);
  desc.add<int>("VCaltoElectronGain", 50);
  desc.add<int>("VCaltoElectronOffset", -411);
  desc.add<bool>("doSingleCalibration", false);
  descriptions.add("ctppsPixelClusters", desc);
}

void CTPPSPixelClusterProducer::produce(edm::Event &event, const edm::EventSetup &setup) {
  // get inputs
  edm::DetSetVector<CTPPSPixelDigi> const &rpd = event.get(tokenCTPPSPixelDigi_);

  std::vector<CTPPSPixelCluster> output;

  if (not rpd.empty()) {
    // get analysis mask to mask channels
    const auto &mask = setup.getData(tokenCTPPSPixelAnalysisMask_);
    // get calibration DB
    const auto &gainCalibrations = setup.getData(tokenGainCalib_);
    // run clusterisation
    run(rpd, output, mask, gainCalibrations);
  }

  // write output
  event.emplace(tokenCTPPSPixelCluster_, std::move(output));
}

void CTPPSPixelClusterProducer::run(const edm::DetSetVector<CTPPSPixelDigi> &input,
                                    std::vector<CTPPSPixelCluster> &output,
                                    const CTPPSPixelAnalysisMask &mask,
                                    const CTPPSPixelGainCalibrations &gainCalibration) {
  for (const auto &ds_digi : input) {
    std::vector<CTPPSPixelCluster> clusters;
    clusterizer_.buildClusters(ds_digi.id, ds_digi.data, clusters, &gainCalibration, &mask);

    if (verbosity_) {
      unsigned int cluN = 0;
      for (const auto &cluster : clusters) {
        edm::LogInfo("CTPPSPixelClusterProducer") << "Cluster " << ++cluN << " avg row " << cluster.avg_row()
                                                  << " avg col " << cluster.avg_col() << " ADC.size " << cluster.size();
      }
    }

    for (auto &cluster : clusters) {
      cluster.setDetId(CTPPSPixelDetId(ds_digi.id));
      output.emplace_back(std::move(cluster));
    }
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(CTPPSPixelClusterProducer);
