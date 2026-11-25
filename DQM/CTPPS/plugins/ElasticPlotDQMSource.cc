/****************************************************************************
*
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/CTPPSDetId/interface/TotemRPDetId.h"
#include "DataFormats/CTPPSReco/interface/TotemRPRecHit.h"
#include "DataFormats/CTPPSReco/interface/TotemRPUVPattern.h"
#include "DataFormats/CTPPSReco/interface/TotemRPLocalTrack.h"

#include <string>
#include <array>
#include <map>

//----------------------------------------------------------------------------------------------------

class ElasticPlotDQMSource : public DQMEDAnalyzer {
public:
  ElasticPlotDQMSource(const edm::ParameterSet &ps);
  ~ElasticPlotDQMSource() override;

protected:
  void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;

  void analyze(edm::Event const &e, edm::EventSetup const &eSetup) override;

private:
  static constexpr double ls_duration_inverse = 1. / 23.357;  // s^-1
  static constexpr unsigned int ls_min = 0;
  static constexpr unsigned int ls_max = 2000;  // little more than 12h

  unsigned int verbosity;

  edm::EDGetTokenT<std::vector<TotemRPRecHit>> tokenRecHit;
  edm::EDGetTokenT<std::vector<TotemRPUVPattern>> tokenUVPattern;
  edm::EDGetTokenT<std::vector<TotemRPLocalTrack>> tokenLocalTrack;

  /// plots related to one (anti)diagonal
  struct DiagonalPlots {
    // in the order 45-220-fr, 45-210-fr, 56-210-fr, 56-220-fr
    std::array<unsigned int, 4> rpIds;

    // track correlation in vertical RPs
    MonitorElement *h2_track_corr_vert = nullptr;

    // y distributions in a RP (1st index) with another RP (2nd index) in coincidence
    std::array<std::array<MonitorElement *, 4>, 4> v_h_y;

    // XY hit maps in a give RP (array index) under these conditions
    //   4rp: all 4 diagonal RPs have a track
    //   2rp: diagonal RPs in 220-fr have a track
    std::array<MonitorElement *, 4> v_h2_y_vs_x_dgn_4rp;
    std::array<MonitorElement *, 4> v_h2_y_vs_x_dgn_2rp;

    // event rates vs. time
    MonitorElement *h_rate_vs_time_dgn_4rp = nullptr;
    MonitorElement *h_rate_vs_time_dgn_2rp = nullptr;

    DiagonalPlots() {}

    DiagonalPlots(DQMStore::IBooker &ibooker, int _id);
  };

  std::map<unsigned int, DiagonalPlots> diagonalPlots;

  /// plots related to one RP
  struct PotPlots {
    // event rates vs. time
    //  suff = singal sufficient to reconstruct track
    //  track = track is reconstructed
    //  unresolved = suff && !track
    MonitorElement *h_rate_vs_time_suff = nullptr;
    MonitorElement *h_rate_vs_time_track = nullptr;
    MonitorElement *h_rate_vs_time_unresolved = nullptr;

    PotPlots() {}
    PotPlots(DQMStore::IBooker &ibooker, unsigned int id);
  };

  std::map<unsigned int, PotPlots> potPlots;
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

ElasticPlotDQMSource::DiagonalPlots::DiagonalPlots(DQMStore::IBooker &ibooker, int id) {
  // determine captions
  bool top45 = id & 2;
  bool top56 = id & 1;
  bool diag = (top45 != top56);

  string name = string((diag) ? "diagonal" : "antidiagonal") + " 45" + ((top45) ? "top" : "bot") + " - 56" +
                ((top56) ? "top" : "bot");

  const string &title = name;

  // dermine RP ids of this diagonal
  rpIds[0] = TotemRPDetId(0, 2, (top45) ? 4 : 5);
  rpIds[1] = TotemRPDetId(0, 0, (top45) ? 4 : 5);
  rpIds[2] = TotemRPDetId(1, 0, (top56) ? 4 : 5);
  rpIds[3] = TotemRPDetId(1, 2, (top56) ? 4 : 5);

  // book histograms
  ibooker.setCurrentFolder("CTPPS/TrackingStrip/" + name);

  h2_track_corr_vert = ibooker.book2D("track correlation in verticals", title + ";;", 4, -0.5, 3.5, 4, -0.5, 3.5);
  TH2F *h2 = h2_track_corr_vert->getTH2F();
  TAxis *xa = h2->GetXaxis(), *ya = h2->GetYaxis();
  for (unsigned int i = 0; i < 4; i++) {
    string rpName;
    TotemRPDetId(rpIds[i]).rpName(rpName, TotemRPDetId::nFull);
    rpName = rpName.substr(15);  // removes obvious prefix for better readability

    xa->SetBinLabel(i + 1, rpName.c_str());
    ya->SetBinLabel(i + 1, rpName.c_str());
  }

  h_rate_vs_time_dgn_4rp =
      ibooker.book1D("rate - 4 RPs", title + ";lumi section", ls_max - ls_min + 1, -0.5 + ls_min, +0.5 + ls_max);
  h_rate_vs_time_dgn_2rp = ibooker.book1D(
      "rate - 2 RPs (220-fr)", title + ";lumi section", ls_max - ls_min + 1, -0.5 + ls_min, +0.5 + ls_max);

  for (unsigned int i = 0; i < 4; i++) {
    string rpName;
    TotemRPDetId(rpIds[i]).rpName(rpName, TotemRPDetId::nFull);
    rpName = rpName.substr(15);

    ibooker.setCurrentFolder("CTPPS/TrackingStrip/" + name + "/xy hists");

    v_h2_y_vs_x_dgn_4rp[i] = ibooker.book2D(
        "xy hist - " + rpName + " - 4 RPs cond", title + ";x   (mm);y   (mm)", 100, -18., +18., 100, -18., +18.);
    v_h2_y_vs_x_dgn_2rp[i] = ibooker.book2D(
        "xy hist - " + rpName + " - 2 RPs cond", title + ";x   (mm);y   (mm)", 100, -18., +18., 100, -18., +18.);

    ibooker.setCurrentFolder("CTPPS/TrackingStrip/" + name + "/y hists");

    for (unsigned int j = 0; j < 4; j++) {
      string rpCoincName;
      TotemRPDetId(rpIds[j]).rpName(rpCoincName, TotemRPDetId::nFull);
      rpCoincName = rpCoincName.substr(15);

      v_h_y[i][j] =
          ibooker.book1D("y hist - " + rpName + " - coinc " + rpCoincName, title + ";y   (mm)", 180, -18., +18.);
    }
  }
}

//----------------------------------------------------------------------------------------------------

ElasticPlotDQMSource::PotPlots::PotPlots(DQMStore::IBooker &ibooker, unsigned int id) {
  string path;
  TotemRPDetId(id).rpName(path, TotemRPDetId::nPath);
  ibooker.setCurrentFolder(path);

  string title;
  TotemRPDetId(id).rpName(title, TotemRPDetId::nFull);

  h_rate_vs_time_suff =
      ibooker.book1D("rate - suff", title + ";lumi section", ls_max - ls_min + 1, -0.5 + ls_min, +0.5 + ls_max);
  h_rate_vs_time_track =
      ibooker.book1D("rate - track", title + ";lumi section", ls_max - ls_min + 1, -0.5 + ls_min, +0.5 + ls_max);
  h_rate_vs_time_unresolved =
      ibooker.book1D("rate - unresolved", title + ";lumi section", ls_max - ls_min + 1, -0.5 + ls_min, +0.5 + ls_max);
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

ElasticPlotDQMSource::ElasticPlotDQMSource(const edm::ParameterSet &ps)
    : verbosity(ps.getUntrackedParameter<unsigned int>("verbosity", 0)) {
  tokenRecHit = consumes<std::vector<TotemRPRecHit>>(ps.getUntrackedParameter<edm::InputTag>("tagRecHit"));
  tokenUVPattern = consumes<std::vector<TotemRPUVPattern>>(ps.getUntrackedParameter<edm::InputTag>("tagUVPattern"));
  tokenLocalTrack = consumes<std::vector<TotemRPLocalTrack>>(ps.getUntrackedParameter<edm::InputTag>("tagLocalTrack"));
}

//----------------------------------------------------------------------------------------------------

ElasticPlotDQMSource::~ElasticPlotDQMSource() {}

//----------------------------------------------------------------------------------------------------

void ElasticPlotDQMSource::bookHistograms(DQMStore::IBooker &ibooker, edm::Run const &, edm::EventSetup const &) {
  ibooker.cd();
  ibooker.setCurrentFolder("CTPPS");

  // initialize (anti-)diagonal plots
  diagonalPlots[0] = DiagonalPlots(ibooker, 0);  // 45 bot - 56 bot
  diagonalPlots[1] = DiagonalPlots(ibooker, 1);  // 45 bot - 56 top
  diagonalPlots[2] = DiagonalPlots(ibooker, 2);  // 45 top - 56 bot
  diagonalPlots[3] = DiagonalPlots(ibooker, 3);  // 45 top - 56 top

  // loop over arms
  for (unsigned int arm = 0; arm < 2; arm++) {
    // loop over stations
    for (unsigned int st : {0, 2}) {
      // loop over RPs
      for (unsigned int rp : {4, 5}) {
        TotemRPDetId rpId(arm, st, rp);
        potPlots[rpId] = PotPlots(ibooker, rpId);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------

void ElasticPlotDQMSource::analyze(edm::Event const &event, edm::EventSetup const &eventSetup) {
  // get event data
  Handle<std::vector<TotemRPRecHit>> hits;
  event.getByToken(tokenRecHit, hits);

  Handle<std::vector<TotemRPUVPattern>> patterns;
  event.getByToken(tokenUVPattern, patterns);

  Handle<std::vector<TotemRPLocalTrack>> tracks;
  event.getByToken(tokenLocalTrack, tracks);

  // check validity
  bool valid = true;
  valid &= hits.isValid();
  valid &= patterns.isValid();
  valid &= tracks.isValid();

  if (!valid) {
    if (verbosity) {
      LogProblem("ElasticPlotDQMSource") << "ERROR in ElasticPlotDQMSource::analyze > some of the required inputs are "
                                            "not valid. Skipping this event.\n"
                                         << "    hits.isValid = " << hits.isValid() << "\n"
                                         << "    patterns.isValid = " << patterns.isValid() << "\n"
                                         << "    tracks.isValid = " << tracks.isValid();
    }

    return;
  }

  //------------------------------
  // categorise RP data
  map<unsigned int, unsigned int> rp_planes_u_too_full, rp_planes_v_too_full;
  map<unsigned int, bool> rp_pat_suff;
  map<unsigned int, const TotemRPLocalTrack *> rp_track;

  map<unsigned int, unsigned int> hitsPerPlane;
  for (const auto &hit : *hits)
    ++hitsPerPlane[hit.detId().rawId()];

  for (const auto &planeAndCount : hitsPerPlane) {
    TotemRPDetId detId(planeAndCount.first);
    CTPPSDetId rpId = detId.rpId();

    if (planeAndCount.second > 5) {
      if (detId.isStripsCoordinateUDirection())
        rp_planes_u_too_full[rpId]++;
      else
        rp_planes_v_too_full[rpId]++;
    }
  }

  map<unsigned int, pair<unsigned int, unsigned int>> patternsPerRP;
  for (const auto &pattern : *patterns) {
    auto &counts = patternsPerRP[pattern.detId().rawId()];
    if (!pattern.fittable())
      continue;
    if (pattern.projection() == TotemRPUVPattern::projU)
      ++counts.first;
    else if (pattern.projection() == TotemRPUVPattern::projV)
      ++counts.second;
  }

  for (const auto &rpAndCounts : patternsPerRP) {
    CTPPSDetId rpId(rpAndCounts.first);
    rp_pat_suff[rpId] = (rpAndCounts.second.first > 0 || rp_planes_u_too_full[rpId] >= 3) &&
                        (rpAndCounts.second.second > 0 || rp_planes_v_too_full[rpId] >= 3);
  }

  for (const auto &track : *tracks) {
    CTPPSDetId rpId(track.detId());
    const TotemRPLocalTrack *&selectedTrack = rp_track[rpId];
    if (selectedTrack == nullptr && track.isValid())
      selectedTrack = &track;
  }

  //------------------------------
  // diagonal plots

  for (auto &dpp : diagonalPlots) {
    auto &dp = dpp.second;

    // determine diagonal conditions
    bool cond_4rp = true, cond_2rp = true;
    for (unsigned int i = 0; i < 4; i++) {
      if (rp_track[dp.rpIds[i]] == nullptr)
        cond_4rp = false;

      if ((i == 0 || i == 3) && rp_track[dp.rpIds[i]] == nullptr)
        cond_2rp = false;
    }

    if (cond_4rp)
      dp.h_rate_vs_time_dgn_4rp->Fill(event.luminosityBlock(), ls_duration_inverse);

    if (cond_2rp)
      dp.h_rate_vs_time_dgn_2rp->Fill(event.luminosityBlock(), ls_duration_inverse);

    for (unsigned int i = 0; i < 4; i++) {
      const TotemRPLocalTrack *tr_i = rp_track[dp.rpIds[i]];

      if (tr_i == nullptr)
        continue;

      if (cond_4rp)
        dp.v_h2_y_vs_x_dgn_4rp[i]->Fill(tr_i->x0(), tr_i->y0());

      if (cond_2rp)
        dp.v_h2_y_vs_x_dgn_2rp[i]->Fill(tr_i->x0(), tr_i->y0());

      for (unsigned int j = 0; j < 4; j++) {
        if (rp_track[dp.rpIds[j]] == nullptr)
          continue;

        dp.h2_track_corr_vert->Fill(i, j);

        dp.v_h_y[i][j]->Fill(tr_i->y0());
      }
    }
  }

  //------------------------------
  // pot plots

  for (const auto &p : rp_pat_suff) {
    const auto &rpId = p.first;
    auto pp_it = potPlots.find(rpId);
    if (pp_it == potPlots.end())
      continue;
    auto &pp = pp_it->second;

    const auto &pat_suff = rp_pat_suff[rpId];
    const auto &has_track = (rp_track[rpId] != nullptr);

    if (pat_suff)
      pp.h_rate_vs_time_suff->Fill(event.luminosityBlock(), ls_duration_inverse);
    if (has_track)
      pp.h_rate_vs_time_track->Fill(event.luminosityBlock(), ls_duration_inverse);
    if (pat_suff && !has_track)
      pp.h_rate_vs_time_unresolved->Fill(event.luminosityBlock(), ls_duration_inverse);
  }
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(ElasticPlotDQMSource);
