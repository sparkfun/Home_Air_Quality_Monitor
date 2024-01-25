import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:intl/intl.dart';
import 'dart:math';


class DataPoint {
  final double measurement;
  final int epoch;

  DataPoint(this.measurement, this.epoch);
}

class GraphWidget extends StatelessWidget {
  final String title;
  final List<DataPoint> dataPoints;

  const GraphWidget({Key? key, required this.title, required this.dataPoints}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    // Ensuring only 100 datasets are shown
    final points = dataPoints.length > 100 ? dataPoints.sublist(0, 100) : dataPoints;
    final minY = 0.0;
    final maxY = points.isNotEmpty 
                 ? points.map((e) => e.measurement).reduce(max) + 100.0
                 : 100.0; 

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
                    getTitlesWidget: (value, meta) {
                      final DateTime date = DateTime.fromMillisecondsSinceEpoch(points[value.toInt()].epoch * 1000);
                      return Padding(
                        padding: const EdgeInsets.only(top: 10.0),
                        child: Text(DateFormat('MM/dd').format(date), style: TextStyle(fontSize: 10)),
                      );
                    },
                  ),
                ),
              ),
              borderData: FlBorderData(show: true),
              lineBarsData: [
                LineChartBarData(
                  spots: _createSpots(points),
                  isCurved: true,
                  dotData: FlDotData(show: true, getDotPainter: (spot, percent, barData, index) {
                    return FlDotCirclePainter(
                      radius: 4,
                      color: Colors.blue,
                      strokeWidth: 1.5,
                      strokeColor: Colors.white,
                    );
                  }),
                  belowBarData: BarAreaData(show: false),
                  color: Colors.blue,
                  barWidth: 3,
                ),
              ],
              minY: minY,
              maxY: maxY,
              lineTouchData: LineTouchData(
                touchTooltipData: LineTouchTooltipData(
                  tooltipBgColor: Colors.blueAccent,
                  getTooltipItems: (touchedSpots) {
                    return touchedSpots.map((touchedSpot) {
                      final DateTime date = DateTime.fromMillisecondsSinceEpoch(points[touchedSpot.spotIndex].epoch * 1000);
                      return LineTooltipItem(
                        '${touchedSpot.y} at ${DateFormat('MM/dd/yyyy').format(date)}',
                        const TextStyle(color: Colors.white),
                      );
                    }).toList();
                  },
                ),
              ),
            ),
          ),
        ),
      ],
    );
  }

  List<FlSpot> _createSpots(List<DataPoint> points) {
    return List.generate(points.length, (index) => FlSpot(index.toDouble(), points[index].measurement));
  }
}
