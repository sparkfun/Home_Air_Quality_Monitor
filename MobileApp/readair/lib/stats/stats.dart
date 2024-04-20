import 'package:flutter/material.dart';
import 'package:readair/data/packet.dart';
import 'package:readair/stats/graph.dart';

class StatsPage extends StatefulWidget {
  @override
  State<StatsPage> createState() => _StatsPageState();
}

class _StatsPageState extends State<StatsPage> {
  List<DataPacket> packets = [];

  @override
  void initState() {
    super.initState();
    _fetchData();
  }

  Future<void> _fetchData() async {
    packets = await _fetchLastFivePackets(); // Fetch the last packets
    setState(() {});
  }

  Future<List<DataPacket>> _fetchLastFivePackets() async {
    // Fetch the last five packets from your database
    // This is just a placeholder, replace with your actual data fetching logic
    List<DataPacket> packets = await DatabaseService.instance.getLastFivePackets();
    return packets;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () => Navigator.of(context).pop(),
        ),
        title: Text("Graphical Data"),
      ),
      body: SingleChildScrollView(
        child: Padding(
          padding: EdgeInsets.all(16),
          child: Column(
            children: [
              // Passing data to GraphWidget
              GraphWidget(title: "Temperature", dataPoints: _convertToDataPoints(packets, (p) => p.temp)),
              GraphWidget(title: "AQI", dataPoints: _convertToDataPoints(packets, (p) => p.aqi)),
              GraphWidget(title: "CO2", dataPoints: _convertToDataPoints(packets, (p) => p.co2)),
            ],
          ),
        ),
      ),
    );
  }

  // Helper method to convert packets to DataPoint list
  List<DataPoint> _convertToDataPoints(List<DataPacket> packets, double Function(DataPacket) valueExtractor) {
    return packets
        .map((packet) => DataPoint(valueExtractor(packet), packet.epochTime.toInt()))
        .toList();
  }
}
