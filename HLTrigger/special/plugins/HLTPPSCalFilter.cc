// <author>Mariana Araujo</author>
// <email>mariana.araujo@cern.ch</email>
// <created>2020-12-30</created>
// <description>
// HLT filter module to select events with min and max multiplicity
// in each tracker of the PPS for PCL
// Adapted from a preexisting filter code by Laurent Forthomme
// </description>

#include "FWCore/Framework/interface/global/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/DetSet.h"

#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"

#include "DataFormats/CTPPSDetId/interface/CTPPSPixelDetId.h"

#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"  // pixel

#include <unordered_map>

class HLTPPSCalFilter : public edm::global::EDFilter<> {
public:
  explicit HLTPPSCalFilter(const edm::ParameterSet&);
  ~HLTPPSCalFilter() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions&);
  bool filter(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;

private:
  const edm::InputTag pixelLocalTrackInputTag_;  // Input tag identifying the pixel detector
  const edm::EDGetTokenT<std::vector<CTPPSPixelLocalTrack>> pixelLocalTrackToken_;

  const int minTracks_;
  const int maxTracks_;

  const bool do_express_;
};

void HLTPPSCalFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<edm::InputTag>("pixelLocalTrackInputTag", edm::InputTag("ctppsPixelLocalTracks"))
      ->setComment("input tag of the pixel local track collection");

  desc.add<int>("minTracks", 1)->setComment("minimum number of tracks in pot");
  desc.add<int>("maxTracks", -1)->setComment("maximum number of tracks in pot");

  desc.add<bool>("do_express", true)->setComment("toggle on filter type; true for Express, false for Prompt");

  desc.add<int>("triggerType", trigger::TriggerTrack);

  descriptions.add("hltPPSCalFilter", desc);
}

HLTPPSCalFilter::HLTPPSCalFilter(const edm::ParameterSet& iConfig)
    : pixelLocalTrackInputTag_(iConfig.getParameter<edm::InputTag>("pixelLocalTrackInputTag")),
      pixelLocalTrackToken_(consumes<std::vector<CTPPSPixelLocalTrack>>(pixelLocalTrackInputTag_)),
      minTracks_(iConfig.getParameter<int>("minTracks")),
      maxTracks_(iConfig.getParameter<int>("maxTracks")),
      do_express_(iConfig.getParameter<bool>("do_express")) {}

bool HLTPPSCalFilter::filter(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  // Helper tool to count valid tracks
  const auto valid_trks = [](const auto& trk) { return trk.isValid(); };

  // map for assigning filter pass / fail
  std::unordered_map<uint32_t, bool> pixel_filter_;

  // pixel map definition
  pixel_filter_[CTPPSPixelDetId(0, 0, 3)] = false;
  pixel_filter_[CTPPSPixelDetId(0, 2, 3)] = false;
  pixel_filter_[CTPPSPixelDetId(1, 0, 3)] = false;
  pixel_filter_[CTPPSPixelDetId(1, 2, 3)] = false;

  // filter on pixels (2017+) selection
  edm::Handle<std::vector<CTPPSPixelLocalTrack>> pixelTracks;
  iEvent.getByToken(pixelLocalTrackToken_, pixelTracks);

  std::unordered_map<uint32_t, int> validTracksPerPot;
  for (const auto& track : *pixelTracks) {
    const uint32_t detId = track.detId().rawId();
    if (pixel_filter_.count(detId) == 0) {
      continue;
    }
    // assume pass condition if there is at least one track
    pixel_filter_.at(detId) = true;

    // count number of valid tracks
    if (valid_trks(track))
      ++validTracksPerPot[detId];
  }

  for (auto& [detId, passes] : pixel_filter_) {
    if (!passes)
      continue;
    const auto ntrks = validTracksPerPot[detId];
    // fail condition if ntrks not within minTracks, maxTracks values
    if (minTracks_ > 0 && ntrks < minTracks_)
      passes = false;
    if (maxTracks_ > 0 && ntrks > maxTracks_)
      passes = false;
  }

  // compilation of filter conditions
  if (do_express_) {
    return (pixel_filter_.at(CTPPSPixelDetId(0, 0, 3)) && pixel_filter_.at(CTPPSPixelDetId(0, 2, 3))) ||
           (pixel_filter_.at(CTPPSPixelDetId(1, 0, 3)) && pixel_filter_.at(CTPPSPixelDetId(1, 2, 3)));
  } else {
    return (pixel_filter_.at(CTPPSPixelDetId(0, 0, 3)) || pixel_filter_.at(CTPPSPixelDetId(0, 2, 3))) ||
           (pixel_filter_.at(CTPPSPixelDetId(1, 0, 3)) || pixel_filter_.at(CTPPSPixelDetId(1, 2, 3)));
  }

  return false;
}

// define as a framework module
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HLTPPSCalFilter);
