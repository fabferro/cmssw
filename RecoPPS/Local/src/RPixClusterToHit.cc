#include "RecoPPS/Local/interface/RPixClusterToHit.h"

RPixClusterToHit::RPixClusterToHit(edm::ParameterSet const &conf) {
  verbosity_ = conf.getUntrackedParameter<int>("RPixVerbosity");
}

RPixClusterToHit::~RPixClusterToHit() {}

void RPixClusterToHit::buildHits(unsigned int detId,
                                 const std::vector<CTPPSPixelCluster> &clusters,
                                 std::vector<CTPPSPixelRecHit> &hits,
                                 const PPSPixelTopology &ppt) {
  if (verbosity_)
    edm::LogInfo("PPS") << " RPixClusterToHit " << detId << " received cluster array of size = " << clusters.size();
  for (unsigned int i = 0; i < clusters.size(); i++) {
    make_hit(clusters[i], hits, ppt);
  }
}

void RPixClusterToHit::make_hit(CTPPSPixelCluster aCluster,
                                std::vector<CTPPSPixelRecHit> &hits,
                                const PPSPixelTopology &ppt) {
  // take a cluster, generate a rec hit and push it in the rec hit vector

  //call the numbering inside the ROC
  CTPPSPixelIndices pxlInd;
  // get information from the cluster
  // get the whole cluster size and row/col size
  unsigned int thisClusterSize = aCluster.size();
  unsigned int thisClusterRowSize = aCluster.sizeRow();
  unsigned int thisClusterColSize = aCluster.sizeCol();

  // get the minimum pixel row/col
  unsigned int thisClusterMinRow = aCluster.minPixelRow();
  unsigned int thisClusterMinCol = aCluster.minPixelCol();

  // calculate "on edge" flag
  bool anEdgePixel = false;
  if (aCluster.minPixelRow() == 0 || aCluster.minPixelCol() == 0 ||
      int(aCluster.minPixelRow() + aCluster.rowSpan()) == (pxlInd.getDefaultRowDetSize() - 1) ||
      int(aCluster.minPixelCol() + aCluster.colSpan()) == (pxlInd.getDefaultColDetSize() - 1))
    anEdgePixel = true;

  // check for bad (ADC=0) pixels in cluster
  bool aBadPixel = false;
  for (unsigned int i = 0; i < thisClusterSize; i++) {
    if (aCluster.pixelADC(i) == 0)
      aBadPixel = true;
  }

  // check for spanning two ROCs
  bool twoRocs = false;
  int currROCId = pxlInd.getROCId(aCluster.pixelCol(0), aCluster.pixelRow(0));

  for (unsigned int i = 1; i < thisClusterSize; i++) {
    if (pxlInd.getROCId(aCluster.pixelCol(i), aCluster.pixelRow(i)) != currROCId) {
      twoRocs = true;
      break;
    }
  }

  //estimate position and error of the hit
  double avgWLocalX = 0;
  double avgWLocalY = 0;
  double weights = 0;
  double varianceX = 0.;
  double varianceY = 0.;

  double minPxlX = 0;
  double minPxlY = 0;
  double maxPxlX = 0;
  double maxPxlY = 0;
  double halfSizeX = 0;
  double halfSizeY = 0;
  double avgPxlX = 0;
  double avgPxlY = 0;

  if (verbosity_)
    edm::LogInfo("PPS") << "RPixClusterToHit "
                        << " hit pixels: ";

  for (unsigned int i = 0; i < thisClusterSize; i++) {
    if (verbosity_)
      edm::LogInfo("PPS") << "RPixClusterToHit " << aCluster.pixelRow(i) << " " << aCluster.pixelCol(i) << " "
                          << aCluster.pixelADC(i);

    ppt.pixelRange(aCluster.pixelRow(i), aCluster.pixelCol(i), minPxlX, maxPxlX, minPxlY, maxPxlY);
    halfSizeX = (maxPxlX - minPxlX) / 2.;
    halfSizeY = (maxPxlY - minPxlY) / 2.;
    avgPxlX = minPxlX + halfSizeX;
    avgPxlY = minPxlY + halfSizeY;
    avgWLocalX += avgPxlX * aCluster.pixelADC(i);
    avgWLocalY += avgPxlY * aCluster.pixelADC(i);
    weights += aCluster.pixelADC(i);
  }

  if (weights == 0) {
    edm::LogError("RPixClusterToHit") << " unexpected weights = 0 for cluster (Row_min, Row_max, Col_min, Col_max) = ("
                                      << aCluster.minPixelRow() << "," << aCluster.minPixelRow() + aCluster.rowSpan()
                                      << "," << aCluster.minPixelCol() << ","
                                      << aCluster.minPixelCol() + aCluster.colSpan() << ")";
    return;
  }
  double invWeights = 1. / weights;
  double avgLocalX = avgWLocalX * invWeights;
  double avgLocalY = avgWLocalY * invWeights;

//calculate variance
  if(thisClusterSize>1){
    for (unsigned int i = 0; i < thisClusterSize; i++) {
      ppt.pixelRange(aCluster.pixelRow(i), aCluster.pixelCol(i), minPxlX, maxPxlX, minPxlY, maxPxlY);
      avgPxlX = minPxlX + halfSizeX;
      avgPxlY = minPxlY + halfSizeY;
      varianceX += aCluster.pixelADC(i) * (avgPxlX - avgLocalX) * (avgPxlX - avgLocalX) * invWeights;
      varianceY += aCluster.pixelADC(i) * (avgPxlY - avgLocalY) * (avgPxlY - avgLocalY) * invWeights;
    }
  }
// cluster size 1 in X or Y
  if(varianceX < 1e-6) varianceX = halfSizeX*halfSizeX/3.;
  if(varianceY < 1e-6) varianceY = halfSizeY*halfSizeY/3.;

  LocalPoint lp(avgLocalX, avgLocalY, 0);
  LocalError le(varianceX, 0, varianceY);
  if (verbosity_)
    edm::LogInfo("PPS") << "RPixClusterToHit " << lp << " with error " << le;

  hits.emplace_back(lp,
                    le,
                    anEdgePixel,
                    aBadPixel,
                    twoRocs,
                    thisClusterMinRow,
                    thisClusterMinCol,
                    thisClusterSize,
                    thisClusterRowSize,
                    thisClusterColSize);

  return;
}
