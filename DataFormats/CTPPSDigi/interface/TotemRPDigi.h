/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
*   Hubert Niewiadomski
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#ifndef DataFormats_CTPPSDigi_TotemRPDigi
#define DataFormats_CTPPSDigi_TotemRPDigi

/**
 * Digi structure for TOTEM RP silicon strip sensors.
**/
namespace io_v1 {
  class TotemRPDigi {
  public:
    TotemRPDigi(unsigned short strip_no = 0, uint32_t detId = 0) : strip_no_(strip_no), detId_(detId) {}

    unsigned short stripNumber() const { return strip_no_; }
    uint32_t detId() const { return detId_; }

  private:
    /// index of the activated strip
    unsigned short strip_no_;
    uint32_t detId_;
  };

  inline bool operator<(const TotemRPDigi& one, const TotemRPDigi& other) {
    return one.stripNumber() < other.stripNumber();
  }
}  // namespace io_v1
using TotemRPDigi = io_v1::TotemRPDigi;
#endif
