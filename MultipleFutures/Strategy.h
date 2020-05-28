/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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
/*
 * File:    Strategy.h
 * Author:  raymond@burkholder.net
 * Project: MultipleFutures
 * Created on March 22, 2020, 18:30
 */

#pragma once

#include <map>

#include <OUCharting/ChartDVBasics.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Order.h>
#include <TFTrading/Position.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/DailyTradeTimeFrames.h>

#include <TFInteractiveBrokers/IBTWS.h>

#include <TFIndicators/TSSWStochastic.h>
#include <TFIndicators/TSSWSMA.h>

class Strategy:
  public ou::ChartDVBasics,
  public ou::tf::DailyTradeTimeFrame<Strategy>
{
  friend class ou::tf::DailyTradeTimeFrame<Strategy>;
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;

  Strategy( pWatch_t, uint16_t nSecondsPerBar );
  virtual ~Strategy();

  void HandleButtonUpdate() {}
  //void HandleButtonSend();
  void HandleButtonCancel();

  void EmitBarSummary();

protected:
private:

  using pIB_t = ou::tf::IBTWS::pProvider_t;
  using pOrder_t = ou::tf::Order::pOrder_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  uint16_t m_nSecondsPerBar;

  using mapCounts_t = std::map<ou::tf::Trade::price_t,size_t>;
  mapCounts_t m_mapPrices;

  ou::ChartEntryShape m_ceStop;
  ou::ChartEntryIndicator m_ceStochastic;
  ou::ChartEntryIndicator m_ceStochasticSmoothed;
  ou::ChartEntryIndicator m_ceStochasticSize;
  ou::ChartEntryMark m_ceStochasticLimits;
  ou::ChartEntryIndicator m_cePositionPL;
  ou::ChartEntryMark m_cePositionPLZero;

  struct Trade {
    ou::tf::OrderSide::enumOrderSide side;
    double entry;
    double offset;
    double stop;
    double trail;
    double tick;
    Trade(): entry {}, stop {}, trail {}, offset {}, side( ou::tf::OrderSide::Unknown ) {}
    Trade( ou::tf::OrderSide::enumOrderSide side_, double entry_, double offset_ )
    : Trade()
    {
      side = side_;
      offset = offset_;
      switch ( side ) {
        case ou::tf::OrderSide::enumOrderSide::Buy:
          break;
        case ou::tf::OrderSide::enumOrderSide::Sell:
          break;
      }
    }
    void Emit( std::ostream& io ) {
      io << "trade: " << entry << "," << offset << "," << stop << "," << trail;;
    }
  };
  Trade m_trade;

  struct Results {
    unsigned int cntOrders; // should match cntWins + cntLosses
    unsigned int cntWins;
    unsigned int cntLosses;
    double dblProfit;
    double dblLoss;
    Results(): cntOrders {}, cntWins {}, cntLosses {}, dblProfit {}, dblLoss {} {}
  };

  struct OrderResults {
    unsigned int cntInstances; // count of bar transitions
    Results longs;
    Results shorts;
    OrderResults(): cntInstances {} {}
  };

  size_t m_cntBars;
  ou::tf::Bar m_barLast;

  pIB_t m_pIB;
  pWatch_t m_pWatch;

  ou::tf::Trade m_tradeLast;
  ou::tf::Quote m_quoteLast;

  double m_dblAverageBarSize;
  ou::tf::BarFactory m_bfBar;

  pOrder_t m_pOrderEntry;
  //pOrder_t m_pOrderProfit;
  //pOrder_t m_pOrderStop;
  pOrder_t m_pOrderExit;

  pPosition_t m_pPosition;

  enum class EState { initial, entry_wait, entry_filling, entry_cancelling, exit_tracking, exit_filling, cancel_wait, quiesce };
  EState m_state;

  enum class EExitType { exit, stop };
  //EExitType m_typeExit;

  TimeFrame m_tfLatest;

  ou::tf::TSSWStochastic m_stochastic;  // TODO: need to add SMA to K

  ou::tf::Prices m_tsK;
  ou::tf::TSSWSMA<ou::tf::Prices> m_smaK;

  double m_curK;
  const double m_upperK0; // hysteresis cross up
  const double m_upperK1; // hysteresis cross down
  const double m_upperK2; // hysteresis mid
  const double m_middle;
  const double m_lowerK2; // hysteresis mid
  const double m_lowerK1; // hysteresis cross up
  const double m_lowerK0; // hysteresis cross down

  enum class EStateStochastic1 {
    Quiesced, WaitForFirstCrossing, WaitForNeutral,
    WaitForHiCrossUp, HiCrossedUp, //HiCrossedDown,
    WaitForLoCrossDown, LoCrossedDown, //LoCrossedUp
    };
  EStateStochastic1 m_stateStochastic1;

  enum class EStochastic2 {
    quiesced, wait,
    upper0, upper1, upper2,
    middle,
    lower0, lower1, lower2
  };
  EStochastic2 m_stateStochastic2;

  EStochastic2 Stochastic2( double );

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void UpdateStochasticSmoothed1( const ou::tf::Price& );
  void UpdateStochasticSmoothed2( const ou::tf::Price& );
  void AdjustBuyStop( double K );
  void AdjustSellStop( double K );

  void HandleBarComplete( const ou::tf::Bar& );

  void StopTest1( const ou::tf::Quote& quote );

  void Entry( ou::tf::OrderSide::enumOrderSide );
  void Entry1( ou::tf::OrderSide::enumOrderSide );
  void Entry2( ou::tf::OrderSide::enumOrderSide );
  void Exit( EExitType typeExit, ou::tf::Quote::dt_t, double exit, const std::string& sComment );
  void CancelOrders();

  void Entry2Buy(  const ou::tf::Price& smoothedK, const std::string& sComment );
  void Entry2Sell( const ou::tf::Price& smoothedK, const std::string& sComment );

  void HandleRHTrading( const ou::tf::Bar& );
  void HandleCancelling( const ou::tf::Bar& );
  void HandleGoingNeutral( const ou::tf::Bar& );

  void HandleRHTrading( const ou::tf::Quote& );
  void HandleCancelling( const ou::tf::Quote& ) {}
  void HandleGoingNeutral( const ou::tf::Quote& ) {}

  void HandleOrderCancelled( const ou::tf::Order& );
  void HandleEntryOrderFilled( const ou::tf::Order& );
  void HandleExitOrderFilled( const ou::tf::Order& );
  void HandleStopOrderFilled( const ou::tf::Order& );

  void HandleUnRealizedPL( const ou::tf::Position::PositionDelta_delegate_t& );  // unrealized p/l
  void HandleExecution( const ou::tf::Position::PositionDelta_delegate_t& );  // realized p/l

};
