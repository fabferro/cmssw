/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
*   Hubert Niewiadomski
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <map>
#include <vector>

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/CTPPSDigi/interface/TotemRPDigi.h"

#include "RecoPPS/Local/interface/TotemRPClusterProducerAlgorithm.h"

//----------------------------------------------------------------------------------------------------

/**
 * Merges neighbouring active TOTEM RP strips into clusters.
 **/
class TotemRPClusterProducer : public edm::stream::EDProducer<> {
public:
  explicit TotemRPClusterProducer(const edm::ParameterSet& conf);

  ~TotemRPClusterProducer() override {}

  void produce(edm::Event& e, const edm::EventSetup& c) override;
  static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
  edm::ParameterSet conf_;
  int verbosity_;
  edm::InputTag digiInputTag_;
  edm::EDGetTokenT<std::vector<TotemRPDigi>> digiInputTagToken_;

  TotemRPClusterProducerAlgorithm algorithm_;

  void run(const std::vector<TotemRPDigi>& input, std::vector<TotemRPCluster>& output);
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

TotemRPClusterProducer::TotemRPClusterProducer(edm::ParameterSet const& conf) : conf_(conf), algorithm_(conf) {
  verbosity_ = conf.getParameter<int>("verbosity");

  digiInputTag_ = conf.getParameter<edm::InputTag>("tagDigi");
  digiInputTagToken_ = consumes<std::vector<TotemRPDigi>>(digiInputTag_);

  produces<std::vector<TotemRPCluster>>();
}

//----------------------------------------------------------------------------------------------------

void TotemRPClusterProducer::produce(edm::Event& e, const edm::EventSetup& es) {
  // get input
  edm::Handle<std::vector<TotemRPDigi>> input;
  e.getByToken(digiInputTagToken_, input);

  // prepare output
  std::vector<TotemRPCluster> output;

  // run clusterisation
  if (!input->empty())
    run(*input, output);

  // save output to event
  e.put(make_unique<std::vector<TotemRPCluster>>(std::move(output)));
}

//----------------------------------------------------------------------------------------------------

void TotemRPClusterProducer::run(const std::vector<TotemRPDigi>& input,
                                 std::vector<TotemRPCluster>& output) {
  std::map<uint32_t, std::vector<TotemRPDigi>> digisByDetId;
  for (const auto& digi : input)
    digisByDetId[digi.detId()].push_back(digi);

  for (const auto& [detId, digis] : digisByDetId) {
    std::vector<TotemRPCluster> clusters;
    algorithm_.buildClusters(detId, digis, clusters);
    for (auto& cluster : clusters) {
      cluster.setDetId(TotemRPDetId(detId));
      output.emplace_back(std::move(cluster));
    }
  }
}

//----------------------------------------------------------------------------------------------------

void TotemRPClusterProducer::fillDescriptions(edm::ConfigurationDescriptions& descr) {
  edm::ParameterSetDescription desc;

  desc.add<edm::InputTag>("tagDigi", edm::InputTag("totemRPRawToDigi", "TrackingStrip"))
      ->setComment("input digis collection to retrieve");
  desc.add<int>("verbosity", 0);

  descr.add("totemRPClusterProducer", desc);
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(TotemRPClusterProducer);
