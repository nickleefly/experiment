#include "sierrachart.h"

SCDLLName("ATR VWMA Trading Bot")

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
    SCInputRef VWMAPeriod = sc.Input[12];
    SCInputRef ATRSlopePeriod = sc.Input[13];
    SCInputRef MinShortSlope = sc.Input[14];
    SCInputRef FirstTargetATR = sc.Input[15];
    SCInputRef SecondTargetATR = sc.Input[16];

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
    SCPersistentInt& TradeType = sc.PersistVars->i11;  // Trade type: 1 = crossing long, 2 = crossing short, 3 = ATR bottom, 4 = ATR top, 5 = slope long, 6 = slope short

    if (sc.SetDefaults)
    {
        sc.GraphName = "ATR VWMA Trading Bot";
        sc.AutoLoop = 1;
        sc.FreeDLL = 0;

        ATRPeriod.Name = "ATR Period";
        ATRPeriod.SetInt(14);
        ATRPeriod.SetIntLimits(1, 100);
        EnableTrading.Name = "Enable Trading";
        EnableTrading.SetYesNo(0);
        TargetMultiplier.Name = "Target ATR Multiplier (Legacy)";
        TargetMultiplier.SetFloat(2.0f);
        TargetMultiplier.SetFloatLimits(0.1f, 10.0f);
        StopMultiplier.Name = "Stop ATR Multiplier";
        StopMultiplier.SetFloat(1.0f);
        StopMultiplier.SetFloatLimits(0.1f, 10.0f);
        RiskPercentage.Name = "Risk Percentage per Trade";
        RiskPercentage.SetFloat(1.0f);
        RiskPercentage.SetFloatLimits(0.1f, 10.0f);
        MinATRThreshold.Name = "Minimum ATR Threshold";
        MinATRThreshold.SetFloat(0.0001f);
        MinATRThreshold.SetFloatLimits(0.0001f, 10.0f);
        MaxDailyLoss.Name = "Max Daily Loss Percentage";
        MaxDailyLoss.SetFloat(2.0f);
        MaxDailyLoss.SetFloatLimits(0.1f, 20.0f);
        TradingStartTime.Name = "Trading Start Time (HHMM)";
        TradingStartTime.SetTime(HMS_TIME(9, 30, 0));
        TradingEndTime.Name = "Trading End Time (HHMM)";
        TradingEndTime.SetTime(HMS_TIME(16, 0, 0));
        TickMoveThreshold.Name = "Tick Move Threshold";
        TickMoveThreshold.SetInt(4);
        TickMoveThreshold.SetIntLimits(1, 20);
        WaitSeconds.Name = "Wait Seconds";
        WaitSeconds.SetInt(10);
        WaitSeconds.SetIntLimits(1, 60);
        VolumeThreshold.Name = "Minimum Volume Threshold";
        VolumeThreshold.SetInt(100);
        VolumeThreshold.SetIntLimits(1, 10000);
        VWMAPeriod.Name = "VWMA Period";
        VWMAPeriod.SetInt(14);
        VWMAPeriod.SetIntLimits(1, 1000);
        ATRSlopePeriod.Name = "ATR Slope Period";
        ATRSlopePeriod.SetInt(5);
        ATRSlopePeriod.SetIntLimits(2, 50);
        MinShortSlope.Name = "Min Negative Slope for Shorts (degrees)";
        MinShortSlope.SetFloat(-30.0f);
        MinShortSlope.SetFloatLimits(-90.0f, 0.0f);
        FirstTargetATR.Name = "First Target ATR (1 or 2)";
        FirstTargetATR.SetInt(1);
        FirstTargetATR.SetIntLimits(1, 2);
        SecondTargetATR.Name = "Second Target ATR (2, 3, or 4)";
        SecondTargetATR.SetInt(2);
        SecondTargetATR.SetIntLimits(2, 4);

        BuySignal.Name = "Buy Signal";
        BuySignal.DrawStyle = DRAWSTYLE_ARROW_UP;
        BuySignal.PrimaryColor = RGB(0, 255, 0);
        BuySignal.LineWidth = 2;
        SellSignal.Name = "Sell Signal";
        SellSignal.DrawStyle = DRAWSTYLE_ARROW_DOWN;
        SellSignal.PrimaryColor = RGB(255, 0, 0);
        SellSignal.LineWidth = 2;

        TotalProfit = 0.0f;
        WinningTrades = 0;
        LosingTrades = 0;
        MaxDrawdown = 0.0f;
        PeakEquity = 0.0f;
        LastBuyPrice = 0.0f;
        LastSellPrice = 0.0f;
        LastBuyTime = 0.0;
        LastSellTime = 0.0;
        BuyVolumeTrigger = 0;
        SellVolumeTrigger = 0;
        OriginalPositionSize = 0;
        HalfClosed = 0;
        TradeType = 0;

        return;
    }

    SCFloatArray TrueRange;
    sc.TrueRange(TrueRange);
    SCFloatArray ATR;
    sc.MovingAverage(TrueRange, ATR, MOVAVGTYPE_SIMPLE, MOVAVGTYPE_SIMPLE, ATRPeriod.GetInt());
    if (ATR[sc.Index] == 0 || ATR[sc.Index] < MinATRThreshold.GetFloat()) return;

    SCFloatArray VWMA;
    sc.VolumeWeightedMovingAverage(sc.Close, VWMA, VWMAPeriod.GetInt());
    SCFloatArray VWMAStdDev;
    sc.StdDeviation(sc.Close, VWMAStdDev, VWMAPeriod.GetInt());
    float VWMAValue = VWMA[sc.Index];
    float StdDev = VWMAStdDev[sc.Index];
    
    float VWMABand1Lower = VWMAValue - StdDev;
    float VWMABand2Lower = VWMAValue - 2.0f * StdDev;
    float VWMABand3Lower = VWMAValue - 3.0f * StdDev;
    float VWMABand4Lower = VWMAValue - 4.0f * StdDev;
    float VWMABand1Upper = VWMAValue + StdDev;
    float VWMABand2Upper = VWMAValue + 2.0f * StdDev;
    float VWMABand3Upper = VWMAValue + 3.0f * StdDev;
    float VWMABand4Upper = VWMAValue + 4.0f * StdDev;

    float ATRSlope = 0.0f;
    if (sc.Index >= ATRSlopePeriod.GetInt())
    {
        float ATRChange = ATR[sc.Index] - ATR[sc.Index - ATRSlopePeriod.GetInt()];
        float SlopeRadians = atan2(ATRChange, ATRSlopePeriod.GetInt() * sc.TickSize);
        ATRSlope = SlopeRadians * (180.0f / 3.14159f);
    }

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
    float PreviousClose = (sc.Index > 0) ? sc.Close[sc.Index - 1] : ClosingPrice;
    float TickSize = sc.TickSize;

    s_SCPositionData PositionData;
    sc.GetTradePosition(PositionData);
    float AccountBalance = sc.AccountBalance;

    float RiskPerTrade = AccountBalance * (RiskPercentage.GetFloat() / 100.0f);
    float StopDistance = ATR[sc.Index] * StopMultiplier.GetFloat();
    int PositionSize = max(1, static_cast<int>(RiskPerTrade / (StopDistance * sc.CurrencyValuePerTick)));

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

    // Manage exits with updated ATR-based targets
    if (TradeActive && PositionData.PositionQuantity != 0)
    {
        int HalfSize = OriginalPositionSize / 2;
        float EntryPrice = PositionData.AveragePrice;

        if (PositionData.PositionQuantity > 0) // Long position
        {
            float ProfitATR = ClosingPrice - EntryPrice;

            // Close half of the long position based on TradeType
            if (!HalfClosed)
            {
                // For TradeType 3 (ATR bottom 3 or 4 channel): Close half at 1 ATR
                if (TradeType == 3 && ProfitATR >= ATR[sc.Index])
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
                    sprintf_s(message, "Long Half Closed (1 ATR, ATR Bottom): Profit=%.2f, Price=%.4f", TradeProfit, ClosingPrice);
                    sc.AddMessageToLog(message, 0);
                }
                // For other TradeTypes (1, 5, or existing): Use configurable FirstTargetATR
                else if (TradeType != 3 && ProfitATR >= (ATR[sc.Index] * FirstTargetATR.GetInt()))
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
                    sprintf_s(message, "Long Half Closed (%d ATR): Profit=%.2f, Price=%.4f", FirstTargetATR.GetInt(), TradeProfit, ClosingPrice);
                    sc.AddMessageToLog(message, 0);
                }
            }
            // Close the remaining half of the long position based on TradeType
            else if (HalfClosed)
            {
                // For TradeType 3 (ATR bottom 3 or 4 channel): Close remaining half at 2 ATR
                if (TradeType == 3 && ProfitATR >= (ATR[sc.Index] * 2))
                {
                    sc.FlattenAndCancelAllOrders();
                    TradeActive = 0;
                    HalfClosed = 0;
                    OriginalPositionSize = 0;
                    TradeType = 0;
                    float TradeProfit = (ClosingPrice - EntryPrice) * HalfSize * sc.CurrencyValuePerTick;
                    TotalProfit += TradeProfit;
                    DailyPnL += TradeProfit;
                    PeakEquity = max(PeakEquity, TotalProfit);
                    MaxDrawdown = max(MaxDrawdown, PeakEquity - TotalProfit);
                    if (TradeProfit > 0) WinningTrades++;
                    else if (TradeProfit < 0) LosingTrades++;
                    char message[256];
                    sprintf_s(message, "Long Full Closed (2 ATR, ATR Bottom): Profit=%.2f, Price=%.4f", TradeProfit, ClosingPrice);
                    sc.AddMessageToLog(message, 0);
                }
                // For other TradeTypes (1, 5, or existing): Use configurable SecondTargetATR
                else if (TradeType != 3 && ProfitATR >= (ATR[sc.Index] * SecondTargetATR.GetInt()))
                {
                    sc.FlattenAndCancelAllOrders();
                    TradeActive = 0;
                    HalfClosed = 0;
                    OriginalPositionSize = 0;
                    TradeType = 0;
                    float TradeProfit = (ClosingPrice - EntryPrice) * HalfSize * sc.CurrencyValuePerTick;
                    TotalProfit += TradeProfit;
                    DailyPnL += TradeProfit;
                    PeakEquity = max(PeakEquity, TotalProfit);
                    MaxDrawdown = max(MaxDrawdown, PeakEquity - TotalProfit);
                    if (TradeProfit > 0) WinningTrades++;
                    else if (TradeProfit < 0) LosingTrades++;
                    char message[256];
                    sprintf_s(message, "Long Full Closed (%d ATR): Profit=%.2f, Price=%.4f", SecondTargetATR.GetInt(), TradeProfit, ClosingPrice);
                    sc.AddMessageToLog(message, 0);
                }
            }
        }
        else if (PositionData.PositionQuantity < 0) // Short position
        {
            float ProfitATR = EntryPrice - ClosingPrice;

            // Close half of the short position based on TradeType
            if (!HalfClosed)
            {
                // For TradeType 4 (ATR top 3 or 4 channel): Close half at 1 ATR
                if (TradeType == 4 && ProfitATR >= ATR[sc.Index])
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
                    sprintf_s(message, "Short Half Closed (1 ATR, ATR Top): Profit=%.2f, Price=%.4f", TradeProfit, ClosingPrice);
                    sc.AddMessageToLog(message, 0);
                }
                // For other TradeTypes (2, 6, or existing): Use configurable FirstTargetATR
                else if (TradeType != 4 && ProfitATR >= (ATR[sc.Index] * FirstTargetATR.GetInt()))
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
                    sprintf_s(message, "Short Half Closed (%d ATR): Profit=%.2f, Price=%.4f", FirstTargetATR.GetInt(), TradeProfit, ClosingPrice);
                    sc.AddMessageToLog(message, 0);
                }
            }
            // Close the remaining half of the short position based on TradeType
            else if (HalfClosed)
            {
                // For TradeType 4 (ATR top 3 or 4 channel): Close remaining half at 2 ATR
                if (TradeType == 4 && ProfitATR >= (ATR[sc.Index] * 2))
                {
                    sc.FlattenAndCancelAllOrders();
                    TradeActive = 0;
                    HalfClosed = 0;
                    OriginalPositionSize = 0;
                    TradeType = 0;
                    float TradeProfit = (EntryPrice - ClosingPrice) * HalfSize * sc.CurrencyValuePerTick;
                    TotalProfit += TradeProfit;
                    DailyPnL += TradeProfit;
                    PeakEquity = max(PeakEquity, TotalProfit);
                    MaxDrawdown = max(MaxDrawdown, PeakEquity - TotalProfit);
                    if (TradeProfit > 0) WinningTrades++;
                    else if (TradeProfit < 0) LosingTrades++;
                    char message[256];
                    sprintf_s(message, "Short Full Closed (2 ATR, ATR Top): Profit=%.2f, Price=%.4f", TradeProfit, ClosingPrice);
                    sc.AddMessageToLog(message, 0);
                }
                // For other TradeTypes (2, 6, or existing): Use configurable SecondTargetATR
                else if (TradeType != 4 && ProfitATR >= (ATR[sc.Index] * SecondTargetATR.GetInt()))
                {
                    sc.FlattenAndCancelAllOrders();
                    TradeActive = 0;
                    HalfClosed = 0;
                    OriginalPositionSize = 0;
                    TradeType = 0;
                    float TradeProfit = (EntryPrice - ClosingPrice) * HalfSize * sc.CurrencyValuePerTick;
                    TotalProfit += TradeProfit;
                    DailyPnL += TradeProfit;
                    PeakEquity = max(PeakEquity, TotalProfit);
                    MaxDrawdown = max(MaxDrawdown, PeakEquity - TotalProfit);
                    if (TradeProfit > 0) WinningTrades++;
                    else if (TradeProfit < 0) LosingTrades++;
                    char message[256];
                    sprintf_s(message, "Short Full Closed (%d ATR): Profit=%.2f, Price=%.4f", SecondTargetATR.GetInt(), TradeProfit, ClosingPrice);
                    sc.AddMessageToLog(message, 0);
                }
            }
        }
    }
    // Handle closure by stop loss or target from attached orders
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
            TradeType = 0;
        }
    }

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
    
    // Existing Trade Logic 1: Long between VWMA -2σ and -3σ with slope > 30°
    if (ClosingPrice <= VWMABand2Lower && ClosingPrice > VWMABand3Lower && 
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
            TradeType = 5;  // Slope long
            sprintf_s(message, "Buy Order #%d (2-3 Lower, Slope): Size=%d, Price=%.4f, ATR=%.4f, Slope=%.2f", 
                     TradeCount, PositionSize, ClosingPrice, ATR[sc.Index], ATRSlope);
            sc.AddMessageToLog(message, 0);
        }
        else if (Result < 0)
        {
            sprintf_s(message, "Buy Order Failed: Error %d", Result);
            sc.AddMessageToLog(message, 1);
        }
    }
    // Existing Trade Logic 2: Long between VWMA -3σ and -4σ
    else if (ClosingPrice <= VWMABand3Lower && ClosingPrice > VWMABand4Lower && BuyConditionMet)
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
            TradeType = 3;  // ATR bottom
            sprintf_s(message, "Buy Order #%d (3-4 Lower, ATR Bottom): Size=%d, Price=%.4f, ATR=%.4f", 
                     TradeCount, PositionSize, ClosingPrice, ATR[sc.Index]);
            sc.AddMessageToLog(message, 0);
        }
        else if (Result < 0)
        {
            sprintf_s(message, "Buy Order Failed: Error %d", Result);
            sc.AddMessageToLog(message, 1);
        }
    }
    // Existing Trade Logic 3: Short between VWMA +3σ and +4σ with slope < MinShortSlope
    else if (ClosingPrice >= VWMABand3Upper && ClosingPrice < VWMABand4Upper && 
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
            TradeType = 4;  // ATR top
            sprintf_s(message, "Sell Order #%d (3-4 Upper, ATR Top): Size=%d, Price=%.4f, ATR=%.4f, Slope=%.2f", 
                     TradeCount, PositionSize, ClosingPrice, ATR[sc.Index], ATRSlope);
            sc.AddMessageToLog(message, 0);
        }
        else if (Result < 0)
        {
            sprintf_s(message, "Sell Order Failed: Error %d", Result);
            sc.AddMessageToLog(message, 1);
        }
    }
    // Existing Trade Logic 4: Short between VWMA +2σ and +3σ with slope < MinShortSlope
    else if (ClosingPrice >= VWMABand2Upper && ClosingPrice < VWMABand3Upper && 
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
            TradeType = 6;  // Slope short
            sprintf_s(message, "Sell Order #%d (2-3 Upper, Slope): Size=%d, Price=%.4f, ATR=%.4f, Slope=%.2f", 
                     TradeCount, PositionSize, ClosingPrice, ATR[sc.Index], ATRSlope);
            sc.AddMessageToLog(message, 0);
        }
        else if (Result < 0)
        {
            sprintf_s(message, "Sell Order Failed: Error %d", Result);
            sc.AddMessageToLog(message, 1);
        }
    }
    // New Trade Logic 1: Long when crossing MiddleATR from bottom (VWMA -1σ) and price moving up
    else if (sc.Index > 0 && PreviousClose <= MiddleATR && ClosingPrice > MiddleATR && 
             ClosingPrice > VWMABand1Lower && ClosingPrice > PreviousClose)
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
            TradeType = 1;  // Crossing long
            sprintf_s(message, "Buy Order #%d (Crossing MiddleATR Up): Size=%d, Price=%.4f, ATR=%.4f", 
                     TradeCount, PositionSize, ClosingPrice, ATR[sc.Index]);
            sc.AddMessageToLog(message, 0);
        }
        else if (Result < 0)
        {
            sprintf_s(message, "Buy Order Failed: Error %d", Result);
            sc.AddMessageToLog(message, 1);
        }
    }
    // New Trade Logic 2: Short when crossing MiddleATR from top (VWMA +1σ) and price moving down
    else if (sc.Index > 0 && PreviousClose >= MiddleATR && ClosingPrice < MiddleATR && 
             ClosingPrice < VWMABand1Upper && ClosingPrice < PreviousClose)
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
            TradeType = 2;  // Crossing short
            sprintf_s(message, "Sell Order #%d (Crossing MiddleATR Down): Size=%d, Price=%.4f, ATR=%.4f", 
                     TradeCount, PositionSize, ClosingPrice, ATR[sc.Index]);
            sc.AddMessageToLog(message, 0);
        }
        else if (Result < 0)
        {
            sprintf_s(message, "Sell Order Failed: Error %d", Result);
            sc.AddMessageToLog(message, 1);
        }
    }

    if (sc.Index == sc.ArraySize - 1)
    {
        float WinRate = TradeCount > 0 ? (float)WinningTrades / (WinningTrades + LosingTrades) * 100.0f : 0.0f;
        sprintf_s(message, "Backtest Results: TotalProfit=%.2f, Trades=%d, WinRate=%.1f%%, MaxDD=%.2f", 
                 TotalProfit, TradeCount, WinRate, MaxDrawdown);
        sc.AddMessageToLog(message, 0);
    }
}
