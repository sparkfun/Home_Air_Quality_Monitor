import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'dart:math';
import 'package:readair/data/packet.dart';

class StatsPage extends StatefulWidget {
  @override
  State<StatsPage> createState() => _StatsPageState();
}

class _StatsPageState extends State<StatsPage> {
  List<DataPacket> packets = [];
  final Random _random = Random();

  @override
  void initState() {
    super.initState();
    _fetchData();;
  }

  Future<List<DataPacket>> _fetchLastFivePackets() async {
  List<DataPacket> packets = await DatabaseService.instance.getLastFivePackets(); // Implement this method in your DatabaseService
  return packets;
}

  Future<void> _fetchData() async {
    packets = await _fetchLastFivePackets();
    setState(() {});
  }
  
  List<FlSpot> generateRandomData() {
    return List.generate(10, (index) => FlSpot(index.toDouble(), _random.nextInt(90) + 10.0));
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
              _buildGraph("Temperature", packets.map((e) => e.temp).toList()),
              _buildGraph("AQI", packets.map((e) => e.aqi).toList()),
              _buildGraph("CO2", packets.map((e) => e.co2).toList()),
            ],
          ),
        ),
      ),
    );
  }

  Widget _buildGraph(String title, List<double> values) {
    return Column(
      children: [
        Padding(
          padding: const EdgeInsets.symmetric(vertical: 8.0),
          child: Text(title, style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold)),
        ),
        Container(
          height: 300,
          child: LineChart(
            LineChartData(
              gridData: FlGridData(show: false),
              titlesData: FlTitlesData(
                leftTitles: AxisTitles(
                  sideTitles: SideTitles(showTitles: true, reservedSize: 40),
                ),
                bottomTitles: AxisTitles(
                  sideTitles: SideTitles(
                    showTitles: true,
                    reservedSize: 20,
                    getTitlesWidget: (value, meta) => Text('${value.toInt()}'),
                  ),
                ),
              ),
              borderData: FlBorderData(show: true),
              lineBarsData: [
                LineChartBarData(
                  spots: _createSpots(values),
                  isCurved: true,
                  dotData: FlDotData(show: false),
                  belowBarData: BarAreaData(show: false),
                  color: Colors.blue,
                  barWidth: 3,
                ),
              ],
            ),
          ),
        ),
      ],
    );
  }

  List<FlSpot> _createSpots(List<double> values) {
    return List.generate(values.length, (index) => FlSpot(index.toDouble(), values[index]));
  }
}
