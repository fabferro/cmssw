#ifndef SimPPS_RPIXDigiAnalyzer_h
#define SimPPS_RPIXDigiAnalyzer_h

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <FWCore/Framework/interface/one/EDAnalyzer.h>
#include <DataFormats/CTPPSDetId/interface/CTPPSPixelDetId.h>
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "DataFormats/CTPPSDigi/interface/CTPPSPixelDigi.h"
#include "DataFormats/CTPPSDigi/interface/CTPPSPixelDigiCollection.h"

#include "SimDataFormats/CrossingFrame/interface/CrossingFrame.h"
#include "SimDataFormats/CrossingFrame/interface/MixCollection.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include <vector>

#include <FWCore/Framework/interface/Event.h>
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "CondFormats/PPSObjects/interface/PPSPixelTopology.h"
#include "CondFormats/DataRecord/interface/PPSPixelTopologyRcd.h"

#include <iostream>
#include <set>
#include <string>

#include "TH2D.h"

#define SELECTED_PIXEL_ROW 89
#define SELECTED_PIXEL_COLUMN 23
#define SELECTED_UNITID 2014314496
#define TG184 0.332655724

#define USE_MIDDLE_OF_PIXEL_2
#define CENTERX 1.05
#define CENTERY -8.475

using namespace edm;
using namespace std;

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}  // namespace edm

class PPSPixelDigiAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit PPSPixelDigiAnalyzer(const edm::ParameterSet &pset);
  ~PPSPixelDigiAnalyzer() override;
  void endJob() override;
  void beginJob() override;
  void analyze(const edm::Event &event, const edm::EventSetup &eventSetup) override;

private:
  TH2D *hAllHits;
  TH2D *hOneHitperEvent;
  TH2D *hOneHitperEvent2;
  TH2D *hOneHitperEventCenter;
  TH2D *hOneHitperEvent2Center;
  //TFile *file;
  std::string label_;

  int verbosity_;
  edm::EDGetTokenT<edm::PSimHitContainer> psim_token;
  edm::EDGetTokenT<std::vector<CTPPSPixelDigi>> pixel_token;
  edm::ESGetToken<PPSPixelTopology, PPSPixelTopologyRcd> pixelTopologyToken_;

  unsigned int found_corresponding_digi_count_;
  unsigned int cumulative_cluster_size_[3];
};

PPSPixelDigiAnalyzer::PPSPixelDigiAnalyzer(const ParameterSet &pset)
    : hAllHits(nullptr),
      hOneHitperEvent(nullptr),
      hOneHitperEvent2(nullptr),
      hOneHitperEventCenter(nullptr),
      hOneHitperEvent2Center(nullptr) {
  label_ = pset.getUntrackedParameter<string>("label");
  verbosity_ = pset.getParameter<int>("Verbosity");
  edm::Service<TFileService> file;
#ifdef USE_MIDDLE_OF_PIXEL
  hOneHitperEvent = file->make<TH2D>("OneHitperEvent", "One Hit per Event", 30, -8.511, -8.361, 20, 1, 1.1);
  hOneHitperEvent2 = file->make<TH2D>("OneHitperEvent2", "One Hit per Event 2", 30, -8.511, -8.361, 20, 1, 1.1);
#else
  hOneHitperEvent = file->make<TH2D>("OneHitperEvent", "One Hit per Event", 30, -8.55, -8.4, 20, 1, 1.1);
  hOneHitperEvent2 = file->make<TH2D>("OneHitperEvent2", "One Hit per Event 2", 30, -8.55, -8.4, 20, 1, 1.1);
  hOneHitperEventCenter =
      file->make<TH2D>("OneHitperEventCenter", "One Hit per Event Center", 30, -0.075, 0.075, 20, -0.05, 0.05);
  hOneHitperEvent2Center =
      file->make<TH2D>("OneHitperEvent2Center", "Cluster Size 2", 30, -0.075, 0.075, 20, -0.05, 0.05);
#endif
  file->cd();
  hAllHits = file->make<TH2D>("AllHits", "All Hits", 10, 1, 1.1, 10, -8.55, -8.4);

  psim_token = consumes<PSimHitContainer>(edm::InputTag("g4SimHits", "CTPPSPixelHits"));
  pixel_token = consumes<std::vector<CTPPSPixelDigi>>(edm::InputTag(label_, ""));  //label=RPixDetDigitizer???
  pixelTopologyToken_ = esConsumes<PPSPixelTopology, PPSPixelTopologyRcd>();
}

PPSPixelDigiAnalyzer::~PPSPixelDigiAnalyzer() {}

void PPSPixelDigiAnalyzer::beginJob() {
  found_corresponding_digi_count_ = 0;
  for (int a = 0; a < 3; a++)
    cumulative_cluster_size_[a] = 0;
}
void PPSPixelDigiAnalyzer::endJob() {
  edm::LogInfo("PPSPixelDigiAnalyzer") << "found_corresponding_digi_count_: " << found_corresponding_digi_count_;
  edm::LogInfo("PPSPixelDigiAnalyzer") << "Cumulative cluster size (1,2,>2) = " << cumulative_cluster_size_[0] << ", "
                                       << cumulative_cluster_size_[1] << ", " << cumulative_cluster_size_[2];
}

void PPSPixelDigiAnalyzer::analyze(const Event &event, const EventSetup &eventSetup) {
  if (verbosity_ > 0)
    edm::LogInfo("PPSPixelDigiAnalyzer") << "--- Run: " << event.id().run() << " Event: " << event.id().event();

  edm::LogInfo("PPSPixelDigiAnalyzer")
      << "                                                            I do love Pixels     ";
  Handle<PSimHitContainer> simHits;
  event.getByToken(psim_token, simHits);

  edm::Handle<std::vector<CTPPSPixelDigi>> CTPPSPixelDigis;
  event.getByToken(pixel_token, CTPPSPixelDigis);
  edm::ESHandle<PPSPixelTopology> thePixelTopology = eventSetup.getHandle(pixelTopologyToken_);

  if (verbosity_ > 0)
    edm::LogInfo("PPSPixelDigiAnalyzer") << "\n=================== RPDA Starting SimHit access"
                                         << "  ===================";

  if (verbosity_ > 1)
    edm::LogInfo("PPSPixelDigiAnalyzer") << simHits->size();

  double selected_pixel_lower_x;
  double selected_pixel_lower_y;
  double selected_pixel_upper_x;
  double selected_pixel_upper_y;
  double myX = 0;
  double myY = 0;

  thePixelTopology->pixelRange(SELECTED_PIXEL_ROW,
                               SELECTED_PIXEL_COLUMN,
                               selected_pixel_lower_x,
                               selected_pixel_upper_x,
                               selected_pixel_lower_y,
                               selected_pixel_upper_y);

  double hit_inside_selected_pixel[2];
  bool found_hit_inside_selected_pixel = false;

  for (vector<PSimHit>::const_iterator hit = simHits->begin(); hit != simHits->end(); hit++) {
    LocalPoint entryP = hit->entryPoint();
    LocalPoint exitP = hit->exitPoint();
    LocalPoint midP((entryP.x() + exitP.x()) / 2., (entryP.y() + exitP.y()) / 2.);

#ifdef USE_MIDDLE_OF_PIXEL
    if (entryP.x() > selected_pixel_lower_x && entryP.x() < selected_pixel_upper_x &&
        entryP.y() > (selected_pixel_lower_y + 0.115 * TG184) && entryP.y() < (selected_pixel_upper_y + 0.115 * TG184)
#else
#ifdef USE_MIDDLE_OF_PIXEL_2
    if (midP.x() > selected_pixel_lower_x && midP.x() < selected_pixel_upper_x && midP.y() > selected_pixel_lower_y &&
        midP.y() < selected_pixel_upper_y
#else
    if (entryP.x() > selected_pixel_lower_x && entryP.x() < selected_pixel_upper_x &&
        entryP.y() > selected_pixel_lower_y && entryP.y() < selected_pixel_upper_y
#endif
#endif
        && hit->detUnitId() == SELECTED_UNITID) {
      hit_inside_selected_pixel[0] = entryP.x();
      hit_inside_selected_pixel[1] = entryP.y();
      found_hit_inside_selected_pixel = true;
#ifdef USE_MIDDLE_OF_PIXEL_2
      hAllHits->Fill(midP.x(), midP.y());
      myX = midP.x();
      myY = midP.y();
#else
      hAllHits->Fill(entryP.x(), entryP.y());
      myX = entryP.x();
      myY = entryP.y();
#endif
      if (verbosity_ > 2)
        edm::LogInfo("PPSPixelDigiAnalyzer") << hit_inside_selected_pixel[0] << " " << hit_inside_selected_pixel[1];
    }

    //--------------

    if (verbosity_ > 1)
      if (hit->timeOfFlight() > 0) {
        edm::LogInfo("PPSPixelDigiAnalyzer")
            << "DetId: " << hit->detUnitId() << "PID: " << hit->particleType() << " TOF: " << hit->timeOfFlight()
            << " Proc Type: " << hit->processType() << " p: " << hit->pabs() << " x = " << entryP.x()
            << "   y = " << entryP.y() << "  z = " << entryP.z();
      }
  }

  if (verbosity_ > 0)
    edm::LogInfo("PPSPixelDigiAnalyzer") << "\n=================== RPDA Starting Digi access"
                                         << "  ===================";
  std::set<uint32_t> detector_ids;
  bool found_corresponding_digi = false;
  unsigned int corresponding_digi_cluster_size = 0;

  for (const auto& digi : *CTPPSPixelDigis) {
    const bool first_digi_for_detector = detector_ids.insert(digi.detId()).second;
    if (first_digi_for_detector && verbosity_ > 1)
      edm::LogInfo("PPSPixelDigiAnalyzer") << "DetId: " << digi.detId();

    if (verbosity_ > 2) {
      edm::LogInfo("PPSPixelDigiAnalyzer") << "           Digi row  " << digi.row() << ", col " << digi.column();
      double lx, ly, ux, uy;
      thePixelTopology->pixelRange(digi.row(), digi.column(), lx, ux, ly, uy);
      edm::LogInfo("PPSPixelDigiAnalyzer")
          << " pixel boundaries x low up, y low up " << lx << " " << ux << " " << ly << " " << uy;
    }

    if (digi.detId() == SELECTED_UNITID && found_hit_inside_selected_pixel && digi.row() == SELECTED_PIXEL_ROW &&
        digi.column() == SELECTED_PIXEL_COLUMN) {
      if (verbosity_ > 1)
        edm::LogInfo("PPSPixelDigiAnalyzer") << "           Digi row  " << digi.row() << ", col " << digi.column();
      found_corresponding_digi_count_++;
      found_corresponding_digi = true;
      corresponding_digi_cluster_size = 1;
    }
  }

  if (found_corresponding_digi) {
    for (const auto& digi : *CTPPSPixelDigis) {
      if (digi.detId() != SELECTED_UNITID)
        continue;

      if ((digi.row() == SELECTED_PIXEL_ROW + 1 && digi.column() == SELECTED_PIXEL_COLUMN) ||
          (digi.row() == SELECTED_PIXEL_ROW - 1 && digi.column() == SELECTED_PIXEL_COLUMN) ||
          (digi.row() == SELECTED_PIXEL_ROW && digi.column() == SELECTED_PIXEL_COLUMN + 1) ||
          (digi.row() == SELECTED_PIXEL_ROW && digi.column() == SELECTED_PIXEL_COLUMN - 1)) {
        corresponding_digi_cluster_size++;
        if (verbosity_ > 1)
          edm::LogInfo("PPSPixelDigiAnalyzer") << "           Digi row  " << digi.row() << ", col " << digi.column();
      }
    }
  }

  if (corresponding_digi_cluster_size > 0) {
    edm::LogInfo("PPSPixelDigiAnalyzer")
        << "corresponding_digi_cluster_size in the event: " << corresponding_digi_cluster_size;
    hOneHitperEvent->Fill(myY, myX);
    hOneHitperEventCenter->Fill(myY - CENTERY, myX - CENTERX);
    if (corresponding_digi_cluster_size < 3) {
      cumulative_cluster_size_[corresponding_digi_cluster_size - 1]++;
      if (corresponding_digi_cluster_size > 1) {
        hOneHitperEvent2->Fill(myY, myX);
        hOneHitperEvent2Center->Fill(myY - CENTERY, myX - CENTERX);
      }
    } else {
      cumulative_cluster_size_[2]++;
      hOneHitperEvent2->Fill(myY, myX);
      hOneHitperEvent2Center->Fill(myY - CENTERY, myX - CENTERX);
    }
  }

  const int numberOfDetUnits = detector_ids.size();

  if (verbosity_ > 1)
    edm::LogInfo("PPSPixelDigiAnalyzer") << "numberOfDetUnits in the event: " << numberOfDetUnits;
}

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(PPSPixelDigiAnalyzer);

#endif
