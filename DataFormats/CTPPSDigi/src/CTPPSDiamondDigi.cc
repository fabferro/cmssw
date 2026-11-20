/** \file
 *
 *
 * \author Seyed Mohsen Etesami
 */

#include <DataFormats/CTPPSDigi/interface/CTPPSDiamondDigi.h>

using namespace std;

CTPPSDiamondDigi::CTPPSDiamondDigi(
  unsigned int ledgt_,
  unsigned int tedgt_,
  unsigned int threvolt_,
  bool mhit_,
  unsigned short hptdcerror_,
  uint32_t detId)
  : ledgt(ledgt_),
    tedgt(tedgt_),
    threvolt(threvolt_),
    mhit(mhit_),
    hptdcerror(HPTDCErrorFlags(hptdcerror_)),
    detId_(detId) {}

CTPPSDiamondDigi::CTPPSDiamondDigi()
  : ledgt(0), tedgt(0), threvolt(0), mhit(false), hptdcerror(HPTDCErrorFlags(0)), detId_(0) {}

// Comparison
bool CTPPSDiamondDigi::operator==(const CTPPSDiamondDigi& digi) const {
  if (ledgt != digi.leadingEdge() || tedgt != digi.trailingEdge() || threvolt != digi.thresholdVoltage() ||
      detId_ != digi.detId() ||
      mhit != digi.multipleHit() || hptdcerror.errorFlag() != digi.hptdcErrorFlags().errorFlag())
    return false;
  else
    return true;
}
