#include "sierrachart.h"

SCDLLName("ATR VWAP Trading Bot")

SCSFExport scsf_ATRBasedTradingBotEnhancedPlus(SCStudyInterfaceRef sc)
{
    // Subgraphs
    SCSubgraphRef BuySignal = sc.Subgraph[0];
    SCSubgraphRef SellSignal = sc.Subgraph[1];
    
    // Inputs
    SCInputRef ATRPeriod = sc.Input[0];
    SCInputRef EnableTrading = sc.Input[1];
    SCInputRef TargetMultiplier = sc.Input[2];
    SCInputRef StopMultiplier = sc.Input[3];
    SCInputRef RiskPercentage = sc.Input[4];
    SCInputRef MinATRThreshold = sc.Input[5];
    SCInputRef MaxDailyLoss = sc.Input[6];
    SCInputRef TradingStartTime = sc.Input[7];
    SCInputRef TradingEndTime = sc.Input[8];
    SCInputRef TickMoveThreshold = sc.Input[9];
    SCInputRef WaitSeconds = sc.Input[10];
    SCInputRef VolumeThreshold = sc.Input[11];
    SCInputRef VWAPBandPeriod = sc.Input[12];
    SCInputRef ATRSlopePeriod = sc.Input[13];
    SCInputRef MinShortSlope = sc.Input[14];

    // Persistent variables
    SCPersistentInt& LastTradeBarIndex = sc.PersistVars->i1;
    SCPersistentInt& TradeActive = sc.PersistVars->i2;
    SCPersistentFloat& DailyPnL = sc.PersistVars->f1;
    SCPersistentInt& LastDayProcessed = sc.PersistVars->i3;
    SCPersistentInt& TradeCount = sc.PersistVars->i4;
    SCPersistentFloat& TotalProfit = sc.PersistVars->f5;
    SCPersistentInt& WinningTrades = sc.PersistVars->i5;
    SCPersistentInt& LosingTrades = sc.PersistVars->i6;
    SCPersistentFloat& MaxDrawdown = sc.PersistVars->f6;
    SCPersistentFloat& PeakEquity = sc.PersistVars->f7;
    SCPersistentFloat& LastBuyPrice = sc.PersistVars->f8;
    SCPersistentFloat& LastSellPrice = sc.PersistVars->f9;
    SCPersistentDouble& LastBuyTime = sc.PersistVars->d1;
    SCPersistentDouble& LastSellTime = sc.PersistVars->d2;
    SCPersistentInt& BuyVolumeTrigger = sc.PersistVars->i7;
    SCPersistentInt& SellVolumeTrigger = sc.PersistVars->i8;
    SCPersistentInt& OriginalPositionSize = sc.PersistVars->i9;
    SCPersistentInt& HalfClosed = sc.PersistVars->i10;

    if (sc.SetDefaults)
    {
        sc.GraphName = "ATR VWAP Trading Bot";
        sc.AutoLoop = 1;
        sc.FreeDLL = 0;

        ATRPeriod.Name = "ATR Period"; ATRPeriod.SetInt(14); ATRPeriod.SetIntLimits(1, 100);
        EnableTrading.Name = "Enable Trading"; EnableTrading.SetYesNo(0);
        TargetMultiplier.Name = "Target ATR Multiplier"; TargetMultiplier.SetFloat(2.0f); TargetMultiplier.SetFloatLimits(0.1f, 10.0f);
        StopMultiplier.Name = "Stop ATR Multiplier"; StopMultiplier.SetFloat(1.0f); StopMultiplier.SetFloatLimits(0.1f, 10.0f);
        RiskPercentage.Name = "Risk Percentage per Trade"; RiskPercentage.SetFloat(1.0f); RiskPercentage.SetFloatLimits(0.1f, 10.0f);
        MinATRThreshold.Name = "Minimum ATR Threshold"; MinATRThreshold.SetFloat(0.0001f); MinATRThreshold.SetFloatLimits(0.0001f, 10.0f);
        MaxDailyLoss.Name = "Max Daily Loss Percentage"; MaxDailyLoss.SetFloat(2.0f); MaxDailyLoss.SetFloatLimits(0.1f, 20.0f);
        TradingStartTime.Name = "Trading Start Time (HHMM)"; TradingStartTime.SetTime(HMS_TIME(9, 30, 0));
        TradingEndTime.Name = "Trading End Time (HHMM)"; TradingEndTime.SetTime(HMS_TIME(16, 0, 0));
        TickMoveThreshold.Name = "Tick Move Threshold"; TickMoveThreshold.SetInt(4); TickMoveThreshold.SetIntLimits(1, 20);
        WaitSeconds.Name = "Wait Seconds"; WaitSeconds.SetInt(10); WaitSeconds.SetIntLimits(1, 60);
        VolumeThreshold.Name = "Minimum Volume Threshold"; VolumeThreshold.SetInt(100); VolumeThreshold.SetIntLimits(1, 10000);
        VWAPBandPeriod.Name = "VWAP Band Period"; VWAPBandPeriod.SetInt(252); VWAPBandPeriod.SetIntLimits(1, 1000);
        ATRSlopePeriod.Name = "ATR Slope Period"; ATRSlopePeriod.SetInt(5); ATRSlopePeriod.SetIntLimits(2, 50);
        MinShortSlope.Name = "Min Negative Slope for Shorts (degrees)"; MinShortSlope.SetFloat(-30.0f); MinShortSlope.SetFloatLimits(-90.0f, 0.0f);

        BuySignal.Name = "Buy Signal"; BuySignal.DrawStyle = DRAWSTYLE_ARROW_UP; BuySignal.PrimaryColor = RGB(0, 255, 0); BuySignal.LineWidth = 2;
        SellSignal.Name = "Sell Signal"; SellSignal.DrawStyle = DRAWSTYLE_ARROW_DOWN; SellSignal.PrimaryColor = RGB(255, 0, 0); SellSignal.LineWidth = 2;

        TotalProfit = 0.0f; WinningTrades = 0; LosingTrades = 0; MaxDrawdown = 0.0f; PeakEquity = 0.0f;
        LastBuyPrice = 0.0f; LastSellPrice = 0.0f; LastBuyTime = 0.0; LastSellTime = 0.0;
        BuyVolumeTrigger = 0; SellVolumeTrigger = 0;
        OriginalPositionSize = 0; HalfClosed = 0;

        return;
    }

    // Technical calculations
    SCFloatArray TrueRange; sc.TrueRange(TrueRange);
    SCFloatArray ATR; sc.MovingAverage(TrueRange, ATR, MOVAVGTYPE_SIMPLE, ATRPeriod.GetInt());
    if (ATR[sc.Index] == 0 || ATR[sc.Index] < MinATRThreshold.GetFloat()) return;

    // VWAP and Bands
    SCFloatArray VWAP; sc.VWAP(VWAP, VWAPBandPeriod.GetInt());
    SCFloatArray VWAPStdDev; sc.StandardDeviation(sc.Close, VWAPStdDev, VWAPBandPeriod.GetInt());
    float VWAPValue = VWAP[sc.Index];
    float StdDev = VWAPStdDev[sc.Index];
    
    float VWAPBand1Lower = VWAPValue - StdDev;
    float VWAPBand2Lower = VWAPValue - 2.0f * StdDev;
    float VWAPBand3Lower = VWAPValue - 3.0f * StdDev;
    float VWAPBand4Lower = VWAPValue - 4.0f * StdDev;
    float VWAPBand1Upper = VWAPValue + StdDev;
    float VWAPBand2Upper = VWAPValue + 2.0f * StdDev;
    float VWAPBand3Upper = VWAPValue + 3.0f * StdDev;
    float VWAPBand4Upper = VWAPValue + 4.0f * StdDev;

    // ATR Slope Calculation
    float ATRSlope = 0.0f;
    if (sc.Index >= ATRSlopePeriod.GetInt())
    {
        float ATRChange = ATR[sc.Index] - ATR[sc.Index - ATRSlopePeriod.GetInt()];
        float SlopeRadians = atan2(ATRChange, ATRSlopePeriod.GetInt() * sc.TickSize);
        ATRSlope = SlopeRadians * (180.0f / 3.14159f);
    }

    // Time handling
    SCDateTime CurrentTime = sc.CurrentSystemDateTime;
    double CurrentTimeSeconds = CurrentTime.GetTimeAsDouble();
    int CurrentDay = CurrentTime.GetDayOfYear();
    int CurrentMinutes = CurrentTime.GetHour() * 60 + CurrentTime.GetMinute();
    int StartMinutes = HM_TIME_TO_MINUTES(TradingStartTime.GetTime());
    int EndMinutes = HM_TIME_TO_MINUTES(TradingEndTime.GetTime());

    if (LastDayProcessed != CurrentDay)
    {
        DailyPnL = 0.0f;
        LastDayProcessed = CurrentDay;
    }

    float MiddleATR = ATR[sc.Index] * 0.5f;
    float ClosingPrice = sc.Close[sc.Index];
    float TickSize = sc.TickSize;
    
    // Position and account management
    s_SCPositionData PositionData;
    sc.GetTradePosition(PositionData);
    float AccountBalance = sc.AccountBalance;
    
    float RiskPerTrade = AccountBalance * (RiskPercentage.GetFloat() / 100.0f);
    float StopDistance = ATR[sc.Index] * StopMultiplier.GetFloat();
    int PositionSize = max(1, static_cast<int>(RiskPerTrade / (StopDistance * sc.CurrencyValuePerTick)));

    // Volume tracking
    float CurrentBidPrice = sc.Bid;
    float CurrentAskPrice = sc.Ask;
    int RecentBuyVolume = sc.GetRecentAskVolumeAtPrice(CurrentAskPrice);
    int RecentSellVolume = sc.GetRecentBidVolumeAtPrice(CurrentBidPrice);

    char volMessage[256];
    sprintf_s(volMessage, "BidVol=%d at %.4f, AskVol=%d at %.4f", 
             RecentSellVolume, CurrentBidPrice, RecentBuyVolume, CurrentAskPrice);
    sc.AddMessageToLog(volMessage, 0);

    if (RecentBuyVolume >= VolumeThreshold.GetInt() && LastBuyPrice == 0.0f)
    {
        LastBuyPrice = CurrentAskPrice;
        LastBuyTime = CurrentTimeSeconds;
        BuyVolumeTrigger = 1;
        sprintf_s(volMessage, "Buy Volume Triggered: %d at %.4f", RecentBuyVolume, LastBuyPrice);
        sc.AddMessageToLog(volMessage, 0);
    }
    
    if (RecentSellVolume >= VolumeThreshold.GetInt() && LastSellPrice == 0.0f)
    {
        LastSellPrice = CurrentBidPrice;
        LastSellTime = CurrentTimeSeconds;
        SellVolumeTrigger = 1;
        sprintf_s(volMessage, "Sell Volume Triggered: %d at %.4f", RecentSellVolume, LastSellPrice);
        sc.AddMessageToLog(volMessage, 0);
    }

    // Check price movement and timing
    bool BuyConditionMet = false;
    bool SellConditionMet = false;
    
    if (BuyVolumeTrigger)
    {
        float PriceMoveUp = ClosingPrice - LastBuyPrice;
        if (PriceMoveUp >= TickMoveThreshold.GetInt() * TickSize)
        {
            double TimeElapsed = CurrentTimeSeconds - LastBuyTime;
            if (TimeElapsed >= WaitSeconds.GetInt())
            {
                BuyConditionMet = true;
            }
        }
        else if (PriceMoveUp <= 0)
        {
            LastBuyPrice = 0.0f;
            BuyVolumeTrigger = 0;
            sc.AddMessageToLog("Buy Trigger Reset: Price Returned", 0);
        }
    }

    if (SellVolumeTrigger)
    {
        float PriceMoveDown = LastSellPrice - ClosingPrice;
        if (PriceMoveDown >= TickMoveThreshold.GetInt() * TickSize)
        {
            double TimeElapsed = CurrentTimeSeconds - LastSellTime;
            if (TimeElapsed >= WaitSeconds.GetInt())
            {
                SellConditionMet = true;
            }
        }
        else if (PriceMoveDown <= 0)
        {
            LastSellPrice = 0.0f;
            SellVolumeTrigger = 0;
            sc.AddMessageToLog("Sell Trigger Reset: Price Returned", 0);
        }
    }

    // Manage exits
    if (TradeActive && PositionData.PositionQuantity != 0)
    {
        int HalfSize = OriginalPositionSize / 2;
        float EntryPrice = PositionData.AveragePrice;

        if (PositionData.PositionQuantity > 0) // Long position
        {
            float ProfitATR = ClosingPrice - EntryPrice;
            float ProfitChannels = (ClosingPrice - VWAPValue) / StdDev;

            // Close half at 1 ATR profit
            if (!HalfClosed && ProfitATR >= ATR[sc.Index])
            {
                s_SCNewOrder ExitOrder;
                ExitOrder.OrderQuantity = HalfSize;
                ExitOrder.OrderType = SCT_ORDERTYPE_MARKET;
                sc.SellExit(ExitOrder);
                HalfClosed = 1;
                float TradeProfit = (ClosingPrice - EntryPrice) * HalfSize * sc.CurrencyValuePerTick;
                TotalProfit += TradeProfit;
                DailyPnL += TradeProfit;
                PeakEquity = max(PeakEquity, TotalProfit);
                MaxDrawdown = max(MaxDrawdown, PeakEquity - TotalProfit);
                char message[256];
                sprintf_s(message, "Long Half Closed (1 ATR): Profit=%.2f, Price=%.4f", TradeProfit, ClosingPrice);
                sc.AddMessageToLog(message, 0);
            }
            // Close remaining half at 2 channels profit
            else if (HalfClosed && ProfitChannels >= 2.0f)
            {
                sc.FlattenAndCancelAllOrders();
                TradeActive = 0;
                HalfClosed = 0;
                OriginalPositionSize = 0;
                float TradeProfit = (ClosingPrice - EntryPrice) * HalfSize * sc.CurrencyValuePerTick;
                TotalProfit += TradeProfit;
                DailyPnL += TradeProfit;
                PeakEquity = max(PeakEquity, TotalProfit);
                MaxDrawdown = max(MaxDrawdown, PeakEquity - TotalProfit);
                if (TradeProfit > 0) WinningTrades++;
                else if (TradeProfit < 0) LosingTrades++;
                char message[256];
                sprintf_s(message, "Long Full Closed (2 Channels): Profit=%.2f, Price=%.4f", TradeProfit, ClosingPrice);
                sc.AddMessageToLog(message, 0);
            }
        }
        else if (PositionData.PositionQuantity < 0) // Short position
        {
            float ProfitATR = EntryPrice - ClosingPrice;
            float ProfitChannels = (VWAPValue - ClosingPrice) / StdDev;

            // Close half at 1 ATR profit
            if (!HalfClosed && ProfitATR >= ATR[sc.Index])
            {
                s_SCNewOrder ExitOrder;
                ExitOrder.OrderQuantity = HalfSize;
                ExitOrder.OrderType = SCT_ORDERTYPE_MARKET;
                sc.BuyExit(ExitOrder);
                HalfClosed = 1;
                float TradeProfit = (EntryPrice - ClosingPrice) * HalfSize * sc.CurrencyValuePerTick;
                TotalProfit += TradeProfit;
                DailyPnL += TradeProfit;
                PeakEquity = max(PeakEquity, TotalProfit);
                MaxDrawdown = max(MaxDrawdown, PeakEquity - TotalProfit);
                char message[256];
                sprintf_s(message, "Short Half Closed (1 ATR): Profit=%.2f, Price=%.4f", TradeProfit, ClosingPrice);
                sc.AddMessageToLog(message, 0);
            }
            // Close remaining half at 2 channels profit
            else if (HalfClosed && ProfitChannels >= 2.0f)
            {
                sc.FlattenAndCancelAllOrders();
                TradeActive = 0;
                HalfClosed = 0;
                OriginalPositionSize = 0;
                float TradeProfit = (EntryPrice - ClosingPrice) * HalfSize * sc.CurrencyValuePerTick;
                TotalProfit += TradeProfit;
                DailyPnL += TradeProfit;
                PeakEquity = max(PeakEquity, TotalProfit);
                MaxDrawdown = max(MaxDrawdown, PeakEquity - TotalProfit);
                if (TradeProfit > 0) WinningTrades++;
                else if (TradeProfit < 0) LosingTrades++;
                char message[256];
                sprintf_s(message, "Short Full Closed (2 Channels): Profit=%.2f, Price=%.4f", TradeProfit, ClosingPrice);
                sc.AddMessageToLog(message, 0);
            }
        }
    }
    else if (TradeActive && PositionData.PositionQuantity == 0 && PositionData.LastFillOrderID != 0)
    {
        s_SCTradingOrderFillData LastFill;
        if (sc.GetOrderFillEntry(PositionData.LastFillOrderID, LastFill) != 0)
        {
            float TradeProfit = (LastFill.FillPrice - PositionData.AveragePrice) * 
                              PositionData.LastFillQuantity * sc.CurrencyValuePerTick * 
                              (PositionData.PositionQuantity > 0 ? 1 : -1);
            TotalProfit += TradeProfit;
            DailyPnL += TradeProfit;
            PeakEquity = max(PeakEquity, TotalProfit);
            MaxDrawdown = max(MaxDrawdown, PeakEquity - TotalProfit);
            if (TradeProfit > 0) WinningTrades++;
            else if (TradeProfit < 0) LosingTrades++;
            char message[256];
            sprintf_s(message, "Trade Closed (Stop/Other): Profit=%.2f, TotalProfit=%.2f", TradeProfit, TotalProfit);
            sc.AddMessageToLog(message, 0);
            TradeActive = 0;
            HalfClosed = 0;
            OriginalPositionSize = 0;
        }
    }

    // Trading conditions
    if (!EnableTrading.GetYesNo() || 
        LastTradeBarIndex == sc.Index ||
        CurrentMinutes < StartMinutes ||
        CurrentMinutes >= EndMinutes ||
        (DailyPnL < 0 && fabs(DailyPnL) > (AccountBalance * (MaxDailyLoss.GetFloat() / 100.0f))) ||
        PositionData.PositionQuantity != 0)
    {
        return;
    }

    s_SCNewOrder NewOrder;
    NewOrder.OrderQuantity = PositionSize;
    NewOrder.OrderType = SCT_ORDERTYPE_MARKET;
    NewOrder.Target1Offset = ATR[sc.Index] * TargetMultiplier.GetFloat();
    NewOrder.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT;
    NewOrder.StopAllOffset = ATR[sc.Index];
    NewOrder.AttachedOrderStopAllType = SCT_ORDERTYPE_STOP;

    int Result = 0;
    char message[256];
    
    // Trading logic
    if (ClosingPrice <= VWAPBand2Lower && ClosingPrice > VWAPBand3Lower && 
        ATRSlope > 30.0f && BuyConditionMet)
    {
        BuySignal[sc.Index] = ClosingPrice;
        NewOrder.StopAllOffset = ATR[sc.Index];
        Result = sc.BuyEntry(NewOrder);
        if (Result > 0)
        {
            LastTradeBarIndex = sc.Index;
            TradeActive = 1;
            TradeCount++;
            LastBuyPrice = 0.0f;
            BuyVolumeTrigger = 0;
            OriginalPositionSize = PositionSize;
            HalfClosed = 0;
            sprintf_s(message, "Buy Order #%d (2-3 Lower): Size=%d, Price=%.4f, ATR=%.4f, Slope=%.2f", 
                     TradeCount, PositionSize, ClosingPrice, ATR[sc.Index], ATRSlope);
            sc.AddMessageToLog(message, 0);
        }
        else if (Result < 0)
        {
            sprintf_s(message, "Buy Order Failed: Error %d", Result);
            sc.AddMessageToLog(message, 1);
        }
    }
    else if (ClosingPrice <= VWAPBand3Lower && ClosingPrice > VWAPBand4Lower && BuyConditionMet)
    {
        BuySignal[sc.Index] = ClosingPrice;
        Result = sc.BuyEntry(NewOrder);
        if (Result > 0)
        {
            LastTradeBarIndex = sc.Index;
            TradeActive = 1;
            TradeCount++;
            LastBuyPrice = 0.0f;
            BuyVolumeTrigger = 0;
            OriginalPositionSize = PositionSize;
            HalfClosed = 0;
            sprintf_s(message, "Buy Order #%d (3-4 Lower): Size=%d, Price=%.4f, ATR=%.4f", 
                     TradeCount, PositionSize, ClosingPrice, ATR[sc.Index]);
            sc.AddMessageToLog(message, 0);
        }
        else if (Result < 0)
        {
            sprintf_s(message, "Buy Order Failed: Error %d", Result);
            sc.AddMessageToLog(message, 1);
        }
    }
    else if (ClosingPrice >= VWAPBand3Upper && ClosingPrice < VWAPBand4Upper && 
             ATRSlope < MinShortSlope.GetFloat() && SellConditionMet)
    {
        SellSignal[sc.Index] = ClosingPrice;
        Result = sc.SellEntry(NewOrder);
        if (Result > 0)
        {
            LastTradeBarIndex = sc.Index;
            TradeActive = 1;
            TradeCount++;
            LastSellPrice = 0.0f;
            SellVolumeTrigger = 0;
            OriginalPositionSize = PositionSize;
            HalfClosed = 0;
            sprintf_s(message, "Sell Order #%d (3-4 Upper): Size=%d, Price=%.4f, ATR=%.4f, Slope=%.2f", 
                     TradeCount, PositionSize, ClosingPrice, ATR[sc.Index], ATRSlope);
            sc.AddMessageToLog(message, 0);
        }
        else if (Result < 0)
        {
            sprintf_s(message, "Sell Order Failed: Error %d", Result);
            sc.AddMessageToLog(message, 1);
        }
    }
    else if (ClosingPrice >= VWAPBand2Upper && ClosingPrice < VWAPBand3Upper && 
             ATRSlope < MinShortSlope.GetFloat() && SellConditionMet)
    {
        SellSignal[sc.Index] = ClosingPrice;
        NewOrder.StopAllOffset = ATR[sc.Index];
        Result = sc.SellEntry(NewOrder);
        if (Result > 0)
        {
            LastTradeBarIndex = sc.Index;
            TradeActive = 1;
            TradeCount++;
            LastSellPrice = 0.0f;
            SellVolumeTrigger = 0;
            OriginalPositionSize = PositionSize;
            HalfClosed = 0;
            sprintf_s(message, "Sell Order #%d (2-3 Upper): Size=%d, Price=%.4f, ATR=%.4f, Slope=%.2f", 
                     TradeCount, PositionSize, ClosingPrice, ATR[sc.Index], ATRSlope);
            sc.AddMessageToLog(message, 0);
        }
        else if (Result < 0)
        {
            sprintf_s(message, "Sell Order Failed: Error %d", Result);
            sc.AddMessageToLog(message, 1);
        }
    }

    // Final stats
    if (sc.Index == sc.ArraySize - 1)
    {
        float WinRate = TradeCount > 0 ? (float)WinningTrades / (WinningTrades + LosingTrades) * 100.0f : 0.0f;
        sprintf_s(message, "Backtest Results: TotalProfit=%.2f, Trades=%d, WinRate=%.1f%%, MaxDD=%.2f", 
                 TotalProfit, TradeCount, WinRate, MaxDrawdown);
        sc.AddMessageToLog(message, 0);
    }
}
