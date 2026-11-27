/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
* 	Hubert Niewiadomski
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/Utilities/interface/ESGetToken.h"

#include "DataFormats/CTPPSReco/interface/TotemRPRecHit.h"
#include "DataFormats/CTPPSReco/interface/TotemRPUVPattern.h"
#include "DataFormats/CTPPSReco/interface/TotemRPLocalTrack.h"

#include "Geometry/Records/interface/VeryForwardRealGeometryRecord.h"
#include "Geometry/VeryForwardGeometryBuilder/interface/CTPPSGeometry.h"

#include "RecoPPS/Local/interface/TotemRPLocalTrackFitterAlgorithm.h"

#include <map>
#include <vector>

//----------------------------------------------------------------------------------------------------

/**
 *\brief Fits tracks trough a single RP.
 **/
class TotemRPLocalTrackFitter : public edm::stream::EDProducer<> {
public:
  explicit TotemRPLocalTrackFitter(const edm::ParameterSet &conf);

  ~TotemRPLocalTrackFitter() override {}

  void produce(edm::Event &e, const edm::EventSetup &c) override;
  static void fillDescriptions(edm::ConfigurationDescriptions &);

private:
  int verbosity_;

  /// Selection of the pattern-recognition module.
  edm::InputTag tagUVPattern;

  edm::EDGetTokenT<std::vector<TotemRPUVPattern>> patternCollectionToken;
  edm::ESGetToken<CTPPSGeometry, VeryForwardRealGeometryRecord> geometryToken;

  /// A watcher to detect geometry changes.
  edm::ESWatcher<VeryForwardRealGeometryRecord> geometryWatcher;

  /// The instance of the fitter module
  TotemRPLocalTrackFitterAlgorithm fitter_;
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

TotemRPLocalTrackFitter::TotemRPLocalTrackFitter(const edm::ParameterSet &conf)
    : verbosity_(conf.getParameter<int>("verbosity")), fitter_(conf) {
  tagUVPattern = conf.getParameter<edm::InputTag>("tagUVPattern");
  patternCollectionToken = consumes<std::vector<TotemRPUVPattern>>(tagUVPattern);
  geometryToken = esConsumes<CTPPSGeometry, VeryForwardRealGeometryRecord>();

  produces<std::vector<TotemRPLocalTrack>>();
}

//----------------------------------------------------------------------------------------------------

void TotemRPLocalTrackFitter::produce(edm::Event &e, const edm::EventSetup &setup) {
  if (verbosity_ > 5)
    LogVerbatim("TotemRPLocalTrackFitter") << ">> TotemRPLocalTrackFitter::produce";

  // get geometry
  edm::ESHandle<CTPPSGeometry> geometry = setup.getHandle(geometryToken);

  if (geometryWatcher.check(setup))
    fitter_.reset();

  // get input
  edm::Handle<std::vector<TotemRPUVPattern>> input;
  e.getByToken(patternCollectionToken, input);

  std::map<unsigned int, std::vector<TotemRPUVPattern>> patternsByRP;
  for (const auto &pattern : *input)
    patternsByRP[pattern.detId().rawId()].push_back(pattern);

  // run fit for each RP
  std::vector<TotemRPLocalTrack> output;

  for (const auto &[rpRawId, rpv] : patternsByRP) {
    CTPPSDetId rpId(rpRawId);

    // is U-V association unique?
    unsigned int n_U = 0, n_V = 0;
    unsigned int idx_U = 0, idx_V = 0;
    for (unsigned int pi = 0; pi < rpv.size(); pi++) {
      const TotemRPUVPattern &pattern = rpv[pi];

      // here it would make sense to skip non-fittable patterns, but to keep the logic
      // equivalent to version 7_0_4, nothing is skipped
      /*
      if (pattern.fittable() == false)
        continue;
      */

      switch (pattern.projection()) {
        case TotemRPUVPattern::projU:
          n_U++;
          idx_U = pi;
          break;

        case TotemRPUVPattern::projV:
          n_V++;
          idx_V = pi;
          break;

        default:
          break;
      }
    }

    if (n_U != 1 || n_V != 1) {
      if (verbosity_)
        LogVerbatim("TotemRPLocalTrackFitter")
            << ">> TotemRPLocalTrackFitter::produce > Impossible to combine U and V patterns in RP " << rpId
            << " (n_U=" << n_U << ", n_V=" << n_V << ").";

      continue;
    }

    // again, to follow the logic from version 7_0_4, skip the non-fittable patterns here
    if (!rpv[idx_U].fittable() || !rpv[idx_V].fittable())
      continue;

    // combine U and V hits
    std::vector<TotemRPRecHit> hits;
    hits.insert(hits.end(), rpv[idx_U].hits().begin(), rpv[idx_U].hits().end());

    hits.insert(hits.end(), rpv[idx_V].hits().begin(), rpv[idx_V].hits().end());

    // run fit
    double z0 = geometry->rpTranslation(rpId).z();

    TotemRPLocalTrack track;
    fitter_.fitTrack(hits, z0, *geometry, track);
    track.setDetId(rpId);
    output.emplace_back(std::move(track));

    if (verbosity_ > 5) {
      unsigned int n_hits = 0;
      n_hits = track.hits().size();

      LogVerbatim("TotemRPLocalTrackFitter")
          << "    track in RP " << rpId << ": valid = " << track.isValid() << ", hits = " << n_hits;
    }
  }

  // save results
  e.put(make_unique<std::vector<TotemRPLocalTrack>>(std::move(output)));
}

//----------------------------------------------------------------------------------------------------

void TotemRPLocalTrackFitter::fillDescriptions(edm::ConfigurationDescriptions &descr) {
  edm::ParameterSetDescription desc;

  desc.add<edm::InputTag>("tagUVPattern", edm::InputTag("totemRPUVPatternFinder"))
      ->setComment("input U-V patterns collection to retrieve");
  desc.add<int>("verbosity", 0);

  descr.add("totemRPLocalTrackFitter", desc);
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(TotemRPLocalTrackFitter);
