/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
* 	Hubert Niewiadomski
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#include "RecoPPS/Local/interface/TotemRPRecHitProducerAlgorithm.h"

//----------------------------------------------------------------------------------------------------

void TotemRPRecHitProducerAlgorithm::buildRecoHits(const std::vector<TotemRPCluster>& input,
                                                   std::vector<TotemRPRecHit>& output) {
  for (const auto& clus : input) {
    constexpr double nominal_sigma = 0.0191;
    TotemRPRecHit hit(rp_topology_.GetHitPositionInReadoutDirection(clus.centerStripPosition()), nominal_sigma);
    hit.setDetId(clus.detId());
    output.emplace_back(std::move(hit));
  }
}
