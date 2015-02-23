/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

#include <vector>
#include <string>

#include <OUCommon/Delegate.h>

#include <TFTrading/Symbol.h>

#include "IQFeedMessages.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class IQFeedProvider;

class IQFeedSymbol : public Symbol<IQFeedSymbol> {
  friend class IQFeedProvider;
public:

  typedef Symbol<IQFeedSymbol> inherited_t;
  typedef inherited_t::pInstrument_t pInstrument_t;
  typedef std::string symbol_id_t;

  // Public for RowKeyValues.  Pass in a structure sometime.
  // Public for CVuChartArmsIntraDay.  Pass in structure sometime.
  // Fundamentals
  std::string m_sOptionRoots;
  int m_AverageVolume;
  std::string m_sCompanyName;
  int m_Precision;
  double m_dblHistoricalVolatility;
  double m_dblStrikePrice;
  int m_nShortInterest;
  double m_dblPriceEarnings;
  double m_dbl52WkHi;
  double m_dbl52WkLo;
  double m_dblDividendAmount;
  double m_dblDividendRate;
  double m_dblDividendYield;
  date m_dateExDividend;

  // Update/Summary
  ptime m_dtLastTrade;
  double m_dblTrade;
  double m_dblChange; // last - close
  int m_nTotalVolume;
  int m_nTradeSize;
  double m_dblHigh;
  double m_dblLow;
  double m_dblBid;
  double m_dblAsk;
  int m_nBidSize;
  int m_nAskSize;
  int m_nOpenInterest;
  double m_dblOpen;
  double m_dblClose;
  int m_cntTrades;
  bool m_bNewTrade;
  bool m_bNewQuote;
  bool m_bNewOpen;

  IQFeedSymbol(const std::string &symbol, pInstrument_t pInstrument);
  ~IQFeedSymbol(void);

  ou::Delegate<IQFeedSymbol&> OnFundamentalMessage;
  ou::Delegate<IQFeedSymbol&> OnUpdateMessage;
  ou::Delegate<IQFeedSymbol&> OnSummaryMessage;
  ou::Delegate<IQFeedSymbol&> OnNewsMessage;

protected:

  unsigned short m_cnt;  // used for watch/unwatch
  enum enumQStatus { qUnknown, qFound, qNotFound } m_QStatus;

  void SetQuoteTradeWatchInProgress( void ) { m_bQuoteTradeWatchInProgress = true; };
  void ResetQuoteTradeWatchInProgress( void ) { m_bQuoteTradeWatchInProgress = false; };
  bool GetQuoteTradeWatchInProgress( void ) { return m_bQuoteTradeWatchInProgress; };
  bool m_bQuoteTradeWatchInProgress;

  void SetDepthWatchInProgress( void ) { m_bDepthWatchInProgress = true; };
  void ResetDepthWatchInProgress( void ) { m_bDepthWatchInProgress = false; };
  bool GetDepthWatchInProgress( void ) { return m_bDepthWatchInProgress; };
  bool m_bDepthWatchInProgress;

  void HandleFundamentalMessage( IQFFundamentalMessage *pMsg );
  void HandleUpdateMessage( IQFUpdateMessage *pMsg );
  void HandleSummaryMessage( IQFSummaryMessage *pMsg );
  void HandleNewsMessage( IQFNewsMessage *pMsg );

  template <typename T>
  void DecodePricingMessage( IQFPricingMessage<T> *pMsg );

private:
};

} // namespace tf
} // namespace ou
