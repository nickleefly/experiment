#include "sierrachart.h"

SCDLLName("ATR Based Trading Bot")

    SCSFExport scsf_ATRBasedTradingBot(StudyInterfaceRef sc)
{
    // Define persistent variables
    SCSubgraphRef BuySignal = sc.Subgraph[0];
    SCSubgraphRef SellSignal = sc.Subgraph[1];

    // Input variables
    SCInputRef ATRPeriod = sc.Input[0];

    if (sc.SetDefaults)
    {
        sc.GraphName = "ATR Based Trading Bot";
        sc.AutoLoop = 1;

        // Initialize inputs
        ATRPeriod.Name = "ATR Period";
        ATRPeriod.SetInt(14);

        // Initialize subgraphs
        BuySignal.Name = "Buy Signal";
        BuySignal.DrawStyle = DRAWSTYLE_ARROW_UP;
        BuySignal.PrimaryColor = RGB(0, 255, 0);

        SellSignal.Name = "Sell Signal";
        SellSignal.DrawStyle = DRAWSTYLE_ARROW_DOWN;
        SellSignal.PrimaryColor = RGB(255, 0, 0);

        return;
    }

    // Calculate ATR
    SCFloatArray TrueRange;
    sc.TrueRange(TrueRange);
    SCFloatArray ATR;
    sc.MovingAverage(TrueRange, ATR, MOVAVGTYPE_SIMPLE, ATRPeriod.GetInt());

    // Calculate middle ATR
    float MiddleATR = ATR[sc.Index] / 2.0f;

    // Get the last closing price
    float ClosingPrice = sc.Close[sc.Index];

    // Check if any attached orders have been filled to flatten the position
    s_SCPositionData PositionData;
    sc.GetTradePosition(PositionData);
    if (PositionData.PositionQuantity != 0)
    {
        s_SCTradingOrderFillData LastFill;
        if (sc.GetOrderFillEntry(PositionData.LastFillOrderID, LastFill) != 0)
        {
            // Flatten the position if an attached order is filled
            sc.FlattenAndCancelAllOrders();
        }
    }

    // Buy Signal when price closes above Middle ATR
    if (ClosingPrice > MiddleATR)
    {
        BuySignal[sc.Index] = ClosingPrice;

        // Submit a buy order with attached stop loss and target
        s_SCNewOrder NewOrder;
        NewOrder.OrderQuantity = 1;
        NewOrder.OrderType = SCT_ORDERTYPE_MARKET;

        // Attach target order
        NewOrder.Target1Offset = ATR[sc.Index] * 2.0f; // Target at 2 ATR
        NewOrder.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT;

        // Attach stop loss order
        NewOrder.StopAllOffset = ATR[sc.Index]; // Stop loss at 1 ATR
        NewOrder.AttachedOrderStopAllType = SCT_ORDERTYPE_STOP;

        // Submit the buy order
        int Result = sc.BuyEntry(NewOrder);
    }
    // Sell Signal when price closes below Middle ATR
    else if (ClosingPrice < MiddleATR)
    {
        SellSignal[sc.Index] = ClosingPrice;

        // Submit a sell order with attached stop loss and target
        s_SCNewOrder NewOrder;
        NewOrder.OrderQuantity = 1;
        NewOrder.OrderType = SCT_ORDERTYPE_MARKET;

        // Attach target order
        NewOrder.Target1Offset = ATR[sc.Index] * 2.0f; // Target at 2 ATR
        NewOrder.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT;

        // Attach stop loss order
        NewOrder.StopAllOffset = ATR[sc.Index]; // Stop loss at 1 ATR
        NewOrder.AttachedOrderStopAllType = SCT_ORDERTYPE_STOP;

        // Submit the sell order
        int Result = sc.SellEntry(NewOrder);
    }
}