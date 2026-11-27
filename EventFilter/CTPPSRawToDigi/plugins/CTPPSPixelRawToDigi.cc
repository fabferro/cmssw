/*
 * adapting to CTPPS pixel detector March 2017 - F.Ferro
 */

#include "EventFilter/CTPPSRawToDigi/interface/CTPPSPixelRawToDigi.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/CTPPSDigi/interface/CTPPSPixelDigi.h"

#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"

#include "EventFilter/CTPPSRawToDigi/interface/CTPPSPixelDataFormatter.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"

using namespace std;

CTPPSPixelRawToDigi::CTPPSPixelRawToDigi(const edm::ParameterSet& conf)
    : config_(conf),
      eSummary_("CTPPSPixelDataFormatter", "[ctppsPixelRawToDigi]", edm::isDebugEnabled())

{
  FEDRawDataCollection_ = consumes<FEDRawDataCollection>(config_.getParameter<edm::InputTag>("inputLabel"));
  CTPPSPixelDAQMapping_ = esConsumes<CTPPSPixelDAQMapping, CTPPSPixelDAQMappingRcd>();

  produces<std::vector<CTPPSPixelDigi>>();

  isRun3_ = config_.getParameter<bool>("isRun3");
  includeErrors_ = config_.getParameter<bool>("includeErrors");
  mappingLabel_ = config_.getParameter<std::string>("mappingLabel");

  if (includeErrors_) {
    produces<std::vector<CTPPSPixelDataError>>();
  }
}

CTPPSPixelRawToDigi::~CTPPSPixelRawToDigi() {
  edm::LogInfo("CTPPSPixelRawToDigi") << " CTPPSPixelRawToDigi destructor!";
}

void CTPPSPixelRawToDigi::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<bool>("isRun3", true);
  desc.add<bool>("includeErrors", true);
  desc.add<edm::InputTag>("inputLabel", edm::InputTag("rawDataCollector"));
  desc.add<std::string>("mappingLabel", "RPix");
  descriptions.add("ctppsPixelDigis", desc);
}

void CTPPSPixelRawToDigi::produce(edm::Event& ev, const edm::EventSetup& es) {
  edm::Handle<FEDRawDataCollection> buffers;
  ev.getByToken(FEDRawDataCollection_, buffers);

  edm::ESHandle<CTPPSPixelDAQMapping> mapping;

  bool data_exist = false;
  for (int fed = FEDNumbering::MINCTPPSPixelsFEDID; fed <= FEDNumbering::MAXCTPPSPixelsFEDID; fed++) {
    const FEDRawData& tempRawData = buffers->FEDData(fed);
    if (tempRawData.size() != 0) {
      data_exist = true;
      break;
    }
  }
  /// create product (digis & errors)
  auto collection = std::make_unique<std::vector<CTPPSPixelDigi>>();

  auto errorcollection = std::make_unique<std::vector<CTPPSPixelDataError>>();

  if (data_exist) {
    mapping = es.getHandle(CTPPSPixelDAQMapping_);

    fedIds_ = mapping->fedIds();

    CTPPSPixelDataFormatter formatter(mapping->ROCMapping, eSummary_);
    formatter.setErrorStatus(includeErrors_);

    bool errorsInEvent = false;
    std::vector<CTPPSPixelDataError> nodeterrors;

    for (auto aFed = fedIds_.begin(); aFed != fedIds_.end(); ++aFed) {
      int fedId = *aFed;

      edm::LogInfo("CTPPSPixelRawToDigi") << " PRODUCE DIGI FOR FED: " << dec << fedId << endl;

      CTPPSPixelDataFormatter::Errors errors;
      /// get event data for this fed
      const FEDRawData& fedRawData = buffers->FEDData(fedId);

      formatter.interpretRawData(isRun3_, errorsInEvent, fedId, fedRawData, *collection, errors);

      if (includeErrors_) {
        for (auto const& errorsByDetId : errors) {
          uint32_t errordetid = errorsByDetId.first;
          /// errors given dummy detId must be sorted by Fed
          if (errordetid == RPixErrorChecker::dummyDetId) {
            nodeterrors.insert(nodeterrors.end(), errorsByDetId.second.begin(), errorsByDetId.second.end());
          } else {
            errorcollection->insert(errorcollection->end(), errorsByDetId.second.begin(), errorsByDetId.second.end());
          }
        }
      }
    }

    if (includeErrors_) {
      errorcollection->insert(errorcollection->end(), nodeterrors.begin(), nodeterrors.end());
    }
    if (errorsInEvent)
      LogDebug("CTPPSPixelRawToDigi") << "Error words were stored in this event";
  }
  ///send digis and errors back to framework
  ev.put(std::move(collection));

  if (includeErrors_) {
    ev.put(std::move(errorcollection));
  }
}

void CTPPSPixelRawToDigi::endStream() { eSummary_.printSummary(); }

DEFINE_FWK_MODULE(CTPPSPixelRawToDigi);
